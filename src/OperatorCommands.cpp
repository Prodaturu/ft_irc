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
        OperatorCommands().Mode(tokens, client);
}

void OperatorCommands::Kick(stringList tokens, Client* client, Channel* channel) {
    if (tokens.size() < 2) {
        send(client->getFd(), "ERROR: KICK command requires a channel parameter\r\n", 48, 0);
        return;
    }

    if (!channel->isOperator(client)) {
        send(client->getFd(), "ERROR: You are not a channel operator\r\n", 39, 0);
        return;
    }

    channel->removeMember(client);
    std::stringstream kickMessageStream;
	kickMessageStream << ":" << client->getNickname() << "!user@" << "hostname"
					  << " KICK " << channel->getName() << " " << client->getNickname()
					  << " :" << tokens[3] << "\r\n";
	string kickMessage = kickMessageStream.str();
	channel->broadcast(kickMessage, client);
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
    if (tokens.size() < 2) {
        send(client->getFd(), "ERROR: TOPIC command requires a channel parameter\r\n", 53, 0);
        return;
    }

    if (channel == NULL) {
        send(client->getFd(), "ERROR: No such channel\r\n", 24, 0);
        return;
    }

    if (tokens.size() == 2) {
        string current_topic = channel->getTopic();
        std::stringstream response;
        
        if (current_topic.empty()) {
            response << ":server TOPIC " << channel->getName() << " :\r\n";
        } else {
            response << ":server TOPIC " << channel->getName() << " :" << current_topic << "\r\n";
        }
        
        string reply = response.str();
        send(client->getFd(), reply.c_str(), reply.length(), 0);
        return;
    }

    if (!channel->isOperator(client)) {
        send(client->getFd(), "ERROR: You are not a channel operator\r\n", 39, 0);
        return;
    }

    std::stringstream topicBuilder;
    for (size_t i = 2; i < tokens.size(); i++) {
        topicBuilder << tokens[i];
        if (i < tokens.size() - 1)
            topicBuilder << " ";
    }
    string new_topic = topicBuilder.str();

    channel->setTopic(new_topic);

    std::stringstream topicMessageStream;
    topicMessageStream << ":" << client->getNickname() << "!user@hostname"
                       << " TOPIC " << channel->getName()
                       << " :" << new_topic << "\r\n";
    string topicMessage = topicMessageStream.str();
    channel->broadcast(topicMessage, NULL);
}

void OperatorCommands::Mode(stringList tokens, Client* client) { (void)tokens; (void)client; }

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