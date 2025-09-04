/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohanafi <yohanafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 12:39:15 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/06/18 11:19:00 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerHandler.hpp"
#include <sys/wait.h>

static ServerHandler* SH_instance = NULL;

void handle_sigint(int signum) {
    std::cout << "\n[INFO] Caught signal " << signum << ". Shutting down...\n";
    if (SH_instance) {
        SH_instance->cleanup();
    }
    exit(0);
}

ServerHandler::ServerHandler(Config* conf) : config(*conf) {}

ServerHandler::~ServerHandler() {}

void ServerHandler::serverStarter() {
    SH_instance = this;
    signal(SIGINT, handle_sigint);

    int epfd = epoll_create(1);
    if (epfd == -1){
        return ;
	}

	std::map<int, ServerSettings>& config_servers = config.getServers();

	bool exception_caught = false;
	for (std::map<int, ServerSettings>::iterator it = config_servers.begin(); it != config_servers.end(); ++it){
		ServerExec *srv;
		try{
			srv = new ServerExec(it->second, epfd);
			std::map<std::string, Location> locations = srv->getConfig().getLocation();
		}
		catch(std::exception &e){
			std::cerr << "[SERVER] " << e.what() << std::endl;
			exception_caught = true;
		}
		if (exception_caught)
			exception_caught = false;
		else
			_servers.insert(_servers.begin(), *srv);
	}

    int maxevents = 10;
    struct epoll_event events[maxevents];
    int timeout = 2000;
	int nevready;
	while (_servers.size() > 0){
		nevready = epoll_wait(epfd, events, maxevents, timeout);
		for (std::vector<ServerExec>::iterator it = _servers.begin(); it != _servers.end(); ++it){
			for (int i = 0; i < nevready; i++){
				(*it).handleEvent(epfd, events[i]);
			}
		}
	}
}

void ServerHandler::cleanup(){
	for (std::vector<ServerExec>::iterator it = _servers.begin(); it != _servers.end(); it++)
		it->cleanup();
	_servers.clear();
}
