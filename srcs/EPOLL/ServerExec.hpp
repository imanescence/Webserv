#pragma once
#include "../NGINX/Location.hpp"
#include "../HTTP/Client.hpp"
#include "Socket.hpp"
#include "../NGINX/ServerSettings.hpp"

class ServerExec {

    private:
        ServerSettings                  _config;
        Socket                          _socket;
        std::map<const int, Client>     _clients;

        bool    setup_epoll(int epfd);
        int     accept_new_client(int epfd);

	public:
        ServerExec(ServerSettings & config, int epfd);
        ~ServerExec();

        void	handleEvent(int epfd, struct epoll_event & event);
        void	cleanup();

        const ServerSettings  & getConfig() const;

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
