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
#include <Config/Config.hpp>
#include "VirtualServerConfig.hpp"

#include <fstream>

Config::Config() {
	_index.push_back(DEFAULT_INDEX);
	_root = DEFAULT_ROOT;
	_errorPage[404] = "404.html";
	_maxBodySize = DEFAULT_MAX_BODY_SIZE;
	_autoindex = DEFAULT_AUTOINDEX;
}

Config::Config(Config const & other) {
	_index = other._index;
	_root = other._root;
	_errorPage = other._errorPage;
	_maxBodySize = other._maxBodySize;
	_autoindex = other._autoindex;
}

void Config::parse(std::string& configFilename) {
	std::ifstream	configFile(configFilename);
	std::string		line;

	if (!configFile.is_open())
		throw (std::runtime_error("Couldn't open file: " + configFilename));
	while (!configFile.eof()) {
		std::getline(configFile, line);
		parseLine(line, configFile);
	}
}

void Config::parseLine(std::string& line, std::ifstream& configFile) {
	std::string directives[] = {"autoindex", "client_max_body_size", "error_page", "index", "root", "server"};
	
}

void Config::parseAutoindex(std::string& line) {

}
