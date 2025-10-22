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

void Server::authenticator(string line, Client *client, int client_fd)
{
    stringList tokens = parser(line);
    if (tokens.empty())
        return ;

    if (tokens[0] == "PASS" && checkPassword(tokens, client_fd))
        return (void)client->setAuthenticated(true);
    else if (tokens[0] == "NICK" && checkNickname(tokens, client_fd))
        return (void)client->setNickname(tokens[1]);
    else if (tokens[0] == "USER" && checkUsername(tokens, client_fd))
    {
        client->setUsername(tokens[1]);
        client->setRealname(tokens[4]);
        return ;
    }
    if (!client->isAuthenticated() || client->getNickname().empty() || client->getUsername().empty())
        send(client_fd, errorMessage.c_str(), errorMessage.length(), 0);
}

bool Server::checkNickname(stringList tokens, int client_fd) const
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

bool Server::checkUsername(stringList tokens, int client_fd) const
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

bool Server::checkPassword(stringList tokens, int client_fd) const
{
    std::string errorMessage;

    if (tokens.size() == 2)
    {
        if (tokens[1] != _password)
            errorMessage = "ERROR :Invalid password\r\n";
        else
            return true;
    }
    else if (tokens.size() == 1)
        errorMessage = "ERROR :PASSWORD command requires a password parameter\r\n";
    else
        errorMessage = "ERROR :PASSWORD command has too many parameters\r\n";
    send(client_fd, errorMessage.c_str(), errorMessage.length(), 0);
    return false;
}
