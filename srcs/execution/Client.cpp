/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohanafi <yohanafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 17:46:56 by yohanafi          #+#    #+#             */
/*   Updated: 2025/05/16 11:48:23 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/**
 *  @brief manage the interactions with one particular client (server side)
 *  @param serverfd the file descriptor of the listening socket of the server
 *  @param maxBodySize the maximum number of character accepted in the body of a request
 */
Client::Client(int serverfd, Server config) : _serverfd(serverfd), _config(config){
    try{
        _socket = new Socket(_serverfd);
    }
    catch (std::exception &e){
        std::cerr << e.what() << std::endl;
        throw SocketException();
    }
    std::cout << "new client connected to server with fd " << _socket->getfd();
    std::cout << " | port : " << ntohs(_socket->getAddr().sin_port);
    std::cout << " | s_addr : " << _socket->getAddr().sin_addr.s_addr << std::endl;
    // if (_fd < 0)
    //     std::cerr << "wrong fd" << std::endl;//silence error _fd_client not used
}

Client::~Client() {
    // delete _socket; //vu erreur dans ServerExec, on verra ici plus tard
}

 /**
 *  @brief read un file HTML, if don't exist
 *  @return On success, return the file html
 * 
 *  On error, return static 404 html
*/
 std::string    Client::readHtmlFile (const std::string &Filename)
{
    std::ifstream   file(Filename.c_str());
    if (!file.is_open())
        return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/**
 *  @brief parse the request and search the first occurance espace for separate the METHOD, ex: GET /index.html HTTP/1.1
 *  @return On success, the method
 * 
 *  On error, ""
 */
 static std::string getContentType(const std::string& filename) {
    std::string lowercaseFilename = filename;
    for (size_t i = 0; i < lowercaseFilename.size(); ++i) {
        lowercaseFilename[i] = std::tolower(lowercaseFilename[i]);
    }

    if (lowercaseFilename.find(".html") != std::string::npos || lowercaseFilename.find(".htm") != std::string::npos)
        return "text/html";
    else if (lowercaseFilename.find(".css") != std::string::npos)
        return "text/css";
    else if (lowercaseFilename.find(".js") != std::string::npos)
        return "application/javascript";
    else if (lowercaseFilename.find(".jpg") != std::string::npos || lowercaseFilename.find(".jpeg") != std::string::npos)
        return "image/jpeg";
    else if (lowercaseFilename.find(".png") != std::string::npos)
        return "image/png";
    else if (lowercaseFilename.find(".gif") != std::string::npos)
        return "image/gif";
    else if (lowercaseFilename.find(".ico") != std::string::npos)
        return "image/x-icon";
    else if (lowercaseFilename.find(".txt") != std::string::npos)
        return "text/plain";
    else if (lowercaseFilename.find(".pdf") != std::string::npos)
        return "application/pdf";
    else if (lowercaseFilename.find(".json") != std::string::npos)
        return "application/json";
    else if (lowercaseFilename.find(".xml") != std::string::npos)
        return "application/xml";
    // Fallback par défaut
    return "text/plain";
}
 std::string    Client::buildHttpResponse (const std::string method, const std::string path)
 {
    std::string Filename;

    if (method == "GET") {
        if (path == "/" || path.empty()) {
            Filename = "index.html"; // Page par défaut
        } else if (path == "../images/") {
            Filename = "cutecat.jpg";
        } else {
            Filename = path.substr(1); // Enlever le "/" initial: /reset.css => reset.css
        }
    }

    std::string body = readHtmlFile(Filename);
    if (body.empty())
    {
        std::string notFoundBody = readHtmlFile(Filename);
        if (notFoundBody.empty())
            notFoundBody = "<html><body><h1>404 Not Found</h1></body></html>";
        
        std::ostringstream oss;
        oss << notFoundBody.size();
        std::string sizeStr = oss.str();
        
        std::string response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + sizeStr + "\r\n"
            "Connection: close\r\n"
            "\r\n" + notFoundBody;
        
        return response;
    
    }
    std::ostringstream oss;
    oss << body.size();
    std::string sizeStr = oss.str();

    std::string contentType = getContentType(Filename);

    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: " + contentType + "\r\n"
        "Content-Length: " + sizeStr + "\r\n"
        "Connection: close\r\n"
        "\r\n" + body;

    return response;
 }

int Client::request(){
    HttpRequest *http_request;
    try{
        http_request = new HttpRequest(_socket->getfd(), _config);
    }
    catch (std::exception &e){
        std::cerr << "bad request: " << e.what() << std::endl;
        return -1;
    }
 
    std::string response;
    if ((*http_request).get_protocol() == "HTTP/1.1")
        response = buildHttpResponse((*http_request).get_method(), (*http_request).get_path());
    else if ((*http_request).get_protocol() == "HTTP/1.0")
        response = buildHttpResponse("GET", (*http_request).get_path());
    else
        response = "errorbadrequest()";

    if((*http_request).get_protocol() == "file closed")
        return CLIENT_LEFT;
    
    send(_socket->getfd(), response.c_str(), response.length(), 0);
    // if (response == badrequest)
    //     removeclient

    return 0;
}

int Client::getfd(){
    return _socket->getfd();
}

const char* Client::SocketException::what() const throw(){
    return "No socket in constructor of class Client";
}

bool Client::isValidPath(const char* dir) {
	
	struct stat sb;

	if (stat(dir, &sb) == 0)
		return true;
	
	return false;
}

bool Client::isValidFile(const char *file) {
	struct stat sb;
	
	if (stat(file, &sb) == 0 && !(sb.st_mode & S_IFDIR))
		return true;
	
	return false;
}

bool Client::isAnExecutable(const char *exec) {

	struct stat sb;

	if (stat(exec, &sb) == 0 && !(sb.st_mode & S_IFDIR))

		if (sb.st_mode & S_IXUSR || sb.st_mode & S_IXGRP || sb.st_mode & S_IXOTH)
			return true;
	return false;
}

