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

void Server::authenticator(string line, Client *client, int client_fd) const
{
    string errorMessage = 
        "ERROR :You must authenticate first\r\n"
        "AUTHENTICATE USING:\r\n"
        "PASS <password>\r\n"
        "NICK <nickname>\r\n"
        "USER <username> <hostname> <servername> :<realname>\r\n";
    stringList tokens = parser(line);
    if (tokens.empty())
        return ;

    if (tokens[0] == "PASS" && checkPassword(tokens, client_fd))
        client->setAuthenticated(true);
    else if (tokens[0] == "NICK" && checkNickname(tokens, client_fd))
        client->setNickname(tokens[1]);
    else if (tokens[0] == "USER" && checkUsername(tokens, client_fd))
    {
        client->setUsername(tokens[1]);
        client->setRealname(tokens[4]);
    }
    else if (tokens[0] == "QUIT")
        OperatorCommands().Quit(tokens, client);
    else if (tokens[0] == "KICK") 
        OperatorCommands().Kick(tokens, client);
    else if (tokens[0] == "INVITE")
        OperatorCommands().Invite(tokens, client);
    else if (tokens[0] == "TOPIC")
        OperatorCommands().Topic(tokens, client);
    else if (tokens[0] == "MODE")
        OperatorCommands().Mode(tokens, client);

    if (!errorMessage.empty())
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
