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

void Config::parse(char* configFilename) {
	std::ifstream	configFile(configFilename);
	std::string		line;

	if (!configFile.is_open())
		throw (std::runtime_error(std::string("Couldn't open file: ") + configFilename));
	while (!configFile.eof()) {
		std::getline(configFile, line);
		parseLine(line, configFile);
	}
	configFile.close();
}

typedef void (Config::*parseFunctionType)(std::string&);

void Config::parseLine(std::string& line, std::ifstream& configFile) {
	std::string 		directives[] = {"autoindex",
		"client_max_body_size", "error_page", "index", "root", ""};
	parseFunctionType	parseFunction[] = {&Config::parseAutoindex,
		&Config::parseMaxBodySize, &Config::parseErrorPage,
		&Config::parseIndex, &Config::parseRoot};

	for (int i = 0; !directives[i].empty(); ++i) {
		if (line.compare(0, directives[i].size(), directives[i]) == 0) {
			(this->*parseFunction[i])(line);
			return ;
		}
	}
	if (line == "server {")
		parseServer(configFile);
	else
		throw std::runtime_error("Unknown command: `" + line + '`');
}

void Config::parseAutoindex(std::string& line) {
	(void) line;
}

void Config::parseMaxBodySize(std::string &line) {
	(void) line;
}

void Config::parseErrorPage(std::string &line) {
	(void) line;
}

void Config::parseIndex(std::string &line) {
	(void) line;
}

void Config::parseRoot(std::string &line) {
	(void) line;
}

void Config::parseServer(std::ifstream &configFile) {
	(void) configFile;
}
