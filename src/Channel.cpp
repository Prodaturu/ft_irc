#include "../include/Channel.hpp"

Channel::Channel(const std::string& name) : name(name), userLimit(0) {
    modes.i = false;
    modes.t = false;
    modes.k = false;
    modes.o = false;
    modes.l = false;
}

const std::string& Channel::getName() const {
    return name;
}

const std::string& Channel::getTopic() const {
    return topic;
}

const std::string& Channel::getKey() const {
    return key;
}

const std::vector<Client*>& Channel::getMembers() const {
    return members;
}

size_t Channel::getMemberCount() const {
    return members.size();
}

const Modes& Channel::getModes() const {
    return modes;
}

bool Channel::hasMember(Client* client) const {
    for (size_t i = 0; i < members.size(); i++) {
        if(members[i] == client)
            return true;
    }
    return false;
}

void Channel::addMember(Client* client) {
    if (!hasMember(client)) {
        members.push_back(client);
        if (members.size() == 1)
            addOperator(client);
    }
}

void Channel::removeMember(Client* client) {
    for (size_t i = 0; i < members.size(); i++) {
        if (members[i] == client) {
            members.erase(members.begin() + i);
            break;
        }
    }
    removeOperator(client);
}

bool Channel::isOperator(Client* client) const {
    for (size_t i = 0; i < operators.size(); i++) {
        if (operators[i] == client)
            return true;
    }
    return false;
}

void Channel::addOperator(Client* client) {
    if (!isOperator(client))
        operators.push_back(client);
}

void Channel::removeOperator(Client* client) {
    for (size_t i = 0; i < operators.size(); i++) {
        if (operators[i] == client) {
            operators.erase(operators.begin() + i);
            break;
        }
    }
}

bool Channel::isInvited(Client* client) const {
    for (size_t i = 0; i < invitedClients.size(); i++) {
        if (invitedClients[i] == client)
            return true;
    }
    return false;
}

void Channel::addInvited(Client* client) {
    if (!isInvited(client))
        invitedClients.push_back(client);
}

void Channel::removeInvited(Client* client) {
    for (size_t i = 0; i < invitedClients.size(); i++) {
        if (invitedClients[i] == client) {
            invitedClients.erase(invitedClients.begin() + i);
            break;
        }
    }
}

void Channel::setTopic(const std::string& topic) {
    this->topic = topic;
}

void Channel::setKey(const std::string& key) {
    this->key = key;
}

void Channel::setUserLimit(size_t limit) {
    this->userLimit = limit;
}

size_t Channel::getUserLimit() const {
    return userLimit;
}

void Channel::setModeI(bool value) {
    modes.i = value;
}

void Channel::setModeT(bool value) {
    modes.t = value;
}

void Channel::setModeK(bool value) {
    modes.k = value;
}

void Channel::setModeL(bool value) {
    modes.l = value;
}

void Channel::broadcast(const std::string& message, Client* exclude) {
    std::string full_msg = message + "\r\n";
    for (size_t i = 0; i < members.size(); i++)
        if (members[i] != exclude)
            send(members[i]->getFd(), full_msg.c_str(), full_msg.length(), 0);
}
