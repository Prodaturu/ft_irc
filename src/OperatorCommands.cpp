/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   OperatorCommands.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohrahma <mohrahma@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:37:07 by mohrahma          #+#    #+#             */
/*   Updated: 2025/10/18 16:37:07 by mohrahma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/OperatorCommands.hpp"
#include "../include/Server.hpp"
#include "../include/Channel.hpp"

void Server::execCommand(string line, Client* client)
{
    stringList tokens = parser(line);
    if (tokens.empty())
        return ;
    
    if (tokens[0] == "PASS" || tokens[0] == "NICK" || tokens[0] == "USER")
        return;
    
    // handle seperately because it can create channels
    if (tokens[0] == "JOIN") {
        handleJoin(tokens, client);
        return ;
    }
    
    // handle PRIVMSG separately becuase it can target channels or users
    if (tokens[0] == "PRIVMSG") {
        handlePrivmsg(tokens, client);
        return;
    }
    
    // we handle quit seperately from other OperatorCommands because it doesnt need a channel
    if (tokens[0] == "QUIT") {
        OperatorCommands().Quit(tokens, client, this);
        return;
    }
    
    if (tokens.size() < 2)
        return;
    
    Channel* channel = getChannelByName(tokens[1]);
    if (!channel) {
        send(client->getFd(), "ERROR: No such channel\r\n", 24, 0);
        return;
    }

    if (tokens[0] == "KICK") 
        OperatorCommands().Kick(tokens, client, channel);
    else if (tokens[0] == "INVITE")
        OperatorCommands().Invite(tokens, client, channel, this);
    else if (tokens[0] == "TOPIC")
        OperatorCommands().Topic(tokens, client, channel);
    else if (tokens[0] == "MODE")
        OperatorCommands().Mode(tokens, client, channel, this);
}

void OperatorCommands::Kick(stringList tokens, Client* client, Channel* channel) {
    
    if (tokens.size() < 3) {
        // ERR_NEEDMOREPARAMS (461)
        std::string error = ":localhost 461 " + client->getNickname() + " KICK :Not enough parameters\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (!channel->hasMember(client)) {
        // ERR_NOTONCHANNEL (442)
        std::string error = ":localhost 442 " + client->getNickname() + " " + channel->getName() + " :You're not on that channel\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (!channel->isOperator(client)) {
        // ERR_CHANOPRIVSNEEDED (482)
        std::string error = ":localhost 482 " + client->getNickname() + " " + channel->getName() + " :You're not channel operator\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    string target_nick = tokens[2];
    
    Client* target = NULL;
    const ClientList& members = channel->getMembers();
    for (size_t i = 0; i < members.size(); i++) {
        if (members[i]->getNickname() == target_nick) {
            target = members[i];
            break;
        }
    }

    if (!target) {
        // ERR_USERNOTINCHANNEL (441)
        std::string error = ":localhost 441 " + client->getNickname() + " " + target_nick + " " + channel->getName() + " :They aren't on that channel\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    string reason = client->getNickname();
    if (tokens.size() >= 4) {
        reason = "";
        for (size_t i = 3; i < tokens.size(); i++) {
            if (i > 3)
                reason += " ";
            reason += tokens[i];
        }
        if (!reason.empty() && reason[0] == ':')
            reason = reason.substr(1);
    }

    std::string kickMessage = ":" + client->getNickname() + "!~" + client->getUsername() 
                            + "@localhost KICK " + channel->getName() + " " + target_nick 
                            + " :" + reason + "\r\n";
    
    channel->broadcast(kickMessage, NULL);

    channel->removeMember(target);
    
    std::cout << "[KICK] " << client->getNickname() << " kicked " << target_nick 
              << " from " << channel->getName() << " (" << reason << ")" << std::endl;
}

void OperatorCommands::Invite(stringList tokens, Client* client, Channel* channel, Server* server) {

    if (tokens.size() < 3) {
        std::stringstream error;
        error << ":server 461 " << client->getNickname() 
              << " INVITE :Not enough parameters\r\n";
        send(client->getFd(), error.str().c_str(), error.str().length(), 0);
        return;
    }
    
    if (tokens[2] != channel->getName()) {
        std::stringstream error;
        error << ":server 403 " << client->getNickname() 
              << " " << tokens[2] << " :No such channel\r\n";
        send(client->getFd(), error.str().c_str(), error.str().length(), 0);
        return;
    }

    if (!channel->isOperator(client)) {
        std::stringstream error;
        error << ":server 482 " << client->getNickname() 
              << " " << channel->getName() 
              << " :You're not channel operator\r\n";
        send(client->getFd(), error.str().c_str(), error.str().length(), 0);
        return;
    }

    Client* target_client = server->getClientByNickname(tokens[1]);
    if (!target_client) {
        std::stringstream error;
        error << ":server 401 " << client->getNickname() 
              << " " << tokens[1] << " :No such nick/channel\r\n";
        send(client->getFd(), error.str().c_str(), error.str().length(), 0);
        return;
    }

    if (channel->hasMember(target_client)) {
        std::stringstream error;
        error << ":server 443 " << client->getNickname() 
              << " " << tokens[1] 
              << " " << channel->getName() 
              << " :is already on channel\r\n";
        send(client->getFd(), error.str().c_str(), error.str().length(), 0);
        return;
    }

    channel->addInvited(target_client);

    std::stringstream invite_msg_stream;
    invite_msg_stream << ":" << client->getNickname() << "!user@hostname"
                      << " INVITE " << tokens[1]
                      << " :" << channel->getName() << "\r\n";
    std::string invite_msg = invite_msg_stream.str();
    send(target_client->getFd(), invite_msg.c_str(), invite_msg.length(), 0);

    std::stringstream confirm_msg_stream;
    confirm_msg_stream << ":server 341 " << client->getNickname()
                       << " " << tokens[1]
                       << " " << channel->getName() << "\r\n";
    std::string confirm_msg = confirm_msg_stream.str();
    send(client->getFd(), confirm_msg.c_str(), confirm_msg.length(), 0);

    std::stringstream broadcast_stream;
    broadcast_stream << ":" << client->getNickname() << "!user@hostname"
                     << " NOTICE " << channel->getName()
                     << " :" << tokens[1] << " has been invited\r\n";
    std::string broadcast_msg = broadcast_stream.str();
    channel->broadcast(broadcast_msg, NULL);
}

void OperatorCommands::Topic(stringList tokens, Client* client, Channel* channel) {
    //how to use: TOPIC #channel [new topic]
    
    if (tokens.size() < 2) {
        // ERR_NEEDMOREPARAMS (461)
        std::string error = ":localhost 461 " + client->getNickname() + " TOPIC :Not enough parameters\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (channel == NULL) {
        // ERR_NOSUCHCHANNEL (403)
        std::string error = ":localhost 403 " + client->getNickname() + " " + tokens[1] + " :No such channel\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Check if client is in the channel
    if (!channel->hasMember(client)) {
        // ERR_NOTONCHANNEL (442)
        std::string error = ":localhost 442 " + client->getNickname() + " " + channel->getName() + " :You're not on that channel\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (tokens.size() == 2) {
        string current_topic = channel->getTopic();
        
        if (current_topic.empty()) {
            // RPL_NOTOPIC (331)
            std::string response = ":localhost 331 " + client->getNickname() + " " + channel->getName() + " :No topic is set\r\n";
            send(client->getFd(), response.c_str(), response.length(), 0);
        } else {
            // RPL_TOPIC (332)
            std::string response = ":localhost 332 " + client->getNickname() + " " + channel->getName() + " :" + current_topic + "\r\n";
            send(client->getFd(), response.c_str(), response.length(), 0);
        }
        return;
    }

    const Modes& modes = channel->getModes();
    if (modes.t && !channel->isOperator(client)) {
        // ERR_CHANOPRIVSNEEDED (482)
        std::string error = ":localhost 482 " + client->getNickname() + " " + channel->getName() + " :You're not channel operator\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string new_topic = "";
    for (size_t i = 2; i < tokens.size(); i++) {
        if (i > 2)
            new_topic += " ";
        new_topic += tokens[i];
    }
    
    if (!new_topic.empty() && new_topic[0] == ':')
        new_topic = new_topic.substr(1);

    channel->setTopic(new_topic);

    std::string topicMessage = ":" + client->getNickname() + "!~" + client->getUsername()
                             + "@localhost TOPIC " + channel->getName()
                             + " :" + new_topic + "\r\n";
    channel->broadcast(topicMessage, NULL);
    
    std::cout << "[TOPIC] " << client->getNickname() << " changed topic of " 
              << channel->getName() << " to: " << new_topic << std::endl;
}

void OperatorCommands::Mode(stringList tokens, Client* client, Channel* channel, Server* server) {
    // MODE #channel [+/-modes] [parameters]
    
    if (tokens.size() < 2) {
        // ERR_NEEDMOREPARAMS (461)
        std::string error = ":localhost 461 " + client->getNickname() + " MODE :Not enough parameters\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Query mode (no mode changes specified)
    if (tokens.size() == 2) {
        const Modes& modes = channel->getModes();
        std::string mode_string = "+";
        if (modes.i) mode_string += "i";
        if (modes.t) mode_string += "t";
        if (modes.k) mode_string += "k";
        if (modes.l) mode_string += "l";
        
        // RPL_CHANNELMODEIS (324)
        std::string response = ":localhost 324 " + client->getNickname() + " " + channel->getName() + " " + mode_string + "\r\n";
        send(client->getFd(), response.c_str(), response.length(), 0);
        return;
    }

    // Check if client is an operator
    if (!channel->isOperator(client)) {
        // ERR_CHANOPRIVSNEEDED (482)
        std::string error = ":localhost 482 " + client->getNickname() + " " + channel->getName() + " :You're not channel operator\r\n";
        send(client->getFd(), error.c_str(), error.length(), 0);
        return;
    }

    string modestring = tokens[2];
    bool adding = true;
    size_t param_index = 3;
    
    std::string applied_modes = "";
    std::string applied_params = "";

    for (size_t i = 0; i < modestring.length(); i++) {
        char mode = modestring[i];
        
        if (mode == '+') {
            adding = true;
            continue;
        } else if (mode == '-') {
            adding = false;
            continue;
        }

        if (mode == 'i') {
            // invite only mode
            channel->setModeI(adding);
            applied_modes += (adding ? "+" : "-");
            applied_modes += "i";
        }
        else if (mode == 't') {
            // topic restriction
            channel->setModeT(adding);
            applied_modes += (adding ? "+" : "-");
            applied_modes += "t";
        }
        else if (mode == 'k') {
            // channel key (pw)
            if (adding) {
                if (param_index < tokens.size()) {
                    string key = tokens[param_index++];
                    channel->setKey(key);
                    channel->setModeK(true);
                    applied_modes += "+k";
                    applied_params += " " + key;
                }
            } else {
                channel->setKey("");
                channel->setModeK(false);
                applied_modes += "-k";
            }
        }
        else if (mode == 'o') {
            if (param_index < tokens.size()) {
                string target_nick = tokens[param_index++];
                Client* target = server->getClientByNickname(target_nick);
                
                if (!target) {
                    // ERR_NOSUCHNICK (401)
                    std::string error = ":localhost 401 " + client->getNickname() + " " + target_nick + " :No such nick/channel\r\n";
                    send(client->getFd(), error.c_str(), error.length(), 0);
                    continue;
                }
                
                if (!channel->hasMember(target)) {
                    // ERR_USERNOTINCHANNEL (441)
                    std::string error = ":localhost 441 " + client->getNickname() + " " + target_nick + " " + channel->getName() + " :They aren't on that channel\r\n";
                    send(client->getFd(), error.c_str(), error.length(), 0);
                    continue;
                }
                
                if (adding) {
                    channel->addOperator(target);
                } else {
                    channel->removeOperator(target);
                }
                applied_modes += (adding ? "+o" : "-o");
                applied_params += " " + target_nick;
            }
        }
        else if (mode == 'l') {
            // User limit
            if (adding) {
                if (param_index < tokens.size()) {
                    string limit_str = tokens[param_index++];
                    size_t limit = std::atoi(limit_str.c_str());
                    channel->setUserLimit(limit);
                    channel->setModeL(true);
                    applied_modes += "+l";
                    applied_params += " " + limit_str;
                }
            } else {
                channel->setUserLimit(0);
                channel->setModeL(false);
                applied_modes += "-l";
            }
        }
        else {
            // ERR_UNKNOWNMODE (472)
            std::string error = ":localhost 472 " + client->getNickname() + " " + string(1, mode) + " :is unknown mode char to me\r\n";
            send(client->getFd(), error.c_str(), error.length(), 0);
            std::cout << "[MODE] Unknown mode '" << mode << "' attempted by " << client->getNickname() << " on " << channel->getName() << std::endl;
        }
    }

    // Broadcast mode changes to all channel members
    if (!applied_modes.empty()) {
        std::string mode_msg = ":" + client->getNickname() + "!~" + client->getUsername()
                             + "@localhost MODE " + channel->getName() + " " + applied_modes + applied_params + "\r\n";
        channel->broadcast(mode_msg, NULL);
        
        std::cout << "[MODE] " << client->getNickname() << " set modes on " 
                  << channel->getName() << ": " << applied_modes << applied_params << std::endl;
    }
}

void OperatorCommands::Quit(stringList tokens, Client* client, Server* server) {
    int client_fd = client->getFd();
    string quit_reason = "Client quit";
    
    if (tokens.size() > 1) {
        std::stringstream reason_builder;
        for (size_t i = 1; i < tokens.size(); i++) {
            reason_builder << tokens[i];
            if (i < tokens.size() - 1)
                reason_builder << " ";
        }
        quit_reason = reason_builder.str();
    }

    std::stringstream quit_message_stream;
    quit_message_stream << ":" << client->getNickname() << "!user@hostname"
                        << " QUIT :" << quit_reason << "\r\n";
    string quit_message = quit_message_stream.str();

    ChannelList channels = server->getChannels();
    for (size_t i = 0; i < channels.size(); i++) {
        Channel* channel = channels[i];
        if (channel && channel->hasMember(client)) {
            channel->broadcast(quit_message, client);
            channel->removeMember(client);
        }
    }

    std::stringstream quit_response;
    quit_response << ":server QUIT :" << quit_reason << "\r\n";
    string response = quit_response.str();

    send(client_fd, response.c_str(), response.length(), MSG_DONTWAIT);

    server->removeClient(client_fd);
}

void OperatorCommands::Join(stringList tokens, Client* client) { (void)tokens; (void)client; }