/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iait-ouf <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 11:26:15 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/06/17 15:24:37 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once 
#include "Location.hpp"
#include "Config.hpp"
#include "ServerSettings.hpp"

class Config;

class Parsing {
	private :
		ServerSettings tmpServer;
		Location tmpLoc;

	public :
		Config config;
		Parsing();
		~Parsing();

		// Verification methods
		bool isValidPort(int port);
		bool isValidSize(int maxBodySize);
		bool isValidKeyword(const std::string& word); 
		bool isServerLaunchable(ServerSettings& tmpServer);

		// Pure Parsing methods
		void parseLine(const std::string& line);
		bool lineToSkip(const std::string& line);
		bool isValidSyntax(const std::string& line);
		void parseFile(const std::string& fileName, Config& config);
		std::string trimLeft(const std::string& str);
		void isInsideALocBlock(const std::string& line, ServerSettings& tmpServer, Location& tmpLoc);
		std::string removeSemicolon(const std::string& src);
		bool isValidErrorCode(int code);
		bool isValidRedirect(int code);

		// Keywords methods
		void handlePort(const std::string& value, ServerSettings& tmpServer);
		void handleServerName(const std::string& value, ServerSettings& tmpServer);
		void handleHost(const std::string& value, ServerSettings& tmpServer);
		void handleRoot(const std::string& value, ServerSettings& tmpServer);
		void handleLocation(std::string& value, Location& tmpLoc);
		void handleBodySize(const std::string& value, ServerSettings& tmpServer);
		void handleMethods(const std::string& value, Location& tmpLoc);
		void handleRoot(const std::string& value, Location& tmpLoc);
		void handleErrorPages(const std::string& value, ServerSettings& tmpServer);
		void handleClientTmpBodyPath(const std::string& value, Location& tmpLoc);
		void handleAutoIndex(const std::string& value, Location& tmpLoc);
		void handleRedirect(const std::string& value, Location& tmpLoc);

		size_t lineNumber;
		bool inLocation;
		bool inServer;	
		bool wrongConfig;
		typedef void(Parsing::*keywordSrvHandling)(const std::string&, ServerSettings&);
		std::map<std::string, keywordSrvHandling> keywordSrvHandler;
		typedef void(Parsing::*keywordLocHandling)(const std::string&, Location&);
		std::map<std::string, keywordLocHandling> keywordLocHandler;

		enum Methods {
			GET,
			POST,
			DELETE
		};
};
