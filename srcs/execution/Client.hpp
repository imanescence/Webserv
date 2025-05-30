#pragma once
#include "../../inc/Libraries.h"
#include "../../inc/define.h"
#include "Socket.hpp"
#include "../configuration/Server.hpp"
#include "../HTTP/HttpRequest.hpp"

class Client
{
    private:
        int                 _serverfd;//-> Socket _server ??
        Server              _config;
        Socket*             _socket;
        // ssize_t             _maxBodySize;

        std::string readHtmlFile(const std::string &Filename);
        std::string buildHttpResponse(const std::string method, const std::string path);

    public:
        Client (int serverfd, Server config);
        ~Client(void);

        int     request();
        int     getfd();

        // methods to check if the files and paths
        bool isValidPath(const char *path);
        bool isValidFile(const char *file);
        bool isAnExecutable(const char *exec);
        
        class SocketException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
};
