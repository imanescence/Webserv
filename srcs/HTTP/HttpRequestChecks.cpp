#include "HttpRequest.hpp"

bool HttpRequest::isValidPath(const char* dir) {

	struct stat sb;

	if (stat(dir, &sb) == 0)
		return true;
	return false;
}

bool HttpRequest::isValidFile(const char *file) {

	struct stat sb;

	if (stat(file, &sb) == 0 && !(sb.st_mode & S_IFDIR)) {
		return true;
	}
	return false;
}

bool HttpRequest::isAnExecutable(const char *exec) {

	struct stat sb;

	if (stat(exec, &sb) == 0 && !(sb.st_mode & S_IFDIR))

		if (sb.st_mode & S_IXUSR || sb.st_mode & S_IXGRP || sb.st_mode & S_IXOTH)
			return true;
	return false;
}

bool HttpRequest::isValidLength(){
    _len = 0;
    std::map<std::string, std::string>::iterator contentLengthIt = _headers.find("Content-Length");
    if (contentLengthIt == _headers.end()){
        std::cout << "No Content-Length header found" << std::endl;
        return false;
    }
    _len = atoi(contentLengthIt->second.c_str());
    std::cout << "Content-Length found: " << _len << std::endl;
    if (_len > _config.getMaxBodySize()) {
        std::cerr << "Request body too large: " << _len << " > " << _config.getMaxBodySize() << std::endl;
        sendReponse(_fd, "413 Payload Too Large", "text/html", 
                   "<html><body><h1>Payload Too Large</h1></body></html>");
        return false;
    }
    if (_len < 0) {
        std::cerr << "Invalid Content-Length: " << _len << std::endl;
        sendReponse(_fd, "400 Bad Request", "text/html", 
                   "<html><body><h1>Invalid Content-Length</h1></body></html>");
        return false;
    }
    return true;
}