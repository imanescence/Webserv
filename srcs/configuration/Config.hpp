#pragma once
#include "Server.hpp"

class Config {
	
	private :

		std::string type;			// from instructions or by default ?
		
		std::map<std::string, Server> servers;	// map of servers ex: [localhost][obj Server]

		
		public :
		
		// constructor by default
		Config();
		
		// constructor to set up the server by default
		Config(std::string _default);
		
		// destructor
		~Config();
		
		// to know if the server is present in the config
		bool isServer(const std::string word);
		
		std::map<std::string, Server>& getServers();
		
		// overload operator to print all the attributes
		friend std::ostream& operator<<(std::ostream& out, const Config& config);
		
		void addServer(const std::string& name, const Server& server);
};
