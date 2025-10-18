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

#include "OperatorCommands.hpp"

OperatorCommands::OperatorCommands() {
}

OperatorCommands::~OperatorCommands() {
}

OperatorCommands::OperatorCommands(const OperatorCommands &toCopy) {
    *this = toCopy;
}

OperatorCommands &OperatorCommands::operator=(const OperatorCommands &toCopy) {
    if (this != &toCopy) {
    }
    return *this;
}