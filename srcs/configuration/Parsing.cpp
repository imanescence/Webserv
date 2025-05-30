/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iait-ouf <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 11:44:33 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/05/20 12:36:30 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parsing.hpp"

Parsing::Parsing () {
	
	keywordSrvHandler["listen"] = &Parsing::handlePort;
	keywordSrvHandler["server_name"] = &Parsing::handleServerName;
	keywordSrvHandler["root"] = &Parsing::handleRoot;
	keywordSrvHandler["client_max_body_size"] = &Parsing::handleBodySize;
	keywordSrvHandler["error_page"] = &Parsing::handleErrorPages;
	
	//keywordLocHander["script"] = &Parsing::handleScript;
	keywordLocHandler["method"] = &Parsing::handleMethods;
	keywordLocHandler["root"] = &Parsing::handleRoot;
	//keywordsManager["cgi"] = &Parsing::handleCgi;
}

std::string Parsing::removeSemicolon(const std::string& src) {
	
	std::string cleanStr;

	return (cleanStr = src.substr(0, src.length() - 1));
}



void Parsing::handleRoot(const std::string& value, Location& tmpLoc) {

	//std::string root = value.substr(0, value.length() - 1);
	std::string root = removeSemicolon(value);

	std::cout << "KINSIKIDNSID" << root << std::endl;
	if (!tmpLoc.getRoot().empty()) {
		wrongConfig = true;
		std::cout << "Error : you cannot add another root to the location" << std::endl;
		return;
	}
	else
		tmpLoc.setRoot(root);

}



void Parsing::handlePort(const std::string& value, Server& tmpServer) {
	
	if (tmpServer.getPort()) {
		wrongConfig = true;
		std::cout << "Error : you cannot add another port to the server" << std::endl;
		return;
	}

	std::istringstream iss(value);
    	int port;
    	iss >> port; 
	
	if (isValidPort(port))
		tmpServer.setPort(port);
	else {
		std::cout << "Error : port out of range : '" << port << "'" << std::endl;
		wrongConfig = true;

	}
}


void Parsing::handleServerName(const std::string& value, Server& tmpServer) {
	std::string name = value.substr(0, value.length() - 1);
	if (!tmpServer._serverName.empty())
		std::cout << "Error : only one name for the server" << std::endl;
	tmpServer.setName(name);
}

void Parsing::handleRoot(const std::string& value, Server& tmpServer) {
	
	std::string path = value.substr(0, value.length() - 1);
	if (!tmpServer.getRoot().empty()) {
		wrongConfig = true;
		std::cout << "Error : only one root by server" << std::endl;
		return;
	}
	
	else
		tmpServer.setRoot(path);
}

void Parsing::handleBodySize(const std::string& value, Server& tmpServer) {

	if (tmpServer.getMaxBodySize()) {
		wrongConfig = true;
		std::cout << "Error : you already set the client max body size" << std::endl;
		return;
	}

	std::istringstream iss(value);
    	int bodySize;
	iss >> bodySize;

	if (isValidSize(bodySize))
		tmpServer.setMaxBodySize(bodySize);
	else {
		std::cout << "Error : client_max_body_size is out of range '" << bodySize << "'" << std::endl;
		wrongConfig = true;
	}
}

void Parsing::handleMethods(const std::string& value, Location& tmpLoc) {

	void (Location::*methodsSetters[])(void) = { &Location::setGet, &Location::setPost, &Location::setDelete};
	std::string methods[] = {"GET;", "POST;", "DELETE;"};	
	int method = 0;	
	while (method < 3) {
		if (value == methods[method])		
			break;
		method++;
	}

	switch (method) {
		case 0:
			(tmpLoc.*methodsSetters[0])();
			break;
		case 1:
			(tmpLoc.*methodsSetters[1])();
			break;
		case 2:
			(tmpLoc.*methodsSetters[2])();
			break;
		default:
			(void)tmpLoc;
			std::cout << "Error : the method :'" << value << "' does not exist" << std::endl;
	}
}
	


/*void Parsing::handleScript(const std::string& word, const std::string& value) {
	
	if (tmpServer.getScript()) {
		wrongConfig = true;
		std::cout << "Error : only one root by server" << std::endl;
		return;
	}
}


void Parsing::handleCgi(const std::string& word, const std::string& value) {
}

void Parsing::handleUpload(const std::string& word, const std::string& value) {
}
*/
void Parsing::handleErrorPages(const std::string& value, Server &tmpServer) {

	std::string path = value.substr(0, value.length() - 1);
	std::string::size_type dotPos = path.rfind(".html");
	if (dotPos == std::string::npos || dotPos < 3) {
		std::cout << "Error : wrong file format '" << value << "'" << std::endl;
		wrongConfig = true;
		return;
	}
	std::string extractedCode = path.substr(dotPos - 3, 3);
	std::istringstream iss(extractedCode);
	int errorCode;
	iss >> errorCode;
	
	if (tmpServer._error.find(errorCode) == tmpServer._error.end()) {
		tmpServer.addError(errorCode, path);
	} else {
		wrongConfig = true;
    		std::cout << "Error : this error page is already set up : '" << errorCode << "'" << std::endl;
	}
}

bool Parsing::isValidPort(int port) {
	return port >= 1 && port <= 65535;
}

bool Parsing::isValidSize(int maxBodySize) {
	return maxBodySize >= 1 && maxBodySize <= 100;
}

bool Parsing::isValidPath(const char* dir) {

	struct stat sb;

	if (stat(dir, &sb) == 0)
		return true;
	return false;
}

bool Parsing::isValidFile(const char *file) {

	struct stat sb;

	if (stat(file, &sb) == 0 && !(sb.st_mode & S_IFDIR)) {
		return true;
	}
	return false;
}

bool Parsing::isAnExecutable(const char *exec) {

	struct stat sb;

	if (stat(exec, &sb) == 0 && !(sb.st_mode & S_IFDIR))
		
		if (sb.st_mode & S_IXUSR || sb.st_mode & S_IXGRP || sb.st_mode & S_IXOTH)
			return true;
	return false;
}

bool Parsing::isValidExtension(const std::string& filename, const std::string& extensionToCompare) {

	if (extensionToCompare == ".html") {
		std::string extensionFromFile = filename.substr(filename.length() - 5);
		return (extensionFromFile == ".html");
	}
	else if (extensionToCompare == ".py") {
		std::string extensionFromFile = filename.substr(filename.length() - 3);
		return (extensionFromFile == ".py");
	}
	else if (extensionToCompare == ".php") {
		std::string extensionFromFile = filename.substr(filename.length() - 4);
		return (extensionFromFile == ".php");
	}
	return false;
}

bool Parsing::methodCheckAndLaunchSet(const std::string& method, Location& tmpLoc) {
 (void)tmpLoc;
	if (method == "GET") 	
		return true;
	else if (method == "POST")
		return true;
	else if (method == "DELETE")
		return true;
	return false;
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
	
	if (value.empty()) {

		std::cout << "Error : no value given after " << keyword << "'" << std::endl;
		wrongConfig = true;
		return false;
	}
	
	if (value[value.size() - 1] != ';') {
		std::cout << "Error : each value must terminate by a -> ';'" << std::endl;
		wrongConfig = true;
		return false;
	}
	
	if (iss >> intruder) {

		std::cout << "Error : intruder word found after keyword and value : '" << intruder << "'" << std::endl;
		wrongConfig = true;
		return false;
	}
	wrongConfig = false;
	return true;
}

void Parsing::locBlockHandler(const std::string& line, Server& tmpServer, Location& tmpLoc) {

	std::istringstream iss(line);
	std::string keyword;
	std::string value;
	std::string brace;

	iss >> keyword >> value >> brace;
(void)tmpServer;
(void)tmpLoc;
	if (brace == "{" && inServer) {
		inLocation = true;
		/*if (!wrongConfig) 
			tmpServer.addLocation(value, tmpLoc);
		else {
			wrongConfig = true;
			std::cout << "Error : wrong Location syntax" << std::endl;
		}*/
	}
}


std::string Parsing::trimLeft(const std::string& str) {
    size_t start = 0;
    while (start < str.length() && std::isspace(str[start])) {
        ++start;
    }
    return str.substr(start);
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
	iss >> keyword >> value;

	if (keyword == "server") {
		if (value == "{") {
			inServer = true;
			inLocation = false;
			wrongConfig = false;
			tmpServer = Server();
			continue;
		}
		else {
			wrongConfig = true;
		}
	}
	
	if (keyword == "location") {
		locBlockHandler(trimmedLine, tmpServer, tmpLoc);
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
				static int serverCount = 0;
				std::ostringstream oss;
				oss << "server" << serverCount++;
				std::string serverName = oss.str();
				config.addServer(serverName, tmpServer);
				std::cout << tmpServer;
			}
	    		else
		    		std::cout << "Error : server didn't create because the configuration is not valid" << std::endl;
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
			keywordLocHandling handlingMethod = it->second;
			(this->*handlingMethod)(value, tmpLoc); 
		} else {
			std::cout << "Error : unknow keyword : '" << keyword << "'" << std::endl;
			wrongConfig = true;
		}
	}
	else {
		std::map<std::string, keywordSrvHandling>::iterator it = keywordSrvHandler.find(keyword);
		if (it != keywordSrvHandler.end()) {
			keywordSrvHandling handlingMethod = it->second;
			(this->*handlingMethod)(value, tmpServer); 
		} else {
			std::cout << "Error : unknow keyword : '" << keyword << "'" << std::endl;
			wrongConfig = true;
		}
	}
}
}
