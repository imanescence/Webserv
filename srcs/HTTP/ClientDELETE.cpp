#include "Client.hpp"

HttpFormOut    Client::handleDelete(HttpFormIn & request)
{
    if (request.getPath().find("..") != std::string::npos){
        return HttpFormOut(FORBIDDEN, _config.getErrors());
    }

    std::ifstream   file(request.getPath().c_str());
    if (!file.is_open()){
        return HttpFormOut(NOT_FOUND, _config.getErrors());
    }
    file.close();
    if (std::remove(request.getPath().c_str()) != 0){
        return HttpFormOut(METHOD_FAILURE, _config.getErrors());
    }
    std::string succesMessage = "<html><body><h1>delete succes</h1><p>the file " + request.getPath() + " is deleted .</p>";
    return  HttpFormOut("text/html", succesMessage);;
}
