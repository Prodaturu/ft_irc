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

void OperatorCommands::Kick(stringList tokens, Client* client) { 
    if (!client->isAuthenticated())
        return;
    (void)tokens;
}

void OperatorCommands::Invite(stringList tokens, Client* client) { (void)tokens; (void)client; }

void OperatorCommands::Topic(stringList tokens, Client* client) { (void)tokens; (void)client; }

void OperatorCommands::Mode(stringList tokens, Client* client) { (void)tokens; (void)client; }

void OperatorCommands::Quit(stringList tokens, Client* client) { (void)tokens; (void)client; }