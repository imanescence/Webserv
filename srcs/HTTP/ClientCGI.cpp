#include "Client.hpp"

HttpFormOut Client::callCGI(HttpFormIn & request){
    std::string script = request.getPath();
    std::string query = request.getQueryString() + request.getBody();
    std::string protocol = request.getProtocol();
    std::string method = request.getMethod();
    if (method != "GET" && method != "POST"){
        return HttpFormOut(METHOD_NOT_ALLOWED, _config.getErrors());
    }

    Interface cgi(method, script, query, protocol, request.getLen());	
    int status = cgi.call();
    if (status != SUCCESS){
        return HttpFormOut(status, _config.getErrors());
    }
    return HttpFormOut("text/html", cgi.getResultCGI());
}
