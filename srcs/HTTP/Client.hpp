#pragma once
#include "../EPOLL/Socket.hpp"
#include "../NGINX/ServerSettings.hpp"
#include "HttpForm.hpp"
#include "HttpFormIn.hpp"
#include "HttpFormOut.hpp"

class Client
{
    private:
        ServerSettings      _config;
        Socket              _socket;
        int                 _status;
        HttpFormIn          _request;
        HttpFormOut         _response;

    public:
        Client();
        Client (Socket serverSocket, ServerSettings config);
        ~Client(void);

        int             processing();
        int             buildResponse();
        void            setupMsg(Socket & socket);
        HttpFormOut     handleGet(HttpFormIn & request);
        HttpFormOut     handlePost(HttpFormIn & request);
        HttpFormOut     handleDelete(HttpFormIn & request);
        HttpFormOut     callCGI(HttpFormIn & request);

        //accessors
        int     getfd();
        const ServerSettings & getConfig() const;
        const Socket & getSocket() const;
        const HttpFormIn & getRequest() const;
        const HttpFormOut & getResponse() const;

        enum Methods {
		CGI,
		GET,
		POST,
		DELETE
	};

        class SocketException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
};

std::ostream & operator<<(std::ostream & COUT,  const Client & client);
