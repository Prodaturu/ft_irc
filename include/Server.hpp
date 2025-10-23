#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <cctype>
#include <algorithm>
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
        const ChannelList& getChannels() const;
        Client* getClientByNickname(const string& nickname) const;
        // Channel* getChannelByName(const string& name);

        //parse commands
        
        //authentication
        void authenticator(const string& line, Client* client);
        bool checkNickname(stringList tokens, int client_fd) const;
        bool checkUsername(stringList tokens, int client_fd) const;
        bool checkPassword(stringList tokens, int client_fd) const;
        
        // Validation helpers
        bool isValidNickname(const string& nickname) const;
        bool isNicknameInUse(const string& nickname) const;
        bool isValidUsername(const string& username) const;
        void sendNumericalReply(int code, Client* client, const string& message) const;
        void sendWelcome(Client* client) const;
        
        // Safe send helper
        bool safeSend(int fd, const string& message);

        //commands
        void handleJoin(stringList tokens, Client* client);
        void joinSingleChannel(const string& channel_name, const string& provided_key, Client* client);
        void handlePart(stringList tokens, Client* client);
        void handlePrivmsg(stringList tokens, Client* client);

        //parse commands
        stringList parser(const string &input) const;

        // exec commands
        void execCommand(string line, Client* client);

};

#endif
