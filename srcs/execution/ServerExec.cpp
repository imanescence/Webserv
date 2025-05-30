#include "ServerExec.hpp"

ServerExec::ServerExec(Server & config) : _config(config){
    // _ip = 0x7f000001;
    // _addr.sin_family = AF_INET;
	// _addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	// _addr.sin_port = htons(_config.getPort());
    std::map<std::string, Location> locations = _config.getLocation();
    std::cout << "Number of locations: " << locations.size() << std::endl;
    try{
        _socket = new Socket(INADDR_LOOPBACK, _config.getPort());
    }
    catch (std::exception &e){
        std::cerr << e.what() << std::endl;
        throw SocketException();
        // throw e;//sans doute mieux, a voir plus tard, segv first
    }
    
    if (setup_epoll() == -1){
        std::cerr << "error setup epoll" << std::endl;
        throw EpollException();
    }
}

ServerExec::~ServerExec(){
    for (std::map<const int, Client*>::iterator it = _clients.begin(); it != _clients.end() ; it++){
        delete it->second;
    }
    _clients.clear();

    // delete _socket; //double free ici, mais ou est le free de base ?
}

/**
 *  @brief create an epoll and add an event for the listening ServerExec
 *  @param _epfd a pointer to the file descriptor of epoll
 *  @return On success, 0
 * 
 *  On error, -1
 */
int ServerExec::setup_epoll(){
    int status;
    struct epoll_event sockev;

    _epfd = epoll_create(1); //size is ignored, but must be greater than 0
    if (_epfd == -1)
        return -1;
    std::cout << "fd [" << _epfd << "] used for epoll" << std::endl;

    sockev.events = EPOLLIN;
    sockev.data.fd = _socket->getfd();
    status = epoll_ctl(_epfd, EPOLL_CTL_ADD, _socket->getfd(), &sockev);
    if (status == -1){
        //faut-il EPOLL_CTL_DEL les precedents ajoutes ? probablement, oui, mais tellement la flemme
        std::cerr << "epoll_ctl error | fd: " << sockev.data.fd << std::endl;
        close(_epfd);
        return -1;
    }
    std::cout << "listening socked fd [" << sockev.data.fd << "] added to epoll events" << std::endl;
        
    return status;
}

/**
 *  @brief the ServerExec wait for new clients and communicate with them
 *  @return On success, 0
 * 
 *  On error, -1
 */
int ServerExec::client_processing(){
    int status;
    int clientfd;
    int maxevents = 10;//10 au pif
    struct epoll_event events[maxevents];
    int timeout = 2000;

    int nevready = epoll_wait(_epfd, events, maxevents, timeout);
    if (nevready < 0){
        std::cerr << "error epoll_wait" << std::endl;
        return -1;
    }
    else if (nevready == 0){
        //not needed
        std::cout << _config.getName() <<" listening to port " << _config.getPort() << " is waiting..." << std::endl;
    }
    else{
        for (int i = 0; i < nevready; i++){
            if (events[i].data.fd == _socket->getfd()){
                std::cout << "new client" << std::endl;
                clientfd = accept_new_client(_epfd, events[i].data.fd);
                if (clientfd == -1){
                    std::cerr << "error accept_new_client" << std::endl;
                    //manage too much connections
                }
                else
                    std::cout << "new client fd " << clientfd << std::endl;
            }
            else{
                std::cout << "fd [" << events[i].data.fd << "] send msg to ServerExec" << std::endl;
                // status = client_communication(_epfd, events[i]);
                status = _clients.find(events[i].data.fd)->second->request();
                if (status == CLIENT_LEFT){
                    _clients.erase(events[i].data.fd);
                    epoll_ctl(_epfd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
                    close(events[i].data.fd);
                    // manage error communication
                }
                else if (status == TIMEOUT){
                    std::cout << "request time out" << std::endl;
                }
            }
        }
    }
    return 0;
}

/**
 *  @brief the ServerExec detect a connection, create a new client, and update epoll list
 *  @param _epfd file descriptor of epoll
 *  @return On success, the file descriptor of the new client
 * 
 *  On error, -1
 */
int ServerExec::accept_new_client(int _epfd, int sockfd){
    struct epoll_event clientev;
    
    Client *client = new Client(sockfd, _config);
    //catch error client constructor ?
    clientev.data.fd = client->getfd();
    clientev.events = EPOLLIN;
    if (epoll_ctl(_epfd, EPOLL_CTL_ADD, client->getfd(), &clientev) == -1){
        std::cerr << "cannot add new connection" << std::endl;
        epoll_ctl(_epfd, EPOLL_CTL_DEL, clientev.data.fd, &clientev);//?
        delete client;
        return -1;
    }
    std::cout << "client " << client->getfd() <<" added to epoll" << std::endl;
    _clients.insert(std::pair<int, Client*>(client->getfd(), client));

    return clientev.data.fd;
}

void ServerExec::cleanup() {
    std::cout << "[INFO] Cleaning up ServerExec named" << _config.getName() << " ...\n";

    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        close(it->first);//je crois que ca peut etre gere par la socket, dans client
        delete it->second;
    }
    _clients.clear();

    close(_epfd);//en fait, on s'en fout, mais par principe
}

const Server & ServerExec::getConfig() const{
    return _config;
}

const char* ServerExec::SocketException::what() const throw(){
    return "No socket in constructor of class ServerExec";
}

const char* ServerExec::EpollException::what() const throw(){
    return "Epoll did not setup in constructor of class ServerExec";
}
