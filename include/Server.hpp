#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <csignal>
#include <sstream>

#include "Client.hpp"

// Forward declaration to avoid circular include (Channel.hpp includes Server.hpp)
class Channel;

typedef std::string string;
typedef std::vector<string> stringList;
typedef std::vector<Client*> ClientList;
typedef std::vector<Channel*> ChannelList;

class Server
{

    private:
        int _server_fd;
        int _port;
        string _password;
        std::vector<struct pollfd> _poll_fds;
        ClientList _clients;
        ChannelList _channels;

        void setupSocket();
        void setupServer();
        Client* getClientByFd(int client_fd);
        Channel* getChannelByName(const string& name);
        Channel* createChannel(const string& name);

    public:
        Server(int port, const std::string& password);
        ~Server();

        void start();
        void acceptNewClient();
        void handleClientData(int client_fd);
        void removeClient(int client_fd);

        //getters
        const std::string& getPassword() const;

        //parse commands
        
        //authentication
        void authenticator(string line, Client* client, int client_fd);
        bool checkNickname(stringList tokens, int client_fd) const;
        bool checkUsername(stringList tokens, int client_fd) const;
        bool checkPassword(stringList tokens, int client_fd) const;

        //commands
        void handleJoin(stringList tokens, Client* client);

        //helper function temp
        stringList parser(const string &input) const;

};

#endif
