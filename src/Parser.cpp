/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parse.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohrahma <mohrahma@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 17:12:43 by mohrahma          #+#    #+#             */
/*   Updated: 2025/10/22 17:12:43 by mohrahma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

// Tokenization rules (per RFC-1459 style)
stringList Server::parser(const string &input) const {
    string line = "";
    string token;
    stringList tokens;
    std::istringstream iss(input);

    if (input[0] == ':')
        iss.get(); // Remove leading ':'

    while (iss >> token)
    {
        if (token[0] == ':')
        {
            std::getline(iss, line);
            token.erase(0, 1);
            token += line;
            tokens.push_back(token);
            break;
        }
        tokens.push_back(token);
    }
    std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::toupper);
    return tokens;
}