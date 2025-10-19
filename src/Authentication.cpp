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

bool Client::authenticator(std::string line, Client* client, std::string password, int client_fd) const {
    std::string errorMessage;
    std::vector<std::string> tokens = Server::split(line);
    std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::toupper);

    if (tokens[0] == "QUIT" && tokens.size() == 1)
        //
    else if (tokens[0] == "NICKNAME" && client->checkNickname(tokens, client_fd))
        return (client->setNickname(tokens[1]), true);
    else if (tokens[0] == "USERNAME" && client->checkUsername(tokens, client_fd))
        return (client->setUsername(tokens[1]), true);
    else if (tokens[0] == "PASSWORD" && client->checkPassword(tokens, client, password))
        return (client->setAuthenticated(true), true);
    else if (client->getNickname().empty() && client->getUsername().empty() && !client->isAuthenticated())
        errorMessage = "ERROR :You must set NICKNAME, USERNAME and authenticate with PASSWORD before sending commands\r\n";
    else if (tokens[0] == "KICK")
        //
    else if (tokens[0] == "INVITE")
        //
    else if (tokens[0] == "TOPIC")
        //
    else if (tokens[0] == "MODE")
        //

    send(client_fd, errorMessage.c_str(), errorMessage.length(), 0);
    return false;
}

bool Client::checkNickname(std::vector<std::string> tokens, int client_fd) const {
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

bool Client::checkUsername(std::vector<std::string> tokens, int client_fd) const {
    std::string errorMessage;

    if (tokens.size() == 2)
        return true;
    else if (tokens.size() < 2)
        errorMessage = "ERROR :USERNAME command requires a username parameter\r\n";
    else
        errorMessage = "ERROR :USERNAME command has too many parameters\r\n";
    send(client_fd, errorMessage.c_str(), errorMessage.length(), 0);
    return false;
}

bool Client::checkPassword(std::vector<std::string> tokens, Client* client, std::string password) const {
    std::string errorMessage;

    if (tokens.size() == 2) {
        if (tokens[1] != password)
            errorMessage = "ERROR :Invalid password\r\n";
        else
            return true;
    } else if (tokens.size() == 1)
        errorMessage = "ERROR :PASSWORD command requires a password parameter\r\n";
    else
        errorMessage = "ERROR :PASSWORD command has too many parameters\r\n";
    send(client->getFd(), errorMessage.c_str(), errorMessage.length(), 0);
    return false;
}

