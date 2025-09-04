/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohanafi <yohanafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 17:46:56 by yohanafi          #+#    #+#             */
/*   Updated: 2025/06/24 12:35:39 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/**
 *  @brief lazy initialization
 */
Client::Client(){
    _status = INTERNAL_SERVER_ERROR;
}

/**
 *  @brief manage the interactions with one particular client (server side)
 *  @param serverfd the file descriptor of the listening socket of the server
 *  @param maxBodySize the maximum number of character accepted in the body of a request
 */
Client::Client(Socket serverSocket, ServerSettings config) : _config(config){

    _status = _socket.acceptMaster(serverSocket);
    if (_status != SUCCESS){
        std::cerr << "[CLIENT] socket error = " << _status << std::endl;
        throw SocketException();
    }
    _status = CONTINUE;
}

Client::~Client() {}

/**
 *  @brief generate the HTTP response to a HTTP request
 *  @return the status of the request
 */
int Client::processing()
{
    if (_status == CONTINUE)
        _status = _request.readSocket(_socket, _config.getMaxBodySize());
    else if (_status == PARTIAL_SEND)
        _status = _socket.sendMsg();
    else {
        _status = buildResponse();
        _socket.setBuffer(_response.toString());
    }

    if (_status == CLIENT_LEFT || _status == FILE_CLOSED || _status == RECV_ERROR || _status == SEND_ERROR){
        return REMOVE_CLIENT;
    }
    return _status;
}

int Client::buildResponse() {
    if (_status == CLIENT_LEFT || _status == FILE_CLOSED || _status == RECV_ERROR || _status == SEND_ERROR)
        return _status;
    if (_status != SUCCESS && _status != OK){
        _response = HttpFormOut(_status, _config.getErrors());
        return PARTIAL_SEND;
    }

    std::string   method = _request.getMethod();
    Location      location = _config.findAndGetLocation(_request.getLocationPath());
	if (location.isRedirection()) {
		_response = HttpFormOut("text/html", "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>301 Moved Permanently</title></head><body><h1>Moved Permanently</h1><p>The document has moved <a href=\"" + location.getRedirPath() + "\">here</a>.</p></body></html>", 301, location.getRedirPath());
        return PARTIAL_SEND;
    }
	else {
		size_t pos = location.getPath().size();
    	_request.setPath(location.getRoot() + _request.getLocationPath().substr(pos));

    	HttpFormOut (Client::*handle[])(HttpFormIn & in) \
            = {&Client::callCGI, &Client::handleGet, &Client::handlePost, &Client::handleDelete};
	    std::string methods[] = {"CGI", "GET", "POST", "DELETE"};

    	int handleMethod = 0;
	    while (handleMethod < 4) {
		    if (method == methods[handleMethod])
			    break;
		    handleMethod++;
	    }

	    if (Utils::isAnExecutable(_request.getPath().c_str()))
		    handleMethod = 0;

        switch(handleMethod) {
        
	    case 0:
            _response = (this->*handle[CGI])(_request);
            break;
        
	    case 1:
	        if (location.isGetAllowed())
            	_response = (this->*handle[GET])(_request);
	        else
		        goto method_not_allowed;
            break;
        
	    case 2:
	        if (location.isPostAllowed())
            	_response = (this->*handle[POST])(_request);
	        else
		        goto method_not_allowed;
            break;
        
	    case 3:
	        if (location.isDeleteAllowed())
            	_response = (this->*handle[DELETE])(_request);
	        else
		        goto method_not_allowed;
            break;
        
	    default:
	    method_not_allowed:
            _response = HttpFormOut(METHOD_NOT_ALLOWED, _config.getErrors());
    	}
    }
    return PARTIAL_SEND;
}

int Client::getfd(){
    return _socket.getfd();
}

const ServerSettings & Client::getConfig() const{
    return _config;
}

const Socket & Client::getSocket() const{
    return _socket;
}

const HttpFormIn & Client::getRequest() const{
    return _request;
}

const HttpFormOut & Client::getResponse() const{
    return _response;
}

const char* Client::SocketException::what() const throw(){
    return "No socket in constructor of class Client";
}

std::ostream & operator<<(std::ostream & COUT,  const Client & client){

    COUT << "[CLIENT]" << std::endl;
    COUT << client.getConfig() << client.getSocket() << std::endl;
    COUT << client.getRequest() << client.getResponse();
    return COUT;
}
