/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iait-ouf <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:03:46 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/06/12 15:12:18 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config() {}

Config::Config(std::string _default) : type(_default) {
	servers.insert(std::pair<int, ServerSettings>(8080, ServerSettings("Default", "www/", 8080, 10, 0x7f000001)));
}

std::ostream& operator<<(std::ostream& out, const Config& config) {
    out << "CONFIG TYPE -> " << config.type << "\n";
    
    out << "\n------- SERVERS... -------\n";
    for (std::map<int, ServerSettings>::const_iterator it = config.servers.begin(); it != config.servers.end(); ++it) {
        out << "Server with the port-> " << it->first << "\n";
        out << it->second; 
    }

    return out;
}

std::map<int, ServerSettings>&Config:: getServers() { 

	return servers; 
}

void Config::addServer(const int port, const ServerSettings& server) {
	servers.insert(std::make_pair(port, server));
}

Config::~Config() {}
