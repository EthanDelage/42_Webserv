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
	_root = std::string(PREFIX) + DEFAULT_ROOT;
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
		if (!line.empty())
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
		if (line.compare(0, directives[i].size(), directives[i]) == 0
			&& line[directives[i].size()] == ' ') {
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
	if (line == "autoindex on;")
		_autoindex = true;
	else if (line == "autoindex off;")
		_autoindex = false;
	else
		throw (std::runtime_error("Invalid format: `" + line + "`\n"
			+ "Syntax: \"autoindex\" SP \"on\" | \"off\" \";\""));
}

void Config::parseMaxBodySize(std::string &line) {
	std::string	value = line;
	ssize_t		convertValue;

	value.erase(0, strlen("client_max_body_size "));
	if (line.back() != ';' || value.size() == 1)
		throw (std::runtime_error("Invalid format: `" + line + "`\n"
			+ "Syntax: \"client_max_body_size\" SP size \";\""));
	value.pop_back();
	convertValue = parseSize(value);
	if (convertValue == -1)
		throw (std::runtime_error("Invalid size: `" + line + "`\n"
			+ "Syntax: 1*DIGIT [ \"k\" | \"m\" ]"));
	_maxBodySize = convertValue;
}

void Config::parseErrorPage(std::string &line) {
	(void) line;
}

void Config::parseIndex(std::string &line) {
	(void) line;
}

void Config::parseRoot(std::string &line) {
	std::string	value = line;

	value.erase(0, strlen("root "));
	if (line.back() != ';' || value.size() == 1)
		throw (std::runtime_error("Invalid format: `" + line + "`\n"
			+ "Syntax: \"root\" SP path \";\""));
	value.pop_back();
	_root = parsePath(value);
}

void Config::parseServer(std::ifstream &configFile) {
	(void) configFile;
}

/**
 * @brief takes the path and removes the unwanted char (", ').
 *  Also handles \" and \'
 */
std::string Config::parsePath(std::string &value) {
	std::string	path;
	char		quote;

	for (size_t i = 0; i < value.size(); ++i) {
		if ((value[i] == '"' || value[i] == '\'')
			&& (i == 0 || value[i - 1] != '\\')) {
			quote = value[i];
			++i;
			while (i < value.size() && value[i] != quote) {
				path += value[i];
				++i;
			}
		}
		else if ((value[i] == '"' || value[i] == '\'') && i > 0
			&& value[i -1] == '\\')
			path.back() = value[i];
		else
			path += value[i];
	}
	if (path[0] != '/')
		path = PREFIX + path;
	return (path);
}

/**
 * @brief converts a string in 'size' format to ssize_t
 * @return returns converted value or -1 on error
 */
ssize_t Config::parseSize(std::string &value) {
	char*	rest;
	double	conversion = std::strtod(value.c_str(), &rest);
	ssize_t	result = static_cast<ssize_t>(conversion);

	if (!isdigit(value[0])
		|| conversion != round(conversion)
		|| value.find('.') != std::string::npos)
		return (-1);
	if (*rest == '\0')
		return (result);
	else if (*rest == 'k' && *(++rest) == '\0')
		return (result * 1 << 10);
	else if (*rest == 'm' && *(++rest) == '\0')
		return (result * 1 << 20);
	else
		return (-1);
}
