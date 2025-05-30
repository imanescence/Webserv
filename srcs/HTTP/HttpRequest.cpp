#include "HttpRequest.hpp"
#include <sys/socket.h>
#include <errno.h>
#include <cstring>  // pour strerror
#include <unistd.h> // pour read()

// Et dans votre HttpRequest.h :
#include <sys/socket.h>

HttpRequest::HttpRequest(){}

HttpRequest::HttpRequest(int fd, Server config) : _fd(fd), _config(config)
{
    char *msg;
    _uploadedFileName = "";
    _uploadedFileContent = "";
    _error = "";

    // AJOUT: Vérifier que le fd est valide
    if (fd < 0) {
        _protocol = "invalid fd";
        _error = "Invalid file descriptor";
        throw RequestException(_error);
    }

    msg = get_next_line(_fd);
    if (!msg){
        std::cout << "terminate connection with client associated to fd " << _fd << std::endl;
        _protocol = "file closed";
        return;
    }

    std::cout << "First line received: " << msg << std::endl; // DEBUG
    
    if (parseRequest(msg) == -1){
        _protocol = "bad request";
        free(msg);
        _error = "failed to parse request line";
        throw RequestException(_error);
    }
    free(msg);

    // Lire les headers
    if (parseHeaders() == -1)
        throw RequestException(_error);

    // CORRECTION 2: Améliorer la gestion du Content-Length
    isValidLength();
    std::cout << "Content-Length = " << _len << std::endl;

    // CORRECTION 3: Améliorer la lecture du body
    if (!parseBody())
        return;

    //DEBUG
    std::cout << "request :" << std::endl;
    std::cout << "    method: " << _method;
    std::cout << " | location: " << _location.getPath() << std::endl;
    std::cout << "    root: " << _location.getRoot();
    std::cout << " | relative path to root: " << _path << std::endl;
    std::cout << "    protocol: " << _protocol << std::endl;
    std::cout << "headers:" << std::endl;

    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
        
    if (_method == "POST") {
        std::cout << "POST body length: " << _content.length() << " bytes" << std::endl;
        if (_content.length() > 0) {
            std::string preview = _content.substr(0, std::min(size_t(200), _content.length()));
            std::cout << "Body preview: " << preview << "..." << std::endl;
        }
    }
    else {
        std::cout << "body:" << std::endl << _content << std::endl;
    }
    std::cout << "end request" << std::endl;
    //END DEBUG
    
    // CORRECTION 4: Vérifier que le parsing s'est bien passé avant de traiter
    if (_protocol == "bad request" || _protocol == "file closed" || _protocol == "invalid fd") {
        std::cerr << "Request parsing failed, not processing" << std::endl;
        return;
    }
    
    if (_location.getPath() == "/cgi/"){
        callCGI();
        return ;
    }
    if (_method == "GET")
        handleGet();
    else if (_method == "POST")
        handlePost();
    else if (_method == "DELETE")
    {
        std::cout << "coucou je remtre oklm :)" << std::endl;
        handleDelete();
    }
    else {
        std::cerr << "Unsupported method: " << _method << std::endl;
        sendReponse(_fd, "405 Method Not Allowed", "text/html", 
                   "<html><body><h1>405 Method Not Allowed</h1></body></html>");
    }
}

void HttpRequest::callCGI(){
    if (_method != "GET"){
        std::cerr << "CGI handle method GET only" << std::endl;
        return;
    }
    std::string script = _location.getRoot() + parseScript();
    std::string protocol = "HTTP/1.1";
    std::string query = parseQuery();
 
    std::cout << _method << protocol << query << script << _location.getRoot() << std::endl; 
    Interface cgi(_method, protocol, query, script, _location.getRoot());
	
    if (!cgi.call())
        std::cerr << "error CGI" << std::endl;
    sendReponse(_fd, "200 OK", "text/html", cgi.getResultCGI());
    std::cout << "the answer is: " << cgi.getResultCGI() << std::endl;
}

void HttpRequest::displayDirList(std::string dirPath) {

	//std::string dirName;
	//dirName =  voulait juste faire en sorte davoir "image" en nom plutot que le path
	std::ostringstream htmlResponse, body;
	
	htmlResponse << "HTTP/1.1 200 OK\r\n";
	htmlResponse << "Contente-Type: test/html\r\n";
	
	body << "<html><head><title> Dictory Listing of : " << dirPath << "</title></head><body>\n";
	body << "<h1> Directory Listing of : " << dirPath << "</h1>\n<ul>\n";
	
	for (size_t i = 0; i < _dirList.size(); ++i) {
		body << "<li><a href=\"" << _dirList[i] << "\">" << _dirList[i] << "</a></li>\n";
	}

	body << "</ul>\n</body></html>\n";

	std::string bodyStr = body.str();

	htmlResponse << "Content-Length: " << bodyStr.size() << "\r\n";
	htmlResponse << "Connection: close\r\n";
	htmlResponse << "\r\n";

	htmlResponse << bodyStr;
	std::string html = htmlResponse.str();
	if (send(_fd, html.c_str(), html.length(), MSG_NOSIGNAL) == -1 && errno == EPIPE) {
		close(_fd);
		std::cerr << "--------------------------- BROKEN PIPE EVITE !!!! --------------------" << std::endl;
	}
}

void HttpRequest::extractDirList(std::string dirPath) {

	if (!isValidPath(dirPath.c_str())) {
		std::cerr << "Error : the directory doesn't exist" << std::endl;
		return;
	}

	DIR* dir = opendir(dirPath.c_str());
	if (!dir) {
		std::cerr << "Error : annot open the directory" << std::endl;
		return;
	}

	struct dirent *listing;
	while ((listing = readdir(dir))) {
		
		std::string nameOfDirOrFile = listing->d_name;
		
		if (nameOfDirOrFile != "." && nameOfDirOrFile != "..")
			_dirList.push_back(nameOfDirOrFile);
	}
	
	displayDirList(dirPath);
	closedir(dir);
	_dirList.clear();
}

HttpRequest::~HttpRequest(){
}

//METHODE HANDLE
void    HttpRequest::handleGet(void)
{
    if (_path.find("..") != std::string::npos){
        std::cerr << "Invalid path (security): "<< _path << std::endl;
        send404();
        return;
    }
    
    // Traiter les requêtes vers la racine
    if (_path.empty()) {
	    // si pas d'index alrs directory list
	    if (!isValidFile((_location.getRoot() + "index.html").c_str()))
			extractDirList(_location.getRoot()); 
	    else
	    	_path = "index.html";
    }
    std::cout << "------------------->>>>>> " << _location.getRoot() << std::endl;
    std::string filePath = _location.getRoot() + _path;
    std::ifstream file(filePath.c_str(), std::ios::binary);

    std::cout << "ooooooooooooooooooo>>>>> " << filePath << std::endl;
    
    if (!file){
        std::cerr << "File not found: " << filePath << std::endl;
        send404();
        return;
    }
    std::cerr << "HELLOOOOOOOOO -------> : " << _path << std::endl;
    std::ostringstream buffer;
    buffer << file.rdbuf();
    _content = buffer.str();
    
    // Déterminer le type MIME
    std::string extension;
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filePath.substr(dotPos + 1);
        // Convertir en minuscules
        for (size_t i = 0; i < extension.size(); ++i) {
            extension[i] = std::tolower(extension[i]);
        }
    }
    
    std::string contentType = "application/octet-stream";
    
    if (extension == "html" || extension == "htm")
        contentType = "text/html";
    else if (extension == "css")
        contentType = "text/css";
    else if (extension == "js")
        contentType = "application/javascript";
    else if (extension == "jpg" || extension == "jpeg")
        contentType = "image/jpeg";
    else if (extension == "png")
        contentType = "image/png";
    else if (extension == "gif")
        contentType = "image/gif";
    else if (extension == "ico")
        contentType = "image/x-icon";
    else if (extension == "txt")
        contentType = "text/plain";
    sendReponse(_fd, "200 OK", contentType, _content);
}

void HttpRequest::handleDelete(void)
{
    if (_path.find("..") != std::string::npos) {
        std::cerr << "Invalid path (security): " << _path << std::endl;
        send404();
        return;
    }

    // Optionnel : retirer un slash initial
    if (!_path.empty() && _path[0] == '/') {
        _path = _path.substr(1);
    }

    std::string fullPath = _location.getRoot() + _path;

    std::cout << "DELETE - Trying to delete: " << fullPath << std::endl;

    std::ifstream file(fullPath.c_str());
    if (!file.is_open()) {
        std::cerr << "DELETE - File not found: " << fullPath << std::endl;
        send404();
        return;
    }
    file.close();

    if (std::remove(fullPath.c_str()) != 0) {
        std::cerr << "DELETE - Failed to delete: " << strerror(errno) << std::endl;
        sendReponse(_fd, "500 Internal Server Error", "text/html",
            "<html><body><h1>500 Internal Server Error</h1><p>Impossible de supprimer le fichier.</p></body></html>");
        return;
    }

    std::cout << "DELETE - File deleted successfully: " << fullPath << std::endl;
    sendReponse(_fd, "200 OK", "text/html",
        "<html><body><h1>Fichier supprimé avec succès</h1><p>Le fichier " + _path + " a été supprimé.</p></body></html>");
}


void    HttpRequest::handlePost(void)
{
    std::cout << "handle post request for path " << _path << std::endl;
    
    if (_path.find("..") != std::string::npos){
        std::cerr << "Invalid path (security): "<< _path << std::endl;
        send404();
        return;
    }
    
    // Construire le chemin complet pour la recherche
    std::string fullPath = "/" + _path;
    if (_path[0] == '/') {
        fullPath = _path; // _path contient déjà le /
    }
    
    std::cout << "Looking for location with path: " << fullPath << std::endl;
    
    // Vérifier si c'est une requête d'upload
    if (fullPath != "/upload") {
        std::cerr << "POST can only handle /upload path, got: " << fullPath << std::endl;
        send400();
        return;
    }
    
    // Pour /upload, utiliser la location racine "/" mais avec le répertoire www/upload/
    int locIndex = find_location("/");
    if (locIndex == -1) {
        std::cerr << "No root location found" << std::endl;
        send404();
        return;
    }
    
    // Obtenir la map des locations et trouver la location racine
    std::map<std::string, Location> locations = _config.getLocation();
    Location location;
    
    // Trouver la location racine "/"
    for (std::map<std::string, Location>::iterator it = locations.begin(); it != locations.end(); ++it) {
        if (it->second.getPath() == "/") {
            location = it->second;
            break;
        }
    }
    
    std::cout << "Using root location with path: " << location.getPath() << std::endl;
    
    std::map<std::string, std::string>::iterator contentTypeIt = _headers.find("Content-Type");
    if (contentTypeIt == _headers.end() || contentTypeIt->second.find("multipart/form-data") == std::string::npos)
    {
        std::cerr << "POST request without multipart/form-data" << std::endl;
        send400();
        return ;
    }
    
    size_t boundaryPos = contentTypeIt->second.find("boundary=");
    if (boundaryPos == std::string::npos) {
        std::cerr << "No boundary found in Content-Type" << std::endl;
        send400();
        return;
    }
    
    std::string boundary = contentTypeIt->second.substr(boundaryPos + 9);
    std::cout << "boundary: " << boundary << std::endl;
    
    if (!parseMultipartFormData(boundary))
    {
        std::cerr << "failed to parse multipart/form-data" << std::endl;
        send400();
        return ;
    }
    
    if (!_uploadedFileName.empty() && !_uploadedFileContent.empty())
    {
        std::string uniqueFileName = generateUniqueFileName(_uploadedFileName);
        
        // Utiliser le répertoire www/upload/ pour sauvegarder le fichier
        std::string uploadPath = location.getRoot() + "upload/" + uniqueFileName;
        saveUploaderFile(uploadPath, _uploadedFileContent);
        
        std::string succesMessage = "<html><body><h1>Upload réussi</h1><p>Le fichier " + 
                                    _uploadedFileName + " a été uploadé avec succès sous le nom " + 
                                    uniqueFileName + ".</p><p><a href=\"/\">Retour à l'accueil</a></p>";
        
        std::string extension = uniqueFileName.substr(uniqueFileName.find_last_of('.') + 1);
        for (size_t i = 0; i < extension.size(); i++)
            extension[i] = std::tolower(extension[i]);
        
        if (extension == "jpg" || extension == "jpeg" || extension == "png")
            succesMessage += "<p><img src=\"../www/images/" + uniqueFileName + "\" alt=\"Uploaded Image\" style=\"max-width: 500px;\"></p>";
        
        succesMessage += "</body></html>";
        sendReponse(_fd, "200 OK", "text/html", succesMessage);
    }
    else
        send404();
}

// ------WebKitFormBoundary7MA4YWxkTrZu0gW
// Content-Disposition: form-data; name="username"

// john_doe
// ------WebKitFormBoundary7MA4YWxkTrZu0gW
// Content-Disposition: form-data; name="profile_pic"; filename="photo.jpg"
// Content-Type: image/jpeg

// (binary data here...)
// ------WebKitFormBoundary7MA4YWxkTrZu0gW--

bool HttpRequest::parseMultipartFormData(const std::string& boundary)
{
    std::string startBoundary = "--" + boundary;
    std::string endBoundary = "--" + boundary + "--";
    
    size_t pos = 0;
    
    // Trouver le premier boundary
    size_t nextBoundaryPos = _content.find(startBoundary, pos);
    if (nextBoundaryPos == std::string::npos) {
        std::cerr << "No start boundary found" << std::endl;
        return false;
    }
    
    // Parcourir chaque partie
    while (nextBoundaryPos != std::string::npos) {
        // Déplacer pos après le boundary actuel
        pos = nextBoundaryPos + startBoundary.length();
        
        // Ignorer les caractères de nouvelle ligne après le boundary
        while (pos < _content.length() && (_content[pos] == '\r' || _content[pos] == '\n'))
            pos++;
        
        // Trouver le prochain boundary
        size_t nextPos = _content.find(startBoundary, pos);
        
        // Si on ne trouve pas de prochain boundary, chercher le boundary de fin
        if (nextPos == std::string::npos) {
            nextPos = _content.find(endBoundary, pos);
            if (nextPos == std::string::npos) {
                // Prendre le reste du contenu
                nextPos = _content.length();
            }
        }
        
        // Extraire la partie entre les boundaries
        std::string part = _content.substr(pos, nextPos - pos);
        
        // Trouver la fin des headers (double CRLF)
        size_t headersEnd = part.find("\r\n\r\n");
        if (headersEnd == std::string::npos) {
            std::cerr << "No headers end found in part" << std::endl;
            // Essayer avec juste \n\n au cas où
            headersEnd = part.find("\n\n");
            if (headersEnd == std::string::npos)
                return false;
        }
        
        std::string headers = part.substr(0, headersEnd);
        std::string partContent;
        
        if (headersEnd + 4 < part.length())
            partContent = part.substr(headersEnd + 4); // +4 pour "\r\n\r\n"
        else if (headersEnd + 2 < part.length())
            partContent = part.substr(headersEnd + 2); // +2 pour "\n\n"
        
        // Nettoyer le contenu (enlever le CRLF final)
        while (!partContent.empty() && (partContent[partContent.length()-1] == '\r' || partContent[partContent.length()-1] == '\n'))
            partContent = partContent.substr(0, partContent.length() - 1);
        
        std::cout << "Processing part with headers: " << headers << std::endl;
        std::cout << "Content length: " << partContent.length() << std::endl;
        
        // Parser les headers de cette partie
        size_t contentDispositionPos = headers.find("Content-Disposition:");
        if (contentDispositionPos != std::string::npos) {
            std::string contentDisposition = headers.substr(contentDispositionPos);
            size_t contentDispositionEnd = contentDisposition.find("\r\n");
            if (contentDispositionEnd == std::string::npos)
                contentDispositionEnd = contentDisposition.find("\n");
            if (contentDispositionEnd != std::string::npos)
                contentDisposition = contentDisposition.substr(0, contentDispositionEnd);
            
            std::cout << "Content-Disposition: " << contentDisposition << std::endl;
            
            // Vérifier s'il s'agit d'un fichier
            if (contentDisposition.find("filename=") != std::string::npos) {
                _uploadedFileName = extractFileName(contentDisposition);
                _uploadedFileContent = partContent;
                std::cout << "Found file: " << _uploadedFileName << " with size " << partContent.length() << " bytes" << std::endl;
            }
            // Sinon, c'est un champ de formulaire
            else if (contentDisposition.find("name=") != std::string::npos) {
                size_t nameStart = contentDisposition.find("name=") + 5;
                if (nameStart < contentDisposition.length() && contentDisposition[nameStart] == '"')
                    nameStart++;
                size_t nameEnd = contentDisposition.find("\"", nameStart);
                if (nameEnd == std::string::npos)
                    nameEnd = contentDisposition.find(";", nameStart);
                if (nameEnd == std::string::npos)
                    nameEnd = contentDisposition.length();
                
                if (nameStart < nameEnd) {
                    std::string fieldName = contentDisposition.substr(nameStart, nameEnd - nameStart);
                    _formFields[fieldName] = partContent;
                    std::cout << "Found field: " << fieldName << " = " << partContent << std::endl;
                }
            }
        }
        
        // Passer au prochain boundary
        nextBoundaryPos = nextPos;
        
        // Si on a trouvé le boundary de fin, on s'arrête
        if (nextPos < _content.length() && _content.substr(nextPos, endBoundary.length()) == endBoundary) {
            std::cout << "Found end boundary, stopping" << std::endl;
            break;
        }
    }
    
    return true;
}


std::string HttpRequest::extractFileName(const std::string& contentDisposition)
{
    size_t filenamePos = contentDisposition.find("filename=");
    if (filenamePos == std::string::npos)
        return "";
    size_t filenameStart = filenamePos + 9;
    if (filenameStart < contentDisposition.length() && contentDisposition[filenameStart] == '"')
        filenameStart++;
    size_t filenameEnd = contentDisposition.find("\"", filenameStart);
    if (filenameEnd == std::string::npos)
        filenameEnd = contentDisposition.find(";", filenameStart);
    if (filenameEnd == std::string::npos)
        filenameEnd = contentDisposition.length();
    if (filenameStart >= filenameEnd)
        return "";
    return contentDisposition.substr(filenameStart, filenameEnd - filenameStart);
}

void HttpRequest::saveUploaderFile(const std::string& filePath, const std::string& fileContent)
{
    std::ofstream file(filePath.c_str(), std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "fail to open file or writing: " << filePath << std::endl;
        return ;
    }
    file.write(fileContent.c_str(), fileContent.length());
    file.close();
    std::cout << "Saved file to: " << filePath << " with size " << fileContent.length() << " bytes" << std::endl;
}

std::string HttpRequest::generateUniqueFileName(const std::string& originalFilename)
{
    size_t dotPos = originalFilename.find_last_of(".");
    std::string extension = "";
    std::string baseName = originalFilename;
    if (dotPos != std::string::npos)
    {
        extension = originalFilename.substr(dotPos);
        baseName = originalFilename.substr(0, dotPos);
    }
    time_t now = time(NULL);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", localtime(&now));
    return baseName + "-" + timestamp + extension;
}

// REPONSE FUNCTION()

void HttpRequest::sendReponse(int fd, const std::string& status, const std::string& contentType, const std::string& body) {
    if (fd < 0) {
        std::cerr << "Invalid fd for response: " << fd << std::endl;
        return;
    }
    
    std::ostringstream response;
    
    response << "HTTP/1.1 " << status << "\r\n";
    response << "Content-Type: " << contentType << "\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "Server: WebServ/1.0\r\n";
    response << "\r\n";
    response << body;
    
    std::string responseStr = response.str();
    
    // AJOUT: Vérifier que la socket est encore ouverte
    int error = 0;
    socklen_t len = sizeof(error);
    int result = getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len);
    
    if (result != 0 || error != 0) {
        std::cerr << "Socket error before sending response: " << strerror(error) << std::endl;
        return;
    }
    
    ssize_t bytesSent = send(fd, responseStr.c_str(), responseStr.length(), MSG_NOSIGNAL);
    if (bytesSent == -1) {
	std::cerr << "Failed to send response: " << strerror(errno) << std::endl;
	if (errno == EPIPE) {
		std::cerr << "--------------------------- BROKEN PIPE EVITE !!!! --------------------" << std::endl;
		close(_fd);
	}
    } else if (bytesSent != (ssize_t)responseStr.length()) {
        std::cerr << "Partial response sent: " << bytesSent << "/" << responseStr.length() << " bytes" << std::endl;
    } else {
        std::cout << "Response sent successfully: " << bytesSent << " bytes" << std::endl;
        std::cout << "Status: " << status << std::endl;
    }
}

void    HttpRequest::send400(void)
{
    std::ifstream   file("html/400.html");
    std::string     body;

    if (!file.is_open())
    {
        body = "file 400 not found :D";
        send404();
    }
    else
    {
        std::ostringstream  buffer;
        buffer << file.rdbuf();
        body = buffer.str();
    }
    sendReponse(_fd, "400 Bad Request", "text/html", body);
}

void    HttpRequest::send404(void)
{
    std::ifstream   file("html/404.html");
    std::string     body;

    if (!file.is_open())
        body = "file 404 not found :D";
    else
    {
        std::ostringstream buffer;
        buffer << file.rdbuf();
        body = buffer.str();
    }
    sendReponse(_fd, "404 not found", "text/html", body);
}

const char* HttpRequest::ParsingException::what() const throw(){
    return "Unexpected error in parsing HTTP request";
}

const char* HttpRequest::MaxBodySizeException::what() const throw(){
    return "body size not consistant with header information";
}

const char* HttpRequest::RequestException::what() const throw(){
    return _msg.c_str();
}

HttpRequest::RequestException::RequestException(std::string & msg) : _msg(msg){}

HttpRequest::RequestException::~RequestException() throw(){}
