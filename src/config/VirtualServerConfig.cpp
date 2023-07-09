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

VirtualServerConfig::VirtualServerConfig(std::ifstream& configFile) {
	(void) configFile;

	_isDefault = true;
	_serverNames.push_back("www.test.com");
	_address = "127.0.0.1";
	_port = 80;
	_index.push_back("index.html");
	_root = "undefined";
	_errorPage[404] = "404.html";
	_maxBodySize = -1;
	_autoindex = false;
}

VirtualServerConfig::~VirtualServerConfig() {}
