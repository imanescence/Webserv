
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
        int                 _fd;
        struct sockaddr_in  _addr;
        std::string         _buffer;

    public:
        Socket();
        ~Socket(void);

        int isListeningIPV4(in_addr_t ip, int port);
        int acceptMaster(Socket & serverSocket);

        int         fillBuffer(int readLen);
        size_t      findDelimiter(const std::string & delim, size_t maxPos);
        void        consumeBuffer(size_t pos, size_t lenDelim, std::string & result);
        int         sendMsg();
        
        //accessors
        int                 getfd() const;
        sockaddr_in         getAddr() const;
        const std::string & getBuffer(void) const;
        size_t              getSizeBuffer();
        void                setBuffer(std::string toBuffer);
};

std::ostream & operator<<(std::ostream & COUT,  const Socket & socket);
