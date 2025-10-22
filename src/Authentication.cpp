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
#include "../include/OperatorCommands.hpp"

bool Client::authenticator(std::string line, Client *client, std::string password, int client_fd) const
{
    std::string errorMessage;
    std::vector<std::string> tokens = Server::split(line);
    if (tokens.empty())
        return false;
    std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::toupper);

    // IRC Standard commands (not yet authenticated)
    if (tokens[0] == "PASS" && client->checkPassword(tokens, client, password))
        return (client->setAuthenticated(true), true);
    else if (tokens[0] == "NICK" && client->checkNickname(tokens, client_fd))
        return (client->setNickname(tokens[1]), true);
    else if (tokens[0] == "USER" && client->checkUsername(tokens, client_fd))
        return (client->setUsername(tokens[1]), client->setRealname(tokens[4]), true);
    else if (tokens[0] == "QUIT")
        return (OperatorCommands().Quit(tokens, client_fd), false);
    else if (client->getNickname().empty() || client->getUsername().empty() || !client->isAuthenticated())
        errorMessage = 
        "ERROR :You must authenticate first\r\n"
        "AUTHENTICATE USING:\r\n"
        "PASS <password>\r\n"
        "NICK <nickname>\r\n"
        "USER <username> <hostname> <servername> <realname>\r\n"
        "In any given order\r\n";
            else if (tokens[0] == "KICK") return (OperatorCommands().Kick(tokens, client_fd), true);
    else if (tokens[0] == "INVITE")
        return (OperatorCommands().Invite(tokens, client_fd), true);
    else if (tokens[0] == "TOPIC")
        return (OperatorCommands().Topic(tokens, client_fd), true);
    else if (tokens[0] == "MODE")
        return (OperatorCommands().Mode(tokens, client_fd), true);

    if (!errorMessage.empty())
        send(client_fd, errorMessage.c_str(), errorMessage.length(), 0);
    return false;
}

bool Client::checkNickname(std::vector<std::string> tokens, int client_fd) const
{
    std::string errorMessage;

    if (tokens.size() == 2)
        return true;
    else if (tokens.size() < 2)
        errorMessage = "ERROR :NICKNAME command requires a nickname parameter\r\n";
    else
        errorMessage = "ERROR :NICKNAME command requires only one nickname parameter\r\n";
    send(client_fd, errorMessage.c_str(), errorMessage.length(), 0);
    return false;
}

bool Client::checkUsername(std::vector<std::string> tokens, int client_fd) const
{
    std::string errorMessage;

    // IRC USER command format: USER <user> <mode> <unused> :<realname>
    // After split(), this becomes at least 4 tokens (USER, user, mode, unused)
    if (tokens.size() == 5)
        return true;
    else
        errorMessage = "ERROR :USERNAME command requires username parameter\r\n";
    send(client_fd, errorMessage.c_str(), errorMessage.length(), 0);
    return false;
}

bool Client::checkPassword(std::vector<std::string> tokens, Client *client, std::string password) const
{
    std::string errorMessage;

    if (tokens.size() == 2)
    {
        if (tokens[1] != password)
            errorMessage = "ERROR :Invalid password\r\n";
        else
            return true;
    }
    else if (tokens.size() == 1)
        errorMessage = "ERROR :PASSWORD command requires a password parameter\r\n";
    else
        errorMessage = "ERROR :PASSWORD command has too many parameters\r\n";
    send(client->getFd(), errorMessage.c_str(), errorMessage.length(), 0);
    return false;
}
