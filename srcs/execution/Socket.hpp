
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iait-ouf <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 16:32:38 by iait-ouf          #+#    #+#             */
/*   Updated: 2025/05/05 16:55:57 by iait-ouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
#include "../../inc/Libraries.h"

class Socket
{
    private:
        int _fd;
        struct sockaddr_in _addr;

    public:
        Socket();
        Socket(in_addr_t ip, int port);
        Socket(int serverfd);
        ~Socket(void);

        int getfd();
        sockaddr_in getAddr();

        class SocketException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
        class BindException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
        class ListenException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };

        class AcceptException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
};
