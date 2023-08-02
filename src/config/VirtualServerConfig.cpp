/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServerConfig.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ethan <ethan@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/09 02:30:00 by ethan             #+#    #+#             */
/*   Updated: 2023/07/09 02:30:00 by ethan            ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include "VirtualServerConfig.hpp"
#include "LocationConfig.hpp"

VirtualServerConfig::VirtualServerConfig(Config const & config) : Config(config) {
	_isDefault = true;
	_address = DEFAULT_ADDRESS;
	_port = DEFAULT_PORT;
}

VirtualServerConfig::VirtualServerConfig(VirtualServerConfig const & other) : Config() {
	_index = other._index;
	_isDefaultIndex = other._isDefaultIndex;
	_root = other._root;
	_errorPage = other._errorPage;
	_maxBodySize = other._maxBodySize;
	_autoindex = other._autoindex;
	_isDefault = other._isDefault;
	_serverNames = other._serverNames;
	_address = other._address;
	_port = other._port;
	_locationConfig = other._locationConfig;
}
