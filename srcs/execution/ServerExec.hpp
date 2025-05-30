#pragma once
#include "../../inc/Libraries.h"
#include "../configuration/Location.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "../configuration/Server.hpp"

class Client;//silence error, not proud

class ServerExec {

    private:
        Server                          _config;
        Socket*                         _socket;
		int                             _epfd;
        std::map<const int, Client*>    _clients;

        int setup_epoll();
        int accept_new_client(int epfd, int sockfd);

	public:
        ServerExec(Server & config);
        ~ServerExec();

        int		client_processing();
        void	cleanup();

        const Server  & getConfig() const;

        class SocketException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
        class EpollException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
};
