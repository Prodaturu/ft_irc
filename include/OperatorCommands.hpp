/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   OperatorCommands.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohrahma <mohrahma@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:37:04 by mohrahma          #+#    #+#             */
/*   Updated: 2025/10/18 16:37:04 by mohrahma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OPERATORCOMMANDS_HPP
#define OPERATORCOMMANDS_HPP

#include <string>
#include <vector>

#include "Client.hpp"
#include "Server.hpp"

class Channel;

typedef std::string string;
typedef std::vector<string> stringList;

class OperatorCommands {
    public:
        void Kick(stringList tokens, Client* client, Channel* channel);
        void Invite(stringList tokens, Client* client);
        void Topic(stringList tokens, Client* client, Channel* channel);
        void Mode(stringList tokens, Client* client);
        void Quit(stringList tokens, Client* client, Server* server);
        void Join(stringList tokens, Client* client);
        
};

#endif