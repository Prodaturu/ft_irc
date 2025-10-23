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

// Helper function to join a single channel
void Server::joinSingleChannel(const string& channel_name, const string& provided_key, Client* client)
{
    if (channel_name.empty() || (channel_name[0] != '#' && channel_name[0] != '&'))
    {
        std::string error = "476 " + client->getNickname() + " " + channel_name 
                          + " :Bad Channel Mask\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    Channel* channel = getChannelByName(channel_name);
    if (!channel)
        channel = createChannel(channel_name);

    // ERR_USERONCHANNEL (443)
    if (channel->hasMember(client))
    {
        std::string error = ":localhost 443 " + client->getNickname() + " " + client->getNickname() 
                          + " " + channel_name + " :is already on channel\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // ERR_INVITEONLYCHAN (473) - invite-only mode
    if (channel->getModes().i && !channel->isInvited(client))
    {
        std::string error = ":localhost 473 " + client->getNickname() + " " 
                          + channel_name + " :Cannot join channel (+i) - invite only\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // ERR_BADCHANNELKEY (475) - channel key
    if (!channel->getKey().empty() && channel->getKey() != provided_key)
    {
        std::string error = ":localhost 475 " + client->getNickname() + " " 
                          + channel_name + " :Cannot join channel (+k) - bad key\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // ERR_CHANNELISFULL (471) - user limit
    if (channel->getModes().l && channel->getUserLimit() > 0 
        && channel->getMemberCount() >= channel->getUserLimit())
    {
        std::string error = ":localhost 471 " + client->getNickname() + " " 
                          + channel_name + " :Cannot join channel (+l) - channel is full\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Remove from invited list if present (invite consumed)
    if (channel->isInvited(client))
        channel->removeInvited(client);

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
        std::string error = "461 " + client->getNickname() + " JOIN :Not enough parameters\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Parse comma-separated channel list
    std::vector<string> channels;
    std::vector<string> keys;
    
    string channel_list = tokens[1];
    size_t pos = 0;
    while (pos < channel_list.length())
    {
        size_t comma = channel_list.find(',', pos);
        if (comma == string::npos)
        {
            channels.push_back(channel_list.substr(pos));
            break;
        }
        channels.push_back(channel_list.substr(pos, comma - pos));
        pos = comma + 1;
    }

    // Parse comma-separated keys if provided
    if (tokens.size() >= 3)
    {
        string key_list = tokens[2];
        pos = 0;
        while (pos < key_list.length())
        {
            size_t comma = key_list.find(',', pos);
            if (comma == string::npos)
            {
                keys.push_back(key_list.substr(pos));
                break;
            }
            keys.push_back(key_list.substr(pos, comma - pos));
            pos = comma + 1;
        }
    }

    // Join each channel with corresponding key (or empty string)
    for (size_t i = 0; i < channels.size(); i++)
    {
        string key = (i < keys.size()) ? keys[i] : "";
        joinSingleChannel(channels[i], key, client);
    }
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

// PART Command - Leave a channel
void Server::handlePart(stringList tokens, Client* client)
{
    // ERR_NEEDMOREPARAMS (461)
    if (tokens.size() < 2)
    {
        std::string err = "461 " + client->getNickname() + " PART :Not enough parameters\r\n";
        send(client->getFd(), err.c_str(), err.length(), 0);
        return;
    }

    std::cout << "[PART DEBUG] tokens.size() = " << tokens.size() << std::endl;
    for (size_t i = 0; i < tokens.size(); i++) {
        std::cout << "[PART DEBUG] tokens[" << i << "] = '" << tokens[i] << "'" << std::endl;
    }

    // Parse comma-separated channel list
    std::vector<string> channels;
    string channel_list = tokens[1];
    size_t pos = 0;
    while (pos < channel_list.length())
    {
        size_t comma = channel_list.find(',', pos);
        string channel_name;
        if (comma == string::npos)
        {
            channel_name = channel_list.substr(pos);
            // Trim whitespace
            size_t start = channel_name.find_first_not_of(" \t\r\n");
            size_t end = channel_name.find_last_not_of(" \t\r\n");
            if (start != string::npos && end != string::npos)
                channel_name = channel_name.substr(start, end - start + 1);
            if (!channel_name.empty())
                channels.push_back(channel_name);
            break;
        }
        channel_name = channel_list.substr(pos, comma - pos);
        // Trim whitespace
        size_t start = channel_name.find_first_not_of(" \t\r\n");
        size_t end = channel_name.find_last_not_of(" \t\r\n");
        if (start != string::npos && end != string::npos)
            channel_name = channel_name.substr(start, end - start + 1);
        if (!channel_name.empty())
            channels.push_back(channel_name);
        pos = comma + 1;
    }

    std::cout << "[PART DEBUG] Parsed " << channels.size() << " channels:" << std::endl;
    for (size_t i = 0; i < channels.size(); i++) {
        std::cout << "[PART DEBUG] Channel " << i << ": '" << channels[i] << "'" << std::endl;
    }

    // Extract reason (applies to all channels)
    std::string reason = "";
    if (tokens.size() >= 3)
    {
        reason = tokens[2];
        for (size_t i = 3; i < tokens.size(); i++)
            reason += " " + tokens[i];
        if (!reason.empty() && reason[0] == ':')
            reason = reason.substr(1);
    }

    // Part from each channel
    for (size_t i = 0; i < channels.size(); i++)
    {
        string channelName = channels[i];
        Channel* channel = getChannelByName(channelName);
        
        // ERR_NOSUCHCHANNEL (403)
        if (!channel)
        {
            std::string err = "403 " + client->getNickname() + " " + channelName + " :No such channel\r\n";
            send(client->getFd(), err.c_str(), err.length(), 0);
            continue;
        }

        // ERR_NOTONCHANNEL (442)
        if (!channel->hasMember(client))
        {
            std::string err = "442 " + client->getNickname() + " " + channelName + " :You're not on that channel\r\n";
            send(client->getFd(), err.c_str(), err.length(), 0);
            continue;
        }

        std::string part_msg = ":" + client->getNickname() + "!~" + client->getUsername() 
                             + "@localhost PART " + channelName;
        if (!reason.empty())
            part_msg += " :" + reason;
        part_msg += "\r\n";

        std::vector<Client*> members = channel->getMembers();
        for (size_t j = 0; j < members.size(); j++)
        {
            send(members[j]->getFd(), part_msg.c_str(), part_msg.length(), 0);
        }

        std::cout << "[PART] " << client->getNickname() << " left " << channelName;
        if (!reason.empty())
            std::cout << " (" << reason << ")";
        std::cout << std::endl;

        channel->removeMember(client);

        if (channel->getMembers().empty())
        {
            std::cout << "[CHANNEL] Deleting empty channel: " << channelName << std::endl;
            for (size_t j = 0; j < _channels.size(); j++)
            {
                if (_channels[j] == channel)
                {
                    delete _channels[j];
                    _channels.erase(_channels.begin() + j);
                    break;
                }
            }
        }
    }
}
