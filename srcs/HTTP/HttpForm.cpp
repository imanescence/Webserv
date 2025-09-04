#include "HttpForm.hpp"

HttpForm::HttpForm(){
    _protocol = "";
    _startLine = "";
    _body = "";
}

HttpForm & HttpForm::operator=(const HttpForm &rhs){
    if (&rhs == this)
        return *this;
    for (std::map<std::string, std::string>::const_iterator it = rhs._headers.begin(); it != rhs._headers.end(); it++){
        _headers.insert(*it);
    }
    _protocol = rhs._protocol;
    _body = rhs._body;
    return *this;
}

HttpForm::~HttpForm(){
}

const std::string & HttpForm::getProtocol() const{
    return _protocol;
}

const std::string & HttpForm::getStartLine() const {
    return _startLine;
}

const std::string& HttpForm::getBody() const{
    return _body;
}

void HttpForm::setBody(const std::string & body){
    _body = body;
}

const std::map<std::string, std::string> & HttpForm::getHeaders() const{
    return _headers;
}

int HttpForm::getLen() const{
    std::map<std::string, std::string>::const_iterator contentLengthNode = _headers.find("Content-Length");
    if (contentLengthNode == _headers.end())
        return -1;
    int len = atoi(contentLengthNode->second.c_str());
    if (len < 0) {
        std::cerr << "[HTTP] Invalid Content-Length: " << len << std::endl;
        return -2;
    }
    return len;
}
