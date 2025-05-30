#include "HttpRequest.hpp"

std::string HttpRequest::get_method(){
    return _method;
}

std::string HttpRequest::get_path(){
    return _path;
}

std::string HttpRequest::get_protocol(){
    return _protocol;
}