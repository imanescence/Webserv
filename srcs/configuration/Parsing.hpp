/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iait-ouf <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 11:26:15 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/05/20 12:36:12 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once 
#include "../../inc/Libraries.h"
#include "Location.hpp"
#include "Config.hpp"
#include "Server.hpp"

class Config;

class Parsing {
	private :
		Server tmpServer;
		Location tmpLoc;

	public :
		Config config;
		Parsing();
		~Parsing();

		// Verification methods
		bool isValidPort(int port);
		bool isValidSize(int maxBodySize);
		bool isValidPath(const char *path);
		bool isValidFile(const char *file);
		bool isAnExecutable(const char *exec);
		bool isValidExtension(const std::string& filename, const std::string& extension);
		bool methodCheckAndLaunchSet(const std::string& method, Location& tmpLoc);
		bool isValidKeyword(const std::string& word); 
		
		// Pure Parsing methods
		void parseLine(const std::string& line);
		bool lineToSkip(const std::string& line);
		bool isValidSyntax(const std::string& line);
		void parseFile(const std::string& fileName, Config& config);
		std::string trimLeft(const std::string& str);
		void locBlockHandler(const std::string& line, Server& tmpServer, Location& tmpLoc);
		std::string removeSemicolon(const std::string& src);

		// Keywords methods
		void handlePort(const std::string& value, Server& tmpServer);
		void handleServerName(const std::string& value, Server& tmpServer);
		void handleRoot(const std::string& value, Server& tmpServer);
		void handleLocation(std::string& value, Location& tmpLoc);
		void handleBodySize(const std::string& value, Server& tmpServer);
		//void handleScript(const std::string& value, Location& tmpLoc);
		void handleMethods(const std::string& value, Location& tmpLoc);
		void handleRoot(const std::string& value, Location& tmpLoc);
		//void handleCgi(const std::string& value, const std::string& value);
		//void handleUpload(const std::string& value, Location& tmpLoc);
		void handleErrorPages(const std::string& value, Server& tmpServer);

		bool inLocation;
		bool inServer;	
		bool wrongConfig;
		typedef void(Parsing::*keywordSrvHandling)(const std::string&, Server&);
		std::map<std::string, keywordSrvHandling> keywordSrvHandler;
		typedef void(Parsing::*keywordLocHandling)(const std::string&, Location&);
		std::map<std::string, keywordLocHandling> keywordLocHandler;
};
