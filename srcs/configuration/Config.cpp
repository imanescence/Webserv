/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iait-ouf <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:03:46 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/05/14 21:37:30 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config() {}

Config::Config(std::string _default) : type(_default) {
	servers.insert(std::pair<std::string, Server>("Default", Server("Default", "./web_repo", 8080, 10)));
}

std::ostream& operator<<(std::ostream& out, const Config& config) {
    out << "CONFIG TYPE -> " << config.type << "\n";
    
    out << "\n------- SERVERS... -------\n";
    for (std::map<std::string, Server>::const_iterator it = config.servers.begin(); it != config.servers.end(); ++it) {
        out << "Server -> " << it->first << "\n";
        out << it->second; 
    }

    return out;
}

std::map<std::string, Server>&Config:: getServers() { 

	return servers; 
}

void Config::addServer(const std::string& name, const Server& server) {
	servers.insert(std::make_pair(name, server));
}

Config::~Config() {}
