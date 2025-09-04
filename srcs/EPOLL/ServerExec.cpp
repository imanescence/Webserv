#include "ServerExec.hpp"

ServerExec::ServerExec(ServerSettings & config, int epfd) : _config(config){
    int status;

    status = _socket.isListeningIPV4(_config.getIpAddr(), _config.getPort());
    if (status != SUCCESS){
        std::cerr << "[SERVER] error socket = " << status << std::endl;
        throw SocketException();
    }

    if (!setup_epoll(epfd)){
        std::cerr << "[SERVER] error setup epoll" << std::endl;
        throw EpollException();
    }
}

ServerExec::~ServerExec(){
    _clients.clear();
}

/**
 *  @brief add an event to epoll for the listening ServerExec
 *  @param epfd the file descriptor of epoll
 *  @return true on successful setup, false otherwise
 */
bool ServerExec::setup_epoll(int epfd){
    struct epoll_event sockev;

    sockev.events = EPOLLIN;
    sockev.data.fd = _socket.getfd();
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, _socket.getfd(), &sockev) == -1){
        std::cerr << "[SERVER] epoll_ctl error | fd: " << sockev.data.fd << std::endl;
        return false;
    }
        
    return true;
}

/**
 *  @brief if the file discriptor of the event is known by this server, handle it
 *  @param epfd the file descriptor of epoll
 *  @param event the event to handle
 *  @return the status of the resolution of the event
 */
void ServerExec::handleEvent(int epfd, struct epoll_event & event){
    int eventfd = event.data.fd;

    if (eventfd == _socket.getfd()){
        accept_new_client(epfd);
        return ;
    }

    std::map<const int, Client>::iterator knownClient = _clients.find(eventfd);
    if (knownClient == _clients.end())
        return ;

    int status = knownClient->second.processing();
    if (status == REMOVE_CLIENT || status == SUCCESS){
        epoll_ctl(epfd, EPOLL_CTL_DEL, eventfd, &event);
        _clients.erase(eventfd);
        close(eventfd);
    }
    else if (status == OK){
        event.events = EPOLLOUT;
        status = epoll_ctl(epfd, EPOLL_CTL_MOD, eventfd, &event);
    }
}

/**
 *  @brief create a new client and update epoll list
 *  @param epfd file descriptor of epoll
 *  @return On success, the file descriptor of the new client
 * 
 *  On error, -1
 */
int ServerExec::accept_new_client(int epfd){
    struct epoll_event clientev;
    
    Client client(_socket, _config);
    clientev.data.fd = client.getfd();
    clientev.events = EPOLLIN;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, client.getfd(), &clientev) == -1){
        std::cerr << "[SERVER] cannot add new connection" << std::endl;
        epoll_ctl(epfd, EPOLL_CTL_DEL, clientev.data.fd, &clientev);
        return -1;
    }
    _clients.insert(std::pair<int, Client>(client.getfd(), client));

    return clientev.data.fd;
}

void ServerExec::cleanup() {
    std::cout << "[INFO] Cleaning up ServerExec named" << _config.getName() << " ...\n";

    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        close(it->first);
    }
    _clients.clear();
}

const ServerSettings & ServerExec::getConfig() const{
    return _config;
}

const char* ServerExec::SocketException::what() const throw(){
    return "No socket in constructor of class ServerExec";
}

const char* ServerExec::EpollException::what() const throw(){
    return "Epoll did not setup in constructor of class ServerExec";
}
