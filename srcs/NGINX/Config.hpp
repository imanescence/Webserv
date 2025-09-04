#pragma once
#include "ServerSettings.hpp"

class Config {
	
	private :

		std::string type;
		
		std::map<int, ServerSettings> servers;	// map of servers ex: [localhost][obj Server]

		
		public :
		
		// constructor by default
		Config();
		
		// constructor to set up the server by default
		Config(std::string _default);
		
		// destructor
		~Config();
		
		// to know if the server is present in the config
		bool isServer(const std::string word);
		
		std::map<int, ServerSettings>& getServers();
		
		// overload operator to print all the attributes
		friend std::ostream& operator<<(std::ostream& out, const Config& config);
		
		void addServer(const int port, const ServerSettings& server);
};
