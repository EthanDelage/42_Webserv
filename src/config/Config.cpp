/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ethan <ethan@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 16:43:00 by ethan             #+#    #+#             */
/*   Updated: 2023/07/07 16:43:00 by ethan            ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include "Config.hpp"

#include <iostream>
#include <fstream>

Config::Config(char const * configFilename) {
	std::ifstream		configFile(configFilename);
	if (!configFile.is_open())
		throw std::runtime_error("Cannot open file " + std::string(configFilename));

	VirtualServerConfig	*defaultServerConfig = new VirtualServerConfig(configFile);

	_serverConfig.push_back(defaultServerConfig);
	configFile.close();
}

Config::~Config() {
	for (std::vector<VirtualServerConfig *>::iterator it = _serverConfig.begin(); it != _serverConfig.end(); ++it) {
		delete *it;
	}
}
