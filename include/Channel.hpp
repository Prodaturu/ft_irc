#ifndef CHANNEL_CPP
#define CHANNEL_CPP

#include <string>
#include <vector>
#include <sys/socket.h>

#include "Client.hpp"
#include "Server.hpp"

typedef std::string string;
typedef std::vector<Client*> ClientList;

class Modes {
    public:
        bool i; // Set/remove Invite-only channel
        bool t; // Set/remove the restrictions of the TOPIC command to channel operators
        bool k; // Set/remove the channel key (password)
        bool o; // Give/take channel operator privilege
        bool l; // Set/remove the user limit to channel
}

class Channel {

    private:
        string name;
        string topic;
        ClientList operators;
        ClientList members;
        ClientList invitedClients;
        Modes modes;

    public:
        Channel(const string& name, const string& topic = "", const Modes& modes = Modes(), );

        //getters
        const string& getName() const;
        const string& getTopic() const;
        const ClientList& getMembers() const;
        size_t getMemberCount() const;

        bool hasMember(Client* client) const;
        void addMember(Client* client);
        void removeMember(Client* client);
        
        bool isOperator(Client* client) const;
        void addOperator(Client* client);
        void removeOperator(Client* client);

        void setTopic(const string& topic);

        void broadcast(const string& message, Client* exclude = NULL);
};

#endif