#include "Socket.hpp"

/**
 *  @brief canonical form, never used
 */
Socket::Socket(){}
//can we not do that ??

/**
 *  @brief create a new socket, bind it, and make it listen
 *  @param ip the ip address to listen
 *  @param port the port to listen
 */
Socket::Socket(in_addr_t ip, int port){
    int status;

    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = htonl(ip);
    _addr.sin_port = htons(port);

    _fd = socket(_addr.sin_family, SOCK_STREAM, 0);
    if (_fd == -1)
        throw SocketException();
    
    status = bind(_fd, (struct sockaddr *)& _addr, sizeof(_addr));
    if (status == -1)
        throw BindException();
    
    status = listen(_fd, 10);//arbitrary: 10 = max number of pending connections in queue
    if (status == -1)
        throw ListenException();

    std::cout << "server use a socket to listen port " << port << " with fd [" << _fd << "]" << std::endl;
}

/**
 *  @brief accept a new socket linked to a listening socket
 *  @param serverfd the file descriptor of the listening socket
 */
Socket::Socket(int serverfd){
    memset(&this->_addr, 0, sizeof(this->_addr));
    socklen_t addrLen = sizeof(_addr);
    // _addr.sin_family = AF_INET;//??
	// _addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);//??
    _fd = accept(serverfd, (struct sockaddr *)& _addr, &addrLen);
    // (void)_serverfd;
    // _fd = -1;
    if (_fd == -1){
        throw AcceptException();
    }
}

Socket::~Socket(){
    // if (_fd > 0)
    //     close(_fd);
    // std::cout << _fd << std::endl;
}

int Socket::getfd(){
    return _fd;
}

sockaddr_in Socket::getAddr(){
    return _addr;
}

const char * Socket::SocketException::what() const throw(){
    return "Error of function socket in constructor of class Socket";
}

const char * Socket::BindException::what() const throw(){
    return "Error of function bind in constructor of class Socket";
}

const char * Socket::ListenException::what() const throw(){
    return "Error of function listen in constructor of class Socket";
}

const char* Socket::AcceptException::what() const throw(){
    return "Error of function accept in constructor of class Client";
}
