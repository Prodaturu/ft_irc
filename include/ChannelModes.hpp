/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelModes.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohrahma <mohrahma@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:37:01 by mohrahma          #+#    #+#             */
/*   Updated: 2025/10/18 16:37:01 by mohrahma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNELMODES_HPP
#define CHANNELMODES_HPP

#include <string>

typedef std::string string;

class ChannelModes {
    // ChannelModes class definition will go here

    public:
        ChannelModes();
        ~ChannelModes();
        ChannelModes(const ChannelModes &toCopy);
        ChannelModes &operator=(const ChannelModes &toCopy);
};

#endif