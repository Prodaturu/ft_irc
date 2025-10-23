/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstefano <mstefano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 00:00:00 by mstefano          #+#    #+#             */
/*   Updated: 2025/10/22 00:00:00 by mstefano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Channel.hpp"


Channel* Server::getChannelByName(const string& name)
{
    for (size_t i = 0; i < _channels.size(); i++)
    {
        if (_channels[i]->getName() == name)
            return _channels[i];
    }
    return NULL;
}


Channel* Server::createChannel(const string& name)
{
    Channel* new_channel = new Channel(name);
    _channels.push_back(new_channel);
    std::cout << "[CHANNEL] Created new channel: " << name << std::endl;
    return new_channel;
}


void Server::handleJoin(stringList tokens, Client* client)
{
    if (!client->isAuthenticated() || client->getNickname().empty() || client->getUsername().empty())
    {
        std::string error = "ERROR :You must complete registration first (PASS, NICK, USER)\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (tokens.size() < 2)
    {
        std::string error = "ERROR :JOIN command requires a channel name\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    string channel_name = tokens[1];
    string provided_key = (tokens.size() >= 3) ? tokens[2] : "";

    if (channel_name.empty() || (channel_name[0] != '#' && channel_name[0] != '&'))
    {
        std::string error = "ERROR :Invalid channel name (must start with # or &)\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    Channel* channel = getChannelByName(channel_name);
    if (!channel)
        channel = createChannel(channel_name);

    //(ERR_BADCHANNELKEY 475)
    if (!channel->getKey().empty() && channel->getKey() != provided_key)
    {
        std::string error = ":localhost 475 " + client->getNickname() + " " 
                          + channel_name + " :Cannot join channel (+k) - bad key\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (channel->hasMember(client))
    {
        std::string error = "ERROR :You are already in channel " + channel_name + "\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    channel->addMember(client);
    
    std::cout << "[JOIN] " << client->getNickname() << " joined " << channel_name 
              << " (" << channel->getMemberCount() << " members)" << std::endl;

    std::string join_msg = ":" + client->getNickname() + "!~" + client->getUsername() 
                          + "@localhost JOIN " + channel_name + "\r\n";
    send(client->getFd(), join_msg.c_str(), join_msg.length(), 0);

    std::string broadcast_msg = ":" + client->getNickname() + "!~" + client->getUsername() 
                               + "@localhost JOIN " + channel_name;
    channel->broadcast(broadcast_msg, client);

    if (!channel->getTopic().empty())
    {
        std::string topic_msg = ":localhost 332 " + client->getNickname() + " " 
                               + channel_name + " :" + channel->getTopic() + "\r\n";
        send(client->getFd(), topic_msg.c_str(), topic_msg.length(), 0);
    }

    // (RPL_NAMREPLY 353)
    const ClientList& members = channel->getMembers();
    std::string names_list = ":localhost 353 " + client->getNickname() + " = " 
                            + channel_name + " :";
    
    for (size_t i = 0; i < members.size(); i++)
    {
        if (channel->isOperator(members[i]))
            names_list += "@";
        names_list += members[i]->getNickname();
        if (i < members.size() - 1)
            names_list += " ";
    }
    names_list += "\r\n";
    send(client->getFd(), names_list.c_str(), names_list.length(), 0);

    // (RPL_ENDOFNAMES 366)
    std::string end_of_names = ":localhost 366 " + client->getNickname() + " " 
                              + channel_name + " :End of /NAMES list\r\n";
    send(client->getFd(), end_of_names.c_str(), end_of_names.length(), 0);
}


void Server::handlePrivmsg(stringList tokens, Client* client)
{
    if (!client->isAuthenticated() || client->getNickname().empty() || client->getUsername().empty())
    {
        std::string error = "ERROR :You must complete registration first (PASS, NICK, USER)\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // ERR_NORECIPIENT (411)
    if (tokens.size() < 2)
    {
        std::string error = ":localhost 411 " + client->getNickname() + " :No recipient given (PRIVMSG)\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // ERR_NOTEXTTOSEND (412)
    if (tokens.size() < 3)
    {
        std::string error = ":localhost 412 " + client->getNickname() + " :No text to send\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    string target = tokens[1];
    
    string message = "";
    for (size_t i = 2; i < tokens.size(); i++)
    {
        if (i > 2)
            message += " ";
        message += tokens[i];
    }

    if (!message.empty() && message[0] == ':')
        message = message.substr(1);

    if (!target.empty() && (target[0] == '#' || target[0] == '&'))
    {
        Channel* channel = getChannelByName(target);
        if (!channel)
        {
            // ERR_NOSUCHCHANNEL (403)
            std::string error = ":localhost 403 " + client->getNickname() + " " + target + " :No such channel\r\n";
            send(client->getFd(), error.c_str(), error.length(), 0);
            return;
        }

        // ERR_CANNOTSENDTOCHAN (404)
        if (!channel->hasMember(client))
        {
            std::string error = ":localhost 404 " + client->getNickname() + " " + target + " :Cannot send to channel\r\n";
            send(client->getFd(), error.c_str(), error.length(), 0);
            return;
        }

        // broadcasted message to everyone 
        std::string channel_msg = ":" + client->getNickname() + "!~" + client->getUsername() 
                                 + "@localhost PRIVMSG " + target + " :" + message;
        channel->broadcast(channel_msg, client);
        
        std::cout << "[PRIVMSG] " << client->getNickname() << " -> " << target << ": " << message << std::endl;
    }
    else
    {
        // messages to user (sliding in the DMs)
        Client* recipient = NULL;
        for (size_t i = 0; i < _clients.size(); i++)
        {
            if (_clients[i]->getNickname() == target)
            {
                recipient = _clients[i];
                break;
            }
        }

        if (!recipient)
        {
            // ERR_NOSUCHNICK (401)
            std::string error = ":localhost 401 " + client->getNickname() + " " + target + " :No such nick/channel\r\n";
            send(client->getFd(), error.c_str(), error.length(), 0);
            return;
        }

        std::string dm_msg = ":" + client->getNickname() + "!~" + client->getUsername() 
                           + "@localhost PRIVMSG " + target + " :" + message + "\r\n";
        send(recipient->getFd(), dm_msg.c_str(), dm_msg.length(), 0);
        
        std::cout << "[PRIVMSG] " << client->getNickname() << " -> " << target << " (DM): " << message << std::endl;
    }
}
