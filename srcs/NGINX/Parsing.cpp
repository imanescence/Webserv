/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohanafi <yohanafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 11:44:33 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/06/18 10:58:37 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parsing.hpp"
#include "../utlls/Utils.hpp"

Parsing::Parsing () {
	
	keywordSrvHandler["listen"] = &Parsing::handlePort;
	keywordSrvHandler["server_name"] = &Parsing::handleServerName;
	keywordSrvHandler["root"] = &Parsing::handleRoot;
	keywordSrvHandler["client_max_body_size"] = &Parsing::handleBodySize;
	keywordSrvHandler["error_page"] = &Parsing::handleErrorPages;
	keywordSrvHandler["host"] = &Parsing::handleHost;
	
	keywordLocHandler["method"] = &Parsing::handleMethods;
	keywordLocHandler["root"] = &Parsing::handleRoot;
	keywordLocHandler["client_body_temp_path"] = &Parsing::handleClientTmpBodyPath;
	keywordLocHandler["auto-index"] = &Parsing::handleAutoIndex;
	keywordLocHandler["return"] = &Parsing::handleRedirect;
	lineNumber = 0;
}

std::string Parsing::removeSemicolon(const std::string& src) {
	
	std::string cleanStr;

	return (cleanStr = src.substr(0, src.length() - 1));
}


void Parsing::handleHost(const std::string& value, ServerSettings& tmpServer) {
	
	std::string host = removeSemicolon(value);

	if (tmpServer.getIpAddr()) {
		wrongConfig = true;
		std::cerr << "Line " << lineNumber << " -> Error : you cannot add another host to the server" << std::endl;
		return;
	}
        
	int byte[4];

        std::stringstream ss(host);
        std::string seg;
        int i = 0;

        while (std::getline(ss, seg, '.') && i < 4) {
                
		std::stringstream segStream(seg);
                segStream >> byte[i];
		
		if (seg.empty()) {
			wrongConfig = true;
			std::cerr << "Line " << lineNumber << " -> Error : IP Address must contain only digits" << std::endl;
			return;

		}

		for (size_t i = 0; i < seg.length(); ++i) {
		
			if (!std::isdigit(seg[i])) {
				wrongConfig = true;
				std::cerr << "Line " << lineNumber << " -> Error : IP Address must contain only digits" << std::endl;
				return;
			}
		}

	
		if (byte[i] < 0 || byte[i] > 256) {
			wrongConfig = true;
			std::cerr << "Line " << lineNumber << " -> Error : one or more bytes of IP address are out of range" << std::endl;
			return;
		}
                ++i;
        }
	
	if (i != 4) {

		wrongConfig = true;
		std::cerr << "Line " << lineNumber << " -> Error : wrong address IP format" << std::endl;
        	return;
	}
	
	tmpServer.setIpAddr(byte);

}


void Parsing::handleRoot(const std::string& value, Location& tmpLoc) {
	
	std::string root = removeSemicolon(value);

	if (!tmpLoc.getRoot().empty()) {
		wrongConfig = true;
		std::cerr << "Line " << lineNumber << " -> Error : you cannot add another root to the location" << std::endl;
		return;
	}

	else
		tmpLoc.setRoot(root);

}


void Parsing::handleAutoIndex(const std::string& value, Location& tmpLoc) {
	
	std::string on = removeSemicolon(value);
	if (tmpLoc.isAutoIndexOn() == true) {
		wrongConfig = true;
		std::cerr << "Line " << lineNumber << " -> Error : you cannot multiply the status of auto-index" << std::endl;
		return;
	}
	if (on != "on") {
		wrongConfig = true;
		std::cerr << "Line " << lineNumber << " -> Error : auto-index has to be 'on' or commented : '" << on << "' doesn't match anything" << std::endl;
		return;
	}

	tmpLoc.setAutoIndex();
}


void Parsing::handlePort(const std::string& value, ServerSettings& tmpServer) {
	
	std::istringstream iss(value);
    	int port;
    	iss >> port; 
	
	if (tmpServer.getPort()) {
		wrongConfig = true;
		std::cerr << "Line " << lineNumber << " -> Error : you cannot add another port to the server" << std::endl;
		return;
	}

	if (!isValidPort(port)) {
		wrongConfig = true;
		std::cerr << "Line " << lineNumber << " -> Error : port out of range : '" << port << "'" << std::endl;
		return;
	}

	tmpServer.setPort(port);

}

void Parsing::handleClientTmpBodyPath(const std::string& value, Location& tmpLoc) {

	std::string clientDirPath = removeSemicolon(value);
	
	if (!tmpLoc.getClientBodyTmpPath().empty()) {
		wrongConfig = true;
		std::cerr << "Line " << lineNumber << " -> Error : only one directory for uploads in each location" << std::endl;
		return;
	}
	
	tmpLoc.setClientDir(clientDirPath);
}

void Parsing::handleServerName(const std::string& value, ServerSettings& tmpServer) {
	
	std::string name = removeSemicolon(value);
	
	if (!tmpServer._serverName.empty()) {
		wrongConfig = true;
		std::cerr << "Line " << lineNumber << " -> Error : only one name for the server" << std::endl;
		return;
	}
	
	tmpServer.setName(name);
}

void Parsing::handleRoot(const std::string& value, ServerSettings& tmpServer) {
	
	std::string path = removeSemicolon(value);

	if (!tmpServer.getRoot().empty()) {
		wrongConfig = true;
		std::cerr << "Line " << lineNumber << " -> Error : only one root by server" << std::endl;
		return;
	}
	
	tmpServer.setRoot(path);
}

void Parsing::handleBodySize(const std::string& value, ServerSettings& tmpServer) {

	std::istringstream iss(value);
    	int bodySize;
	iss >> bodySize;
	
	if (tmpServer.getMaxBodySize()) {
		wrongConfig = true;
		std::cerr << "Line " << lineNumber << " -> Error : no redefinition of the client max body size possible" << std::endl;
		return;
	}

	if (!isValidSize(bodySize)) {
		wrongConfig = true;
		std::cerr << "Line " << lineNumber << " -> Error : client_max_body_size is out of range '" << bodySize << "'" << std::endl;
		return;
	}
	
	tmpServer.setMaxBodySize(bodySize);
}

void Parsing::handleMethods(const std::string& value, Location& tmpLoc) {

	std::string methodName = removeSemicolon(value);

	void (Location::*methodsSetters[])(void) = { &Location::setGet, &Location::setPost, &Location::setDelete};
	std::string methods[] = {"GET", "POST", "DELETE"};	
	
	int method = 0;	
	while (method < 3) {
		if (methodName == methods[method])		
			break;
		method++;
	}

	switch (method) {
		case GET:
			(tmpLoc.*methodsSetters[GET])();
			break;
		case POST:
			(tmpLoc.*methodsSetters[POST])();
			break;
		case DELETE:
			(tmpLoc.*methodsSetters[DELETE])();
			break;
		default:
			(void)tmpLoc;
			wrongConfig = true;
			std::cerr << "Line " << lineNumber << " -> Error : the method :'" << methodName << "' does not exist" << std::endl;
	}
}

bool Parsing::isValidErrorCode(int code) {

	switch (code) {
		case 400:
			return true;
		case 402:
			return true;
		case 403:
			return true;
		case 404:
			return true;
		case 405:
			return true;
		case 411:
			return true;
		case 413:
			return true;
		case 414:
			return true;
		case 424:
			return true;
		case 431:
			return true;
		case 500:
			return true;
		case 501:
			return true;
		case 502:
			return true;
		case 504:
			return true;
		default:
			return false;
	}
}

bool Parsing::isValidRedirect(int code) {

	switch (code) {
		case 301:
			return true;
		default:
			return false;
	}
}

void Parsing::handleRedirect(const std::string& value, Location& tmpLoc) {

	std::string path = removeSemicolon(value);
	std::string codeStr;
	size_t i = 0;

	while (i < path.length() && std::isdigit(path[i])) {
		codeStr += path[i];
		i++;
	}

	path = path.substr(i, std::string::npos);
	int code = std::atoi(codeStr.c_str());
	
	if (tmpLoc.isRedirection()) {
		std::cerr << "Line" << lineNumber << " -> Error : a redirection is already setup for this location" << std::endl;
		wrongConfig = true;
		return;
	}

	if (!isValidRedirect(code)) {
		std::cerr << "Line" << lineNumber << " -> Error : no redirection code found for '" << code << "'" << std::endl;
		wrongConfig = true;
		return;
	}
	
	tmpLoc.setRedirectPath(path);

}


void Parsing::handleErrorPages(const std::string& value, ServerSettings &tmpServer) {

	std::string path = removeSemicolon(value);
	std::string codeStr;
	size_t i = 0;

	while (i < path.length() && std::isdigit(path[i])) {
		codeStr += path[i];
		i++;
	}

	path = path.substr(i, std::string::npos);
	int code = std::atoi(codeStr.c_str());
	
	if (!isValidErrorCode(code)) {
		std::cerr << "Line" << lineNumber << " -> Error : no error found for '" << code << "'" << std::endl;
		wrongConfig = true;
		return;
	}

	if (!Utils::isValidExtension(path, ".html")) {
		std::cerr << "Line " << lineNumber << " -> Error : wrong file format '" << path << "'" << std::endl;
		wrongConfig = true;
		return;
	}
	
	if (tmpServer._error.find(code) != tmpServer._error.end()) {
		wrongConfig = true;
    		std::cerr << "Line " << lineNumber << " -> Error : no redefinition of html file for '" << code << "' possible" << std::endl;
		return;
	} 
	
	tmpServer.addError(code, path);
}


bool Parsing::isValidPort(int port) {
	
	return port >= 1 && port <= 65535;
}


bool Parsing::isValidSize(int maxBodySize) {
	
	return maxBodySize >= 1 && maxBodySize <= 100;
}

bool Parsing::lineToSkip(const std::string& line)
{
	size_t firstPrintableOfTheLine = line.find_first_not_of(" \t\n\r\f\v");

	if (firstPrintableOfTheLine == std::string::npos || line[firstPrintableOfTheLine] == '#')
		return true;
	return false;
}

Parsing::~Parsing() {};

bool Parsing::isValidSyntax(const std::string& line) {

	std::istringstream iss(line);
	std::string keyword;
	std::string value;
	std::string intruder;
	
	iss >> keyword >> value;
	if ((keyword == "error_page" || keyword == "return" ) && iss >> intruder) 
		value = value + intruder;

	if (value.empty()) {

		std::cerr << "Line " << lineNumber << " -> Error : no value given after '" << keyword << "'" << std::endl;
		wrongConfig = true;
		return false;
	}
	
	if (value[value.size() - 1] != ';') {
		std::cerr << "Line " << lineNumber << " -> Error : each value must terminate by a -> ';'" << std::endl;
		wrongConfig = true;
		return false;
	}
	
	if (iss >> intruder) {

		std::cerr << "Line " << lineNumber << " -> Error : intruder word found after keyword and value : '" << intruder << "'" << std::endl;
		wrongConfig = true;
		return false;
	}
	
	return true;
}

void Parsing::isInsideALocBlock(const std::string& line, ServerSettings& tmpServer, Location& tmpLoc) {

	(void)tmpServer;
	(void)tmpLoc;
	
	std::istringstream iss(line);
	std::string keyword;
	std::string value;
	std::string brace;

	iss >> keyword >> value >> brace;
	
	if (brace == "{" && inServer)
		inLocation = true;
	else {
		wrongConfig = true;
		std::cerr << "Line " << lineNumber << " -> Error : location is block and must be declare with braces to set up the values inside" << std::endl;
	}
}


std::string Parsing::trimLeft(const std::string& str) {

	size_t start = 0;
	
	while (start < str.length() && std::isspace(str[start]))
        	++start;

	return str.substr(start);
}

bool Parsing::isServerLaunchable(ServerSettings& tmpServer) {
	
	if (tmpServer.getIpAddr() == 0) {
		std::cerr << "Error : host(IP address) non initialized" << std::endl;
		return false;
	}

	if (tmpServer.getMaxBodySize() == 0) {
		std::cerr << "Error : client max body size non initialized" << std::endl;
		return false;
	}

	if (tmpServer.getPort() == 0) {
		std::cerr << "Error : port non initialized" << std::endl;
		return false;
	}

	const std::map<std::string, Location>& locScope = tmpServer.getLocation();	
	std::map<std::string, Location>::const_iterator it = locScope.find("/");
	if (it == locScope.end()) {
		std::cerr << "Error : one root location must be defined" << std::endl;
		return false;
	}
	else if (it != locScope.end()) {
		if (!it->second.isGetAllowed()) {
			std::cerr << "Error : the root location must allow GET" << std::endl;
			return false;
		}
	}

	std::map<std::string, Location>::const_iterator i;
	for (i = locScope.begin(); i != locScope.end(); ++i) {
		if (i->second.getRoot().empty()) {
			std::cerr << "Error : each location must have a root path defined" << std::endl;
			return false;
		}
	}
	
	return true;
}

void Parsing::parseFile(const std::string& fileName, Config& config) {

	std::ifstream in(fileName.c_str());
	std::string line;

	while (std::getline(in, line)) {
		
		std::string trimmedLine = trimLeft(line);
		
		if (lineToSkip(trimmedLine))
	        	continue;
		
		std::istringstream iss(trimmedLine);
		std::string keyword;
		std::string value;
		std::string extra;
		iss >> keyword >> value >> extra;
		
		if (keyword == "server") {
			if (value == "{") {
				inServer = true;
				inLocation = false;
				wrongConfig = false;
				tmpServer = ServerSettings();
				continue;
			}
			else {
				wrongConfig = true;
			}
		}
		
		if (keyword == "location") {
			isInsideALocBlock(trimmedLine, tmpServer, tmpLoc);
			tmpLoc = Location(value);
			continue;
		}
	
		if (keyword == "}") {
			if (inLocation) {
				inLocation = false;
				if (!wrongConfig) 
					tmpServer.addLocation(tmpLoc.getPath(), tmpLoc);
			}
			else if (inServer) {
				inServer = false;
	            		if (!wrongConfig) {
					if (!isServerLaunchable(tmpServer)) {
						std::cerr << "Server '" << tmpServer.getName() << "' cannot be launched" << std::endl;
						exit (EXIT_FAILURE);
					}
					config.addServer(tmpServer.getPort(), tmpServer);
				}
		    		else {
			    		std::cerr << "Error : server didn't create because the configuration is not valid" << std::endl;
					exit (EXIT_FAILURE);
				}
			}
			continue;
		}
		
		if (inServer) {
			if (!isValidSyntax(trimmedLine)){
				wrongConfig = true;
				continue;
			}
	        }
		
		if (inLocation) {
			std::map<std::string, keywordLocHandling>::iterator it = keywordLocHandler.find(keyword);
			if (it != keywordLocHandler.end()) {
				if (keyword == "return") 
					value = value + extra;
				keywordLocHandling handlingMethod = it->second;
				(this->*handlingMethod)(value, tmpLoc); 
			} else {
				std::cerr << "Line " << lineNumber << " -> Error : unknow keyword : '" << keyword << "'" << std::endl;
				wrongConfig = true;
			}
		}
		else {
			std::map<std::string, keywordSrvHandling>::iterator it = keywordSrvHandler.find(keyword);
			if (it != keywordSrvHandler.end()) {
				if (keyword == "error_page") 
					value = value + extra;
				keywordSrvHandling handlingMethod = it->second;
				(this->*handlingMethod)(value, tmpServer); 
			} else {
				std::cerr << "Line " << lineNumber << " -> Error : unknow keyword : '" << keyword << "'" << std::endl;
				wrongConfig = true;
			}
		}
		lineNumber++;
	}
}
