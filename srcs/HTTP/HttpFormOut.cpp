#include "HttpFormOut.hpp"

HttpFormOut::HttpFormOut() : HttpForm() {
    _code = 0;
    _status = "";
}

HttpFormOut::HttpFormOut(int error, const std::map<int, std::string> & errors) : HttpForm() {
    _code = error;
    if (error == SUCCESS || error == OK){
        _code = 200;
        _status = "OK";
    }
    if (error == BAD_REQUEST)
        _status = "Bad Request";
    else if (error == PAYMENT_REQUIRED)
        _status = "Payment Required";
    else if (error == FORBIDDEN)
        _status = "forbidden";
    else if (error == NOT_FOUND)
        _status = "Not Found";
    else if (error == METHOD_NOT_ALLOWED)
        _status = "Method Not Allowed";
    else if (error == LENGTH_REQUIRED)
        _status = "Length Required";
    else if (error == PAYLOAD_TOO_LARGE)
        _status = "Payload Too Large";
    else if (error == URI_TOO_LONG)
        _status = "URI Too Long";
    else if (error == METHOD_FAILURE)
        _status = "methode failure";
    else if (error == REQUEST_HEADER_FIELDS_TOO_LARGE)
        _status = "Request Header Fields Too Large";
    else if (error == INTERNAL_SERVER_ERROR)
        _status = "internal serveur error";
    else if (error == BAD_GATEWAY)
        _status = "Bad Gateway";
    else if (error == NOT_IMPLEMENTED)
        _status = "Not Implemented";
    else if (error == GATEWAY_TIME_OUT)
        _status = "Gateway Time Out";
    else
        _status = "Undefined";
    _protocol = "HTTP/1.1";
    _body = read_error_file(errors);
    _headers.insert(std::pair<std::string, std::string>("Content-Type", "text/html"));
    std::stringstream length;
    length << _body.size();
    _headers.insert(std::pair<std::string, std::string>("Content-Length", length.str()));
    _headers.insert(std::pair<std::string, std::string>("Connection", "close"));
    _headers.insert(std::pair<std::string, std::string>("Server", "WebServ/1.3"));

}

HttpFormOut::HttpFormOut(std::string contentType, const std::string & body) : HttpForm() {
    _protocol = "HTTP/1.1";
    _code = 200;
    _status = "OK";
    _body = body;
    _headers.insert(std::pair<std::string, std::string>("Content-Type", contentType));
    std::stringstream length;
    length << _body.size();
    _headers.insert(std::pair<std::string, std::string>("Content-Length", length.str()));
    _headers.insert(std::pair<std::string, std::string>("Connection", "close"));
    _headers.insert(std::pair<std::string, std::string>("Server", "WebServ/1.3"));
}
HttpFormOut::HttpFormOut(std::string contentType, const std::string & body, int code, const std::string& redirectPath) {

    _protocol = "HTTP/1.1";
    _code = code;
    _status = "Moved Permanently";
    _body = body;
    _headers.insert(std::pair<std::string, std::string>("Content-Type", contentType));
    std::stringstream length;
    length << _body.size();
    _headers.insert(std::pair<std::string, std::string>("Content-Length", length.str()));
    _headers.insert(std::pair<std::string, std::string>("Connection", "close"));
    _headers.insert(std::pair<std::string, std::string>("Server", "WebServ/1.3"));
    _headers.insert(std::make_pair("Location", redirectPath));
}

HttpFormOut::~HttpFormOut(){}

void HttpFormOut::clean(){
    _protocol = "";
    _startLine = "";
    _body = "";
    _code = 0;
    _status = "";
}

const std::string HttpFormOut::read_error_file(const std::map<int, std::string> & errors){
    std::map<int, std::string>::const_iterator errorPage = errors.find(_code);
    if (errorPage == errors.end())
        return "error not supported by configuration";
    std::string fileName = errorPage->second;
    std::ifstream   file(fileName.c_str());
    std::string     body;
    if (!file.is_open()){
        std::stringstream ss;
        ss << _code;
        return "error " + ss.str() + " : " + _status;
    }
    std::ostringstream  buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string HttpFormOut::toString() {
    std::stringstream response;
    
    response << _protocol << " " << _code << " " << _status << "\r\n";
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++){
        response << it->first << ": " << it->second << "\r\n";
    }
    response << "\r\n" << _body;

    std::string responseStr = response.str();
    return responseStr;
}

const std::string & HttpFormOut::getStatus() const{
    return _status;
}

int HttpFormOut::getCode() const{
    return _code;
}

std::ostream & operator<<(std::ostream & COUT, const HttpFormOut & httpform) {

    int printBody = false;

	COUT << "[HttpFormOut]" << std::endl;
    COUT << "    Protocol: " << httpform.getProtocol() << std::endl;
    COUT << "    Code: " << httpform.getCode() << std::endl;
    COUT << "    Message: " << httpform.getStatus() << std::endl;

    COUT << "[HEADERS]" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = httpform.getHeaders().begin(); it != httpform.getHeaders().end(); it++){
        COUT << "    " << it->first << ": " << it->second << std::endl;
    }

    COUT << "[BODY]";
    if (printBody)
        COUT << std::endl << httpform.getBody() << std::endl;
    else
        COUT << " change value of printBody to see body" << std::endl;

    return COUT;
}
