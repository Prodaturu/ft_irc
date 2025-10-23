/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Authentication.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohrahma <mohrahma@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 02:03:23 by mohrahma          #+#    #+#             */
/*   Updated: 2025/10/19 02:03:23 by mohrahma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Channel.hpp"
#include "../include/OperatorCommands.hpp"

void Server::authenticator(const string& line, Client* client)
{
    stringList tokens = parser(line);
    if (tokens.empty())
        return;

    if (tokens[0] == "CAP") 
    {
        send(client->getFd(), ":server CAP * LS :\r\n", 20, 0);
        return;
    } 
    else if (tokens[0] == "PASS") 
    {
        if (tokens.size() != 2) {
            sendNumericalReply(461, client, "PASS :Not enough parameters");
            return;
        }
        if (tokens[1] != _password) {
            sendNumericalReply(464, client, ":Password incorrect");
            return;
        }
        client->setAuthenticated(true);
        return;
    } 
    else if (tokens[0] == "NICK") 
    {
        if (tokens.size() != 2) {
            sendNumericalReply(461, client, "NICK :Not enough parameters");
            return;
        }
        
        if (!client->isAuthenticated()) {
            sendNumericalReply(451, client, ":You have not registered (send PASS first)");
            return;
        }
        
        if (!isValidNickname(tokens[1])) {
            sendNumericalReply(432, client, tokens[1] + " :Erroneous nickname");
            return;
        }
        
        if (isNicknameInUse(tokens[1])) {
            sendNumericalReply(433, client, tokens[1] + " :Nickname is already in use");
            return;
        }
        
        // Broadcast nick change to all channels if already authenticated
        if (client->isAuthenticated() && !client->getNickname().empty())
        {
            std::string old_nick = client->getNickname();
            std::string nick_msg = ":" + old_nick + "!~" + client->getUsername() 
                                 + "@localhost NICK " + tokens[1] + "\r\n";
            
            // Send to all channels the user is in
            for (size_t i = 0; i < _channels.size(); i++)
            {
                if (_channels[i]->hasMember(client))
                {
                    std::vector<Client*> members = _channels[i]->getMembers();
                    for (size_t j = 0; j < members.size(); j++)
                    {
                        send(members[j]->getFd(), nick_msg.c_str(), nick_msg.length(), 0);
                    }
                }
            }
        }
        
        client->setNickname(tokens[1]);
        return;
    } 
    else if (tokens[0] == "USER") 
    {
        if (tokens.size() != 5) {
            sendNumericalReply(461, client, "USER :Not enough parameters");
            return;
        }
        
        if (!client->isAuthenticated()) {
            sendNumericalReply(451, client, ":You have not registered (send PASS first)");
            return;
        }
        
        if (!isValidUsername(tokens[1])) {
            sendNumericalReply(468, client, ":Your username is invalid");
            return;
        }
        
        client->setUsername(tokens[1]);
        client->setRealname(tokens[4]);
        
        if (client->isAuthenticated() && 
            !client->getNickname().empty() && 
            !client->getUsername().empty()) {
        }
        return;
    }

    if (!client->isAuthenticated() || 
        client->getNickname().empty() || 
        client->getUsername().empty()) {
        sendNumericalReply(451, client, ":You have not registered");
    }
}

bool Server::isValidNickname(const string& nickname) const
{
    if (nickname.empty() || nickname.length() > 9)
        return false;
    
    if (!std::isalpha(nickname[0]))
        return false;
    
    for (size_t i = 1; i < nickname.length(); i++) {
        char c = nickname[i];
        if (!std::isalnum(c) && c != '-' && c != '_')
            return false;
    }
    
    return true;
}

bool Server::isNicknameInUse(const string& nickname) const
{
    for (size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i]->getNickname() == nickname)
            return true;
    }
    return false;
}

bool Server::isValidUsername(const string& username) const
{
    return !username.empty() && username.length() <= 10;
}

void Server::sendNumericalReply(int code, Client* client, const string& message) const
{
    string nick = client->getNickname().empty() ? "*" : client->getNickname();
    
    std::stringstream ss;
    ss << ":server " << code << " " << nick << " " << message << "\r\n";
    string reply = ss.str();
    
    send(client->getFd(), reply.c_str(), reply.length(), 0);
}
