#ifndef CHANNEL_CPP
#define CHANNEL_CPP

#include <string>
#include <vector>
#include <sys/socket.h>

#include "Client.hpp"
#include "Server.hpp"

typedef std::string string;
typedef std::vector<Client*> ClientList;

class Channel {

    private:
        string name;
        string topic;
        ClientList operators;
        ClientList members;
        ClientList invitedClients;

    public:
        Channel(const string& name);
        ~Channel();
        Channel(const Channel &toCopy);
        Channel &operator=(const Channel &toCopy);

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