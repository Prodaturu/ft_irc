#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <poll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include "Client.hpp"


class Server {

    private:
        int _server_fd;
        int _port;
        std::string _password;
        std::vector<struct pollfd> _poll_fds;
        std::vector<Client*> _clients;

        void setupSocket();
        void setupServer();

    public:
        Server(int port, const std::string& password);
        ~Server();

        void start();
        void acceptNewClient();
        void handleClientData(int client_fd);
        void removeClient(int client_fd);

        //getters

        const std::string& getPassword() const;
};

#endif