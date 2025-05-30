/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohanafi <yohanafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 17:15:30 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/05/19 18:42:53 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {
	_root = "";
	_serverName = "";
	_port = 0;
	_maxBodySize = 0;
}

Server::Server(const std::string serverName) : _serverName(serverName) {
	_root = "";
	_port = 0;
	_maxBodySize = 0;
    _serverName = "";
}

Server::Server(const std::string name, const std::string root, int port, int maxBodySize) : _serverName(name), _root(root), _port(port), _maxBodySize(maxBodySize) {
        
	_loc.insert(std::pair<std::string, Location>("Images", Location("/images/", "web_repo/images/", "web_repo/images/index.html", false, false, false, false)));
    _loc.insert(std::pair<std::string, Location>("API", Location("/api/", "web_repo/api/", "web_repo/api/index.html", true, true, true, true)));
    _loc.insert(std::pair<std::string, Location>("Upload", Location("/upload/", "web_repo/upload/", "web_repo/upload/index.html", true, true, true, false)));
        
	_error.insert(std::pair<int, std::string>(400, "web_repo/errors/400.html"));
    _error.insert(std::pair<int, std::string>(403, "web_repo/errors/403.html"));
    _error.insert(std::pair<int, std::string>(404, "web_repo/errors/404.html"));
    _error.insert(std::pair<int, std::string>(405, "web_repo/errors/405.html"));
    _error.insert(std::pair<int, std::string>(500, "web_repo/errors/500.html"));
    _error.insert(std::pair<int, std::string>(502, "web_repo/errors/502.html"));
}

Server::~Server(){}

std::ostream& operator<<(std::ostream& out, const Server& server) {
	
	out << "Server Name : " 	<< server._serverName 	<< "\n";
	out << "Root : " 		<< server._root 		<< "\n";
	out << "Port : " 		<< server._port 		<< "\n";
	out << "Max Body Size (Mo) : " 	<< server._maxBodySize 	<< "\n";
	
	out << "\n------------- ... WITH LOCATIONS... -----------\n";
	
	for (std::map<std::string, Location>::const_iterator it = server._loc.begin(); it != server._loc.end(); ++it) {
		out << "\nLocation -> " << it->first << "\n";
		out << it->second;
	}
	
	out << "\n ----------- ... AND ERROR PAGES -------- \n";
	
	for (std::map<int, std::string>::const_iterator it = server._error.begin(); it != server._error.end(); ++it) {
		out << "Error [num of error][path of the file to handle it] -> " << it->first << "|" << it->second << "\n";
	}

    return out;
}

const std::string Server::getName() const {	
	return _serverName;
}

const std::string Server::getRoot() const {
	return _root;
}

int Server::getPort() const {
	return _port;
}

int Server::getMaxBodySize() const {
	return _maxBodySize;
}

std::map<std::string, Location> Server::getLocation() const{
	return _loc;
}

void Server::setName(const std::string& serverName) {
	_serverName = serverName;
}

void Server::setRoot(const std::string& root) {
	_root = root;
}

void Server::setPort(int port) {
	_port = port;
}

void Server::setMaxBodySize(int maxBodySize) {
	_maxBodySize = maxBodySize * 1048576;
}

void Server::addError(int code, const std::string& path) {
	_error[code] = path;
}

void Server::addLocation(const std::string& path, Location& loc) {
	_loc[path] = loc;
}

