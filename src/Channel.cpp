#include "Channel.hpp"

Channel::Channel(const std::string& name) : _name (name) {}

Channel::~Channel(){}

const std::string& Channel::getName() const {
    return _name;
}

const std::string& Channel::getTopic() const {
    return _topic;
}

const std::vector<Client*>& Channel::getMembers() const {
    return _members;
}

size_t Channel::getMemberCount() const {
    return _members.size();
}

void Channel::addMember(Client* client) {
    if (!hasMember(client)) {
        _members.push_back(client);
        if (_members.size() == 1)
            addOperator(client);
    }
}

void Channel::removeMember(Client* client) {
    for (size_t i = 0; i < _members.size(); i++) {
        if (_members[i] == client) {
                _members.erase(_members.begin() + i);
                break ; 
        }
    }
    removeOperator(client);
}

bool Channel::hasMember(Client* client) const {
    for (size_t i = 0; i < _members.size(); i++) {
        if(_members[i] == client)
            return true;
    }
    return false;
}

void Channel::addOperator(Client* client) {
    if (!isOperator(client))
        _operators.push_back(client);
}

void Channel::removeOperator(Client* client) {
    for (size_t i = 0; i < _operators.size(); i++) {
        if (_operators[i] == client) {
            _operators.erase(_operators.begin() + i);
            break;
        }
    }
}

bool Channel::isOperator(Client* client) const {
    for (size_t i = 0; i < _operators.size(); i++) {
        if (_operators[i] == client)
            return true;
    }
    return false;
}

void Channel::setTopic(const std::string& topic) {
    _topic = topic;
}

void Channel::broadcast(const std::string& message, Client* exclude) {
    std::string full_msg = message + "\r\n";
    for (size_t i = 0; i < _members.size(); i++)
        if (_members[i] != exclude)
            send(_members[i]->getFd(), full_msg.c_str(), full_msg.length(), 0);
}