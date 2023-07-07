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

Config::Config(std::string& configFile) {
	virtualServerConfig_t	defaultConfig;

	(void) configFile;
	defaultConfig.serverNames.push_back("www.test.com");
	defaultConfig.address = "127.0.0.1";
	defaultConfig.port = 80;
	defaultConfig.isDefault = true;
	defaultConfig.defaultErrorPage = "404.html";

	_serverConfig.push_back(defaultConfig);
}

Config::~Config() {}
