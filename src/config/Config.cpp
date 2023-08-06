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
#include <cstring>
#include <cstdlib>
#include <cmath>

Config::Config() {
	_index.push_back(DEFAULT_INDEX);
	_isDefaultIndex = true;
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

	value.erase(0, std::strlen("client_max_body_size "));
	if (*(line.end() - 1) != ';' || value.size() == 1)
		throw (std::runtime_error("Invalid format: `" + line + "`\n"
			+ "Syntax: \"client_max_body_size\" SP size \";\""));
	value.erase(value.end() - 1);
	convertValue = parseSize(value);
	if (convertValue == -1)
		throw (std::runtime_error("Invalid size: `" + line + "`\n"
			+ "Syntax: 1*DIGIT [ \"k\" | \"m\" ]"));
	_maxBodySize = convertValue;
}

void Config::parseErrorPage(std::string &line) {
	std::string value = line;
	std::string uri;

	value.erase(0, std::strlen("error_page "));
	if (*(line.end() - 1) != ';' || value.size() == 1)
		throw (std::runtime_error("Invalid format: `" + line + "`\n"
			+ "Syntax: \"error_page\" 1*( SP code ) SP uri \";\""));
    value.erase(value.end() - 1);
	try {
		uri = GetUriErrorPage(value);
		while (!value.empty()) {
			_errorPage[getNextCode(value)] = uri;
			if (value.size() > 3 && value[3] != ' ')
				throw (std::runtime_error("Syntax: \"error_page\" 1*( SP code ) SP uri \";\""));
			if (value.size() > 4)
				value.erase(0, 4);
			else
				value.erase(0, 3);
		}
	} catch (std::runtime_error const & e) {
		throw (std::runtime_error("Invalid format: `" + line + "`\n"
			+ e.what()));
	}
}

void Config::parseIndex(std::string &line) {
	std::string	value = line;
	std::string	file;

	value.erase(0, std::strlen("index "));
	if (*(line.end() - 1) != ';' || value.size() == 1)
		throw (std::runtime_error("Invalid format: `" + line + "`\n"
			+ "Syntax: \"index\" 1*( SP file ) \";\""));
    value.erase(value.end() - 1);
	if (_isDefaultIndex == true) {
		_isDefaultIndex = false;
		_index.clear();
	}
	while (!value.empty()) {
		file = getNextFile(value);
		if (file.empty())
			throw (std::runtime_error("Invalid format: `" + line + "`\n"
				+ "Files must be separated by a single space"));
		_index.push_back(file);
		value.erase(0, file.size());
		if (value.size() != 1)
			value.erase(0, 1);
	}
}

void Config::parseRoot(std::string &line) {
	std::string	value = line;

	value.erase(0, std::strlen("root "));
	if (*(line.end() - 1) != ';' || value.size() == 1)
		throw (std::runtime_error("Invalid format: `" + line + "`\n"
			+ "Syntax: \"root\" SP path \";\""));
    value.erase(value.end() - 1);
	try {
		_root = parsePath(value);
	} catch (std::runtime_error const & e) {
		throw (std::runtime_error("Invalid format: `" + line + "`\n"
			+ e.what()));
	}
}

void Config::parseServer(std::ifstream &configFile) {
	(void) configFile;
}

std::string Config::parsePath(std::string &value) {
	std::string	path;

	path = removeQuote(value);
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

	if (!std::isdigit(value[0])
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

/**
 * @brief takes a string and removes quote char (", ').
 *  Also handles \" and \'
 */
std::string Config::removeQuote(std::string &str) {
	std::string	result;
	char		quote;

	for (size_t i = 0; i < str.size(); ++i) {
		if ((str[i] == '"' || str[i] == '\'')
			&& (i == 0 || str[i - 1] != '\\')) {
			quote = str[i];
			++i;
			while (i < str.size() && (str[i] != quote
					|| (str[i] == quote && str[i - 1] == '\\'))) {
				if (str[i] == quote)
					*(result.end() - 1) = quote;
				else
					result += str[i];
				++i;
			}
			if (i == str.size())
				throw (std::runtime_error(std::string("Missing: `")
					+ quote + '`'));
		} else if ((str[i] == '"' || str[i] == '\'') && str[i -1] == '\\')
			*(result.end() - 1) = str[i];
		else
			result += str[i];
	}
	return (result);
}

/**
 * @brief get the next file while removing the quote
 */
std::string Config::getNextFile(std::string &value) {
	std::string	file;
	char		quote;

	for (size_t i = 0; i < value.size() && value[i] != ' '; ++i) {
		if ((value[i] == '"' || value[i] == '\'')
			&& (i == 0 || value[i - 1] != '\\')) {
			quote = value[i++];
			while (i < value.size() && (value[i] != quote
				|| (value[i] == quote && value[i - 1] == '\\'))) {
				if (value[i] == quote)
					*(file.end() - 1) = quote;
				else
					file += value[i];
				++i;
			}
		} else if ((value[i] == '"' || value[i] == '\'') && value[i - 1] == '\\')
			*(file.end() - 1) = value[i];
		else
			file += value[i];
	}
	return (file);
}

/**
 * @brief get the next code format: code = 3DIGIT
 */
uint16_t Config::getNextCode(std::string &value) {
	uint16_t	code = 0;

	if (value[0] == ' ')
		throw (std::runtime_error("Codes must be separate by single space"));
	for (int i = 0; i < 3; ++i) {
		if (!std::isdigit(value[i]))
			throw (std::runtime_error("Syntax: code = 3DIGIT"));
		code = code * 10 + (value[i] - '0');
	}
	if (code < 300 || code > 599)
		throw (std::runtime_error('`' + value.substr(0, 3)
			+ "`: code must be between 300 and 599"));
	return (code);
}

/**
 * @brief get the uri and remove it from value
 * @return uri
 */
std::string Config::GetUriErrorPage(std::string &value) {
	size_t		indexLastSpace = 0;
	char		quote = 0;
	std::string	uri;

	for (size_t i = 0; i != value.size(); ++i) {
		if (value[i] == ' ' && quote == 0)
			indexLastSpace = i;
		else if ((value[i] == '\'' || value[i] == '"')
			&& (i == 0 || value[i - 1] != '\\')
			&& quote == 0)
			quote = value[i];
		else if ((value[i] == '\'' || value[i] == '"')
			&& (i == 0 || value[i - 1] != '\\')
			&& quote == value[i])
			quote = 0;
	}
	if (indexLastSpace == 0)
		throw (std::runtime_error("Invalid number of arguments"));
	else if (indexLastSpace == value.size() - 1)
		throw (std::runtime_error("Space after uri"));
	uri = value.substr(indexLastSpace + 1);
	value.erase(indexLastSpace);
	return (removeQuote(uri));
}
