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
    
    // we handle quit seperately from other OperatorCommands because it doesnt need a channel
    if (tokens[0] == "QUIT") {
        OperatorCommands().Quit(tokens, client);
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
        OperatorCommands().Invite(tokens, client);
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
	std::string kickMessage = kickMessageStream.str();
	channel->broadcast(kickMessage, client);
}

void OperatorCommands::Invite(stringList tokens, Client* client) { (void)tokens; (void)client; }

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
        std::string current_topic = channel->getTopic();
        std::stringstream response;
        
        if (current_topic.empty()) {
            response << ":server TOPIC " << channel->getName() << " :\r\n";
        } else {
            response << ":server TOPIC " << channel->getName() << " :" << current_topic << "\r\n";
        }
        
        std::string reply = response.str();
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
    std::string new_topic = topicBuilder.str();

    channel->setTopic(new_topic);

    std::stringstream topicMessageStream;
    topicMessageStream << ":" << client->getNickname() << "!user@hostname"
                       << " TOPIC " << channel->getName()
                       << " :" << new_topic << "\r\n";
    std::string topicMessage = topicMessageStream.str();
    channel->broadcast(topicMessage, NULL);
}

void OperatorCommands::Mode(stringList tokens, Client* client) { (void)tokens; (void)client; }

void OperatorCommands::Quit(stringList tokens, Client* client) { (void)tokens; (void)client; }

void OperatorCommands::Join(stringList tokens, Client* client) { (void)tokens; (void)client; }