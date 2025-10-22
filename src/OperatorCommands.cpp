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

void Server::execCommand(string line, Client* client)
{
    stringList tokens = parser(line);
    if (tokens.empty())
    return ;
    Channel* channel = getChannelByName(tokens[1]);
    if (!channel) {
        send(client->getFd(), "ERROR: No such channel\r\n", 24, 0);
        return;
    }

    if (tokens[0] == "JOIN")
        OperatorCommands().Join(tokens, client);
    else if (tokens[0] == "KICK") 
        OperatorCommands().Kick(tokens, client, channel);
    else if (tokens[0] == "INVITE")
        OperatorCommands().Invite(tokens, client);
    else if (tokens[0] == "TOPIC")
        OperatorCommands().Topic(tokens, client);
    else if (tokens[0] == "MODE")
        OperatorCommands().Mode(tokens, client);
    else if (tokens[0] == "QUIT")
        OperatorCommands().Quit(tokens, client);
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

void OperatorCommands::Topic(stringList tokens, Client* client) { (void)tokens; (void)client; }

void OperatorCommands::Mode(stringList tokens, Client* client) { (void)tokens; (void)client; }

void OperatorCommands::Quit(stringList tokens, Client* client) { (void)tokens; (void)client; }

void OperatorCommands::Join(stringList tokens, Client* client) { (void)tokens; (void)client; }