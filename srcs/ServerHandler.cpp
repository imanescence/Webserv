/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iait-ouf <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 12:39:15 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/05/15 18:45:17 by iait-ouf         ###   ########.fr       */
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
	
	std::map<std::string, Server>& config_servers = config.getServers();

	bool exception_caught = false;
	for (std::map<std::string, Server>::iterator it = config_servers.begin(); it != config_servers.end(); ++it){
		ServerExec *srv;
		try{
			srv = new ServerExec(it->second);
			std::map<std::string, Location> locations = srv->getConfig().getLocation();
			std::cout << "ServerHandler: Number of locations: " << locations.size() << std::endl;
		}
		catch(std::exception &e){
			std::cerr << e.what() << std::endl;
			exception_caught = true;
		}
		if (exception_caught)
			exception_caught = false;
		else
			_servers.insert(_servers.begin(), *srv);
	}
	
	while (_servers.size() > 0)
		for (std::vector<ServerExec>::iterator it = _servers.begin(); it != _servers.end(); ++it)
			(*it).client_processing();
}

void ServerHandler::cleanup(){
	for (std::vector<ServerExec>::iterator it = _servers.begin(); it != _servers.end(); it++)
		it->cleanup();
	_servers.clear();
}
