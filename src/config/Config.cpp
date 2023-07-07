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

Config::Config(std::string const & configFile) {
	virtualServerConfig_t	defaultConfig;

	(void) configFile;
	defaultConfig.isDefault = true;
	defaultConfig.serverNames.push_back("www.test.com");
	defaultConfig.address = "127.0.0.1";
	defaultConfig.port = 80;
	defaultConfig.errorPage[404] = "404.html";
	defaultConfig.maxBodySize = 1024;

	_serverConfig.push_back(defaultConfig);
}

Config::~Config() {}
