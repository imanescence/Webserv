#include "HttpRequest.hpp"

int HttpRequest::parseHeaders(){
    char*   msg;
    int     status;

    msg = get_next_line(_fd);
    while (msg != 0 && strcmp(msg, "\r\n") != 0){
        std::cout << "Header line: " << msg << std::endl; // DEBUG
        status = parseHeader(msg);
        if (status == -1){
            std::string blame(msg);
            _error = "failed to parse header: " + blame;
            free(msg);
            return -1;
        }
        free(msg);
        msg = get_next_line(_fd);
    }
    
    if (!msg){
        _error = "no end of headers found";
        return -1;
    }
    free(msg);
    return 0;
}

int HttpRequest::parseHeader(char *msg){
    std::string request(msg, strlen(msg));
    size_t semicolon;
    size_t end;

    semicolon = request.find(':');
    if (semicolon == std::string::npos)
        return -1;
    end = request.find('\r');
    if (end == std::string::npos)
        return -1;
    _headers.insert(std::pair<std::string, std::string>(request.substr(0, semicolon), request.substr(semicolon + 2, end - semicolon - 2)));
    return 0;
}

int HttpRequest::parseRequest(char *msg){
    std::string request(msg, strlen(msg));
    size_t start;
    size_t end;

    start = 0;
    end = request.find(' ');
    if (end == std::string::npos)
        return -1;
    _method = request.substr(start, end - start);

    start = end + 1;
    end = request.rfind(' ');
    if (start == end){
        _path = _method;
        _method = "GET";
        _protocol = "HTTP/1.0";
        std::cout << "bad request";
        return -1;
    }
    else if (find_location(request.substr(start, end - start)) == -1){
        std::cout << "bad request" << std::endl;
        return -1;
    }

    start = end + 1;
    end = request.find('\r');
    if (end == std::string::npos)
        return -1;
    _protocol = request.substr(start, end - start);

    return 0;
}

int HttpRequest::find_location(std::string request_uri) {
    if (request_uri.find('/') == std::string::npos)
        return -1;

    std::map<std::string, Location> locations = _config.getLocation();
    std::string bestMatch;
    size_t bestLength = 0;

    for (std::map<std::string, Location>::iterator it = locations.begin(); it != locations.end(); ++it) {
        const std::string& locPath = it->first;

        // On vérifie que c'est un vrai préfixe
        if (request_uri.compare(0, locPath.length(), locPath) == 0) {
            if (locPath.length() > bestLength) {
                bestLength = locPath.length();
                bestMatch = locPath;
                _location = it->second;
            }
        }
    }

    if (bestMatch.empty()) {
        return -1;
    }

    _path = request_uri.substr(_location.getPath().length());
    return 0;
}

bool HttpRequest::parseBody(){
    if (_len > 0) {
        std::cout << "Reading body of " << _len << " bytes..." << std::endl;
        
        char *body = new char[_len + 1];
        int totalRead = 0;
        int bytesRead = 0;
        
        // AJOUT: Timeout pour éviter les blocages
        struct timeval tv;
        tv.tv_sec = 30;  // 30 secondes de timeout
        tv.tv_usec = 0;
        setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
        
        // Lire le corps complet
        while (totalRead < _len) {
            bytesRead = read(_fd, body + totalRead, _len - totalRead);
            if (bytesRead < 0) {
                std::cerr << "Error reading body: " << strerror(errno) << std::endl;
                delete[] body;
                return false;
            }
            if (bytesRead == 0) {
                std::cerr << "Connection closed while reading body, read " << totalRead << "/" << _len << " bytes" << std::endl;
                delete[] body;
                return false;
            }
            totalRead += bytesRead;
            std::cout << "Read " << bytesRead << " bytes, total: " << totalRead << "/" << _len << std::endl;
        }
        
        body[_len] = '\0';
        _content = std::string(body, _len);
        delete[] body;
        
        std::cout << "Body read successfully: " << _content.length() << " bytes" << std::endl;
    }
    else {
        _content = "";
        std::cout << "No body to read (Content-Length = 0)" << std::endl;
    }
    return true;
}

std::string HttpRequest::parseScript(){
    size_t questionmark = _path.find('?');
    if (questionmark == std::string::npos)
        return "";
    return _path.substr(0, questionmark);
}

std::string HttpRequest::parseQuery(){
    size_t questionmark = _path.find('?');
    if (questionmark == std::string::npos)
        return "";
    return _path.substr(questionmark + 1, _path.size() - questionmark);
}
