#include "Socket.hpp"

/**
 *  @brief lazy initialization
 */
Socket::Socket(){
    _fd = -1;
    memset(&_addr, 0, sizeof(_addr));
    _buffer = "";
}

Socket::~Socket(){}

/**
 *  @brief create a new socket, bind it, and make it listen
 *  @param ip the ip address to listen
 *  @param port the port to listen
 */
int Socket::isListeningIPV4(in_addr_t ip, int port){
    int status;

    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = htonl(ip);
    _addr.sin_port = htons(port);

    _fd = socket(_addr.sin_family, SOCK_STREAM, 0);
    if (_fd == -1)
        return ERROR_SOCKET;

    int reuse = 1;
    status = setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (status == -1){
        close(_fd);
        return ERROR_SETSOCKOPT;
    }

    status = bind(_fd, (struct sockaddr *)& _addr, sizeof(_addr));
    if (status == -1){
        close(_fd);
        return ERROR_BIND;
    }

    status = listen(_fd, 10);
    if (status == -1){
        close(_fd);
        return ERROR_LISTEN;
    }

    return SUCCESS;
}

/**
 *  @brief create a new socket for the client and link it to the listening socket of the server
 *  @param listeningSocket the listening socket of the server
 */
int Socket::acceptMaster(Socket & listeningSocket){
    int status;

    _addr.sin_family = listeningSocket.getAddr().sin_family;
	_addr.sin_addr.s_addr = listeningSocket.getAddr().sin_addr.s_addr;
    _addr.sin_port = listeningSocket.getAddr().sin_port;
    socklen_t addrLen = sizeof(_addr);

    _fd = accept(listeningSocket.getfd(), (struct sockaddr *)& _addr, &addrLen);
    if (_fd == -1)
        return ERROR_ACCEPT;

    int flag = 1;
    status = setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &flag, sizeof(flag));
    if (status == -1)
        return ERROR_SETSOCKOPT;
    
    return SUCCESS;
}

int Socket::fillBuffer(int readLen) {
    if (_fd < 0)
        return CLIENT_LEFT;
    char tmp[readLen];
    ssize_t bytes = recv(_fd, tmp, sizeof(tmp), MSG_DONTWAIT);
    if (bytes < 0) {
        std::cerr << "[ERROR] recv() failed" << std::endl;
        return RECV_ERROR;
    } else if (bytes == 0) {
        std::cout << "[INFO] connection closed by peer" << std::endl;
        return CLIENT_LEFT;
    }
    _buffer.append(tmp, bytes);
    return SUCCESS;
}

/**
 *  @brief find the position of a given delimiter
 *  @param delim the delimiter
 *  @param maxPos the maximum value admited for the position
 *  @return return the position of the delimiter
 * 
 * -1 if it is found after maxPos
 * 
 * 0 if not found
 */
size_t Socket::findDelimiter(const std::string & delim, size_t maxPos){
    size_t pos;
    pos = _buffer.find(delim);
    if (pos == std::string::npos && _buffer.size() < maxPos)
        return 0;
    if (pos > maxPos || _buffer.size() > maxPos)
        return -1;
    return pos;
}

/**
 *  @brief shift a string of interest from buffer to result
 *  @param pos the position of the end of the string of interest
 *  @param lenDelim the length of the delimiter after the string of interest
 *  @param result the string in which the content of the buffer will be put
 */
void Socket::consumeBuffer(size_t pos, size_t lenDelim, std::string & result){
    result = _buffer.substr(0, pos);
    _buffer = _buffer.substr(pos + lenDelim);
}

int Socket::sendMsg () {
    if (_fd < 0) {
        std::cerr << "[HTTP] Invalid fd for response"<< std::endl;
        return FILE_CLOSED;
    }

    int error = 0;
    socklen_t len = sizeof(error);
    int result = getsockopt(_fd, SOL_SOCKET, SO_ERROR, &error, &len);

    if (result != 0 || error != 0) {
        std::cerr << "[HTTP] Socket error before sending response: " << strerror(error) << std::endl;
        std::cerr << "result = " << result << ", error = " << error << std::endl;
        return CLIENT_LEFT;
    }
    ssize_t bytesSent = send(_fd, _buffer.c_str(), _buffer.length(), MSG_NOSIGNAL | MSG_DONTWAIT);
    if (bytesSent == -1) {
	    std::cerr << "Failed to send response" << std::endl;
        return SEND_ERROR;
    }
    if (bytesSent != (ssize_t)_buffer.length()) {
        std::cerr << "[HTTP] Partial response sent: " << bytesSent << "/" << _buffer.length() << " bytes" << std::endl;
        _buffer = _buffer.substr(bytesSent);
        return PARTIAL_SEND;
    }
    return SUCCESS;
}

int Socket::getfd() const{
    return _fd;
}

sockaddr_in Socket::getAddr() const{
    return _addr;
}

const std::string & Socket::getBuffer(void) const{
    return _buffer;
}

size_t Socket::getSizeBuffer() {
    return _buffer.size();
}

void Socket::setBuffer(std::string toBuffer) {
    _buffer = toBuffer;
}

std::ostream & operator<<(std::ostream & COUT,  const Socket & socket){
    
    COUT << "[SOCKET]" << std::endl;
    COUT << "    fd:              " << socket.getfd() << std::endl;
    COUT << "    addr.sin_family: " << socket.getAddr().sin_family << std::endl;
    COUT << "    addr.sin_port:   " << ntohs(socket.getAddr().sin_port) << std::endl;
    COUT << "    addr.sin_addr:   " << ntohl(socket.getAddr().sin_addr.s_addr) << std::endl;
    
    COUT << "    (contain buffer: " << socket.getBuffer() << ")" << std::endl;

    return COUT;
}
