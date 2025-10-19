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

typedef std::string string;

class OperatorCommands {
    public:
        void Kick(std::vector<string> tokens, int client_fd);
        void Invite(std::vector<string> tokens, int client_fd);
        void Topic(std::vector<string> tokens, int client_fd);
        void Mode(std::vector<string> tokens, int client_fd);
        void Quit(std::vector<string> tokens, int client_fd);
};

#endif