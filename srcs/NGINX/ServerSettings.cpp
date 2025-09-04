/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSettings.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohanafi <yohanafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 17:15:30 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/06/17 16:13:01 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSettings.hpp"

ServerSettings::ServerSettings() {
	_root = "";
	_serverName = "";
	_port = 0;
	_maxBodySize = 0;
	_ip = 0;
}

ServerSettings::ServerSettings(const std::string serverName) : _serverName(serverName) {
	_root = "";
	_port = 0;
	_maxBodySize = 0;
    	_serverName = "";
	_ip = 0;
}

ServerSettings::ServerSettings(const std::string name, const std::string root, int port, int maxBodySize, in_addr_t ip) : _serverName(name), _root(root), _port(port), _maxBodySize(maxBodySize), _ip(ip) {
        
	_loc.insert(std::pair<std::string, Location>("/", Location("www/", "/", true, false, false, true, "", false, "")));
	_loc.insert(std::pair<std::string, Location>("/images/", Location("www/images/", "/images/", true, true, true, true, "www/images/clientUploadFiles", false, "")));
	_loc.insert(std::pair<std::string, Location>("/upload/", Location("www/images/", "/upload/", true, true, true, true, "www/images/clientUploadFiles", false, "")));
	_loc.insert(std::pair<std::string, Location>("/cgi/", Location("www/script/", "/cgi/", true, true, true, false, "", false, "")));
        
	_error.insert(std::pair<int, std::string>(400, "www/html/400.html"));
	_error.insert(std::pair<int, std::string>(403, "www/html/403.html"));
	_error.insert(std::pair<int, std::string>(404, "www/html/404.html"));
	_error.insert(std::pair<int, std::string>(405, "www/html/405.html"));
	_error.insert(std::pair<int, std::string>(500, "www/html/500.html"));
	_error.insert(std::pair<int, std::string>(502, "www/html/502.html"));
	_error.insert(std::pair<int, std::string>(424, "www/html/424.html"));
}

ServerSettings::~ServerSettings(){}

std::ostream& operator<<(std::ostream& out, const ServerSettings& server) {
	
	out << "Server Name : " 	<< server._serverName 		<< "\n";
	out << "Root : " 		<< server._root 		<< "\n";
	out << "Port : " 		<< server._port 		<< "\n";
	out << "Max Body Size (Mo) : " 	<< server._maxBodySize 		<< "\n";
	out << "Host IP : " 		<< server._ip 			<< "\n";
	
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

const std::string ServerSettings::getName() const {	
	return _serverName;
}

const std::string ServerSettings::getRoot() const {
	return _root;
}

int ServerSettings::getPort() const {
	return _port;
}

int ServerSettings::getMaxBodySize() const {
	return _maxBodySize;
}

in_addr_t ServerSettings::getIpAddr() const {
	return _ip;
}

Location ServerSettings::findAndGetLocation(std::string path) const{
		
	while (true) {
		
		std::map<std::string, Location>::const_iterator it = _loc.find(path);
		
		if (it != _loc.end()) {
			return it->second;
		}
		
		if (path.empty() || path == "/") {
			break;
		}
		
		if (path[path.size() - 1] == '/') {
			path.erase(path.size() - 1);
		}

		std::size_t pos = path.rfind('/');
		
		if (pos == std::string::npos) {
			break;
		}
		
		path.erase(pos + 1);
	}
    
    return Location();
}

void ServerSettings::setIpAddr(int byte[4]) {
	_ip = (byte[0] << 24) | (byte[1] << 16) | (byte[2] << 8) | byte[3];
}

const std::map<int, std::string> & ServerSettings::getErrors() const{
	return _error;
}

std::map<std::string, Location> ServerSettings::getLocation() const {
	return _loc;
}

void ServerSettings::setName(const std::string& serverName) {
	_serverName = serverName;
}

void ServerSettings::setRoot(const std::string& root) {
	_root = root;
}

void ServerSettings::setPort(int port) {
	_port = port;
}

void ServerSettings::setMaxBodySize(int maxBodySize) {
	_maxBodySize = maxBodySize * 1048576;
}

void ServerSettings::addError(int code, const std::string& path) {
	_error[code] = path;
}

void ServerSettings::addLocation(const std::string& path, Location& loc) {
	_loc[path] = loc;
}

