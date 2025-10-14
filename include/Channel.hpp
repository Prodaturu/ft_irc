#ifndef CHANNEL_CPP
#define CHANNEL_CPP

#include <string>
#include <vector>
#include <sys/socket.h>
#include "Client.hpp"

class Channel {

    private:
        std::string _name;
        std::string _topic;
        std::vector<Client*> _members;
        std::vector<Client*> _operators;

    public:
        Channel(const std::string& name);
        ~Channel();

        //getters
        const std::string& getName() const;
        const std::string& getTopic() const;
        const std::vector<Client*>& getMembers() const;
        size_t getMemberCount() const;

        void addMember(Client* client);
        void removeMember(Client* client);
        bool hasMember(Client* client) const;
        
        void addOperator(Client* client);
        void removeOperator(Client* client);
        bool isOperator(Client* client) const;

        void setTopic(const std::string& topic);

        void broadcast(const std::string& message, Client* exclude = NULL);
};

#endif