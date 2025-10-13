#include "../include/Server.hpp"
#include <errno.h>
#include <stdexcept>

Server::Server(int port, const std::string& password): _server_fd(-1), _port(port), _password(password) {
    setupSocket();
}

Server::~Server() {
    for (size_t i = 0; i < _clients.size(); i++)
        delete _clients[i];
    if (_server_fd != -1)
        close(_server_fd);
}

void Server::setupSocket() {
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
        throw std::runtime_error("Error creating socket");

    int opt = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(_server_fd);
        throw std::runtime_error("Failed to set socket options");
    }

    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(_port); 

    if (bind(_server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(_server_fd);
        throw std::runtime_error("Error binding socket");
    }

    if (listen(_server_fd, 5) < 0) {
        close(_server_fd);
        throw std::runtime_error("Error listening on socket");
    }
    
    std::cout << "Server listening on port " << _port << std::endl;
}

void Server::start() {
    struct pollfd server_poll;
    server_poll.fd = _server_fd;
    server_poll.events = POLLIN;
    server_poll.revents = 0;
    _poll_fds.push_back(server_poll);
    
    std::cout << "Server started successfully!" << std::endl;

    while (true) {
        int poll_count = poll(&_poll_fds[0], _poll_fds.size(), -1);
        if (poll_count < 0)
            throw std::runtime_error("Poll error");
        // Check server socket for new connections
        if (_poll_fds[0].revents & POLLIN)
            acceptNewClient();

        // Check client sockets for data  
        for (size_t i = 1; i < _poll_fds.size(); i++) {
            if (_poll_fds[i].revents & POLLIN)
                handleClientData(_poll_fds[i].fd);
            if (_poll_fds[i].revents & POLLHUP){
                removeClient(_poll_fds[i].fd);
                i--;
            }
        }
    }
}

void Server::acceptNewClient() {
    int client_fd = accept(_server_fd, NULL, NULL);
    if (client_fd < 0) {
        std::cerr << "Error accepting connection" << std::endl;
        return;
    }
    // Set non-blocking
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    Client* new_client = new Client(client_fd);
    _clients.push_back(new_client);
    // Add to poll
    struct pollfd client_poll;
    client_poll.fd = client_fd;
    client_poll.events = POLLIN;
    client_poll.revents = 0;
    _poll_fds.push_back(client_poll);
    
    std::cout << "New client connected: " << client_fd << std::endl;
    
    // send welcome message
    std::string welcome_msg = "Welcome to IRC Server!\r\n";
    send(client_fd, welcome_msg.c_str(), welcome_msg.length(), 0);
}

void Server::handleClientData(int client_fd) {
    char buffer[1024];
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received <= 0) {
        removeClient(client_fd);
        return ;
    }
    
    buffer[bytes_received] = '\0';
    std::string message(buffer);
    
    std::cout << "Received from " << client_fd << ": " << message << std::endl;
    
    // basic response
    if (message == "hello\r\n") {
        std::string response = "dont say hello, say wassup!\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
    }
}

void Server::removeClient(int client_fd) {
    std::cout << "Removing client: " << client_fd << std::endl;
    
    // Remove from clients vector
    for (size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i]->getFd() == client_fd) {
            delete _clients[i];
            _clients.erase(_clients.begin() + i);
            break;
        }
    }
    
    // Remove from poll_fds vector
    for (size_t i = 0; i < _poll_fds.size(); i++) {
        if (_poll_fds[i].fd == client_fd) {
            _poll_fds.erase(_poll_fds.begin() + i);
            break;
        }
    }
    
    close(client_fd);
}

const std::string& Server::getPassword() const {
    return _password;
}