/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelModes.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohrahma <mohrahma@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:37:09 by mohrahma          #+#    #+#             */
/*   Updated: 2025/10/18 16:37:09 by mohrahma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ChannelModes.hpp"

ChannelModes::ChannelModes() {
}

ChannelModes::~ChannelModes() {
}

ChannelModes::ChannelModes(const ChannelModes &toCopy) {
    *this = toCopy;
}

ChannelModes &ChannelModes::operator=(const ChannelModes &toCopy) {
    if (this != &toCopy) {
    }
    return *this;
}