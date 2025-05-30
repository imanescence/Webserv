/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iait-ouf <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 12:37:07 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/05/16 13:55:03 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Libraries.h"
#include "configuration/Config.hpp"
#include "configuration/Server.hpp"
#include "execution/ServerExec.hpp"


class ServerHandler {

	private :
		Config& config;
		std::vector<ServerExec> _servers;

	public :
		ServerHandler(Config* conf);
		~ServerHandler();
		
		void serverStarter();
		void cleanup();
};
