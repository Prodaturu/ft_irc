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

typedef std::string string;

class OperatorCommands {
    // OperatorCommands class definition will go here

    public:
        OperatorCommands();
        ~OperatorCommands();
        OperatorCommands(const OperatorCommands &toCopy);
        OperatorCommands &operator=(const OperatorCommands &toCopy);
};

#endif