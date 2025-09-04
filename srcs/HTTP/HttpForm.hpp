#pragma once
#include "../NGINX/ServerSettings.hpp"
#include "../CGI/Interface.hpp"
#include "../EPOLL/Socket.hpp"
#include "../utlls/Utils.hpp"

class HttpForm
{
    protected:
        std::string                         _protocol;
        std::string                         _startLine;
        std::map<std::string, std::string>  _headers;
        std::string                         _body;

    public:
        HttpForm();
        HttpForm & operator=(const HttpForm &rhs);
        ~HttpForm();

        //accessors
        const std::string & getProtocol() const;
        const std::string & getStartLine() const;
        const std::string & getBody() const;
        const std::map<std::string, std::string> & getHeaders() const;
        int getLen() const;
        void setBody(const std::string & body);
};
