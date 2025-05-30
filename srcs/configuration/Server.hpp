/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohanafi <yohanafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 16:55:01 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/05/19 17:19:46 by iait-ouf         ###   ########.fr       */
/*   Updated: 2025/05/05 16:51:36 by yohanafi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../../inc/Libraries.h"
#include "../configuration/Location.hpp"
// #include "Client.hpp"
// #include "Socket.hpp"
#include "Location.hpp"

class Server {

    private :
	    std::string _serverName;			// name of server ex : localhost
		std::string _root;			// our main repository
		int _port;				// port number ex: 8080
		int _maxBodySize;			// max body size (mo) ex: 10
    	std::map<std::string, Location> _loc;    // map of routes ex : [/images/][instanciate obj Location]
		std::map<int, std::string> _error;	// map of http erros ex : [404][/404.html]

		// Setters
		void setName(const std::string& _serverName);
		void setRoot(const std::string& _root);
		void setPort(int _port);
		void setMaxBodySize(int _maxBodySize);
		void addError(int code, const std::string& path);
		void addLocation(const std::string& path, Location& loc);
		
		friend class Parsing;
		// ou juste mettre une methode ex : friend void Parsing::parse(Data&, const std::string&);
			
	public :
			
		// Constructors and destructor
        Server();
		Server(std::string _serverName);
        Server(std::string _name, std::string _root, int _port, int _maxBodySize);
        ~Server();

		// Getters
		const std::string getName() const;
		const std::string getRoot() const;
		int getPort() const;
		int getMaxBodySize() const;
		std::map<std::string, Location> getLocation() const;

		// Overload operator to print all the attributes of the obj in a row
        friend std::ostream& operator<<(std::ostream& out, const Server& server);

		//void addLocation // faire une methode qui permet d'add une key et une value dans le container
        //+ add errorpage                // de map Locations et qui instancie en meme temps un nouvel obj Location
};
