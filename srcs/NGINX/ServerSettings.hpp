/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSettings.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohanafi <yohanafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 16:55:01 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/06/13 08:25:04 by iait-ouf         ###   ########.fr       */
/*   Updated: 2025/05/05 16:51:36 by yohanafi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../../inc/Libraries.h"
#include "Location.hpp"

class ServerSettings {

    private :
	    std::string						_serverName;		// name of server ex : localhost
		std::string						_root;				// our main repository
		int								_port;				// port number ex: 8080
		int								_maxBodySize;		// max body size (mo) ex: 10
    	std::map<std::string, Location>	_loc;    			// map of routes ex : [/images/][instanciate obj Location]
		std::map<int, std::string>		_error;				// map of http erros ex : [404][path of error 404 html]
		in_addr_t						_ip;

		// Setters
		void setName(const std::string& serverName);
		void setRoot(const std::string& root);
		void setPort(int port);
		void setMaxBodySize(int maxBodySize);
		void addError(int code, const std::string& path);
		void addLocation(const std::string& path, Location& loc);
		void setClientDir(const std::string& clientBodyTmpPath);
		void setIpAddr(int byte[4]);
		
		friend class Parsing;
			
	public :	
		// Constructors and destructor
		ServerSettings();
		ServerSettings(std::string _serverName);
        	ServerSettings(std::string _name, std::string _root, int _port, int _maxBodySize, in_addr_t _ip);
        	~ServerSettings();

		// Getters
		const				std::string getName()			const;
		const				std::string getRoot()			const;
		int				getPort() 				const;
		int				getMaxBodySize()			const;		
		std::map<std::string, Location> getLocation()				const;
		Location			findAndGetLocation(std::string uriLoc)	const;
		in_addr_t			getIpAddr() 				const; 
		const std::map<int, std::string> &getErrors() 				const;

		// Overload operator to print all the attributes of the obj in a row
        	friend std::ostream& operator<<(std::ostream& out, const ServerSettings& server);

};
