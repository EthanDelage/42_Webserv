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
#include "config/VirtualServerConfig.hpp"
#include "config/LocationConfig.hpp"
#include <cstdlib>
#include <limits>
#include <cmath>
#include <iostream>
#include "utils.hpp"

#ifdef UNIT_TESTING
VirtualServerConfig::VirtualServerConfig() : Config() {
	_socketAddress.first = DEFAULT_ADDRESS;
	_socketAddress.second = DEFAULT_PORT;
}
#endif

VirtualServerConfig::VirtualServerConfig(Config const & config) : Config(config) {
	_socketAddress.first = DEFAULT_ADDRESS;
	_socketAddress.second = DEFAULT_PORT;
}

VirtualServerConfig::VirtualServerConfig(VirtualServerConfig const & other) : Config() {
	_index = other._index;
	_isDefaultIndex = true;
	_root = other._root;
	_errorPage = other._errorPage;
	_maxBodySize = other._maxBodySize;
	_autoindex = other._autoindex;
	_serverNames = other._serverNames;
	_socketAddress = other._socketAddress;
	_locationConfig.clear();
	_types = other._types;
}

VirtualServerConfig::~VirtualServerConfig() {
	for (std::vector<LocationConfig*>::iterator it = _locationConfig.begin(); it != _locationConfig.end(); ++it) {
		delete *it;
	}
}

std::string						VirtualServerConfig::getIp() const {return (_socketAddress.first);}

uint16_t						VirtualServerConfig::getPort() const {return (_socketAddress.second);}

socketAddress_t					VirtualServerConfig::getSocketAddress() const {return (socketAddress_t(getIp(), getPort()));}

std::vector<LocationConfig *>	VirtualServerConfig::getLocationConfig() const {return (_locationConfig);}

std::vector<std::string>		VirtualServerConfig::getServerNames() const {return (_serverNames);}

void VirtualServerConfig::parse(std::ifstream& configFile) {
	std::string	line;

	while (!configFile.eof())
	{
		std::getline(configFile, line);
		if (!line.empty()) {
			if (line == "}") {
				if (_locationConfig.empty())
					addDefaultLocation();
				return ;
			}
			removeHorizontalTabAndSpace(line);
			parseLine(line, configFile);
		}
	}
	throw (std::runtime_error("Missing `}` to close the server"));
}

void VirtualServerConfig::parseLine(std::string& line, std::ifstream& configFile) {
	std::string	directive;
	std::string	value;

	if (line.find("location") == 0) {
		parseLocation(configFile, line);
		return;
	}
	lineLexer(line, directive, value);
	try {
		router(directive, value);
	}
	catch (std::exception const & e) {
		throw (std::runtime_error("Invalid format: `" + line + "`\n"
			+ e.what()));
	}
}

void VirtualServerConfig::router(std::string& directive, std::string& value) {
	std::string	directives[] = {
		"autoindex",
		"client_max_body_size",
		"error_page",
		"index",
		"listen",
		"root",
		"server_name",
		"type"
	};
	parseFunctionType	parseFunction[] = {
		&VirtualServerConfig::parseAutoindex,
		&VirtualServerConfig::parseMaxBodySize,
		&VirtualServerConfig::parseErrorPage,
		&VirtualServerConfig::parseIndex,
		&VirtualServerConfig::parseListen,
		&VirtualServerConfig::parseRoot,
		&VirtualServerConfig::parseServerName,
		&VirtualServerConfig::parseType
	};

	for (size_t i = 0; i < (sizeof(directives) / sizeof(*directives)); i++) {
		if (directives[i] == directive) {
			(this->*parseFunction[i])(value);
			return;
		}
	}
	throw (std::runtime_error("Unknown command\n"));
}

void VirtualServerConfig::parseListen(std::string& value) {
	size_t						separator;
	std::vector<std::string>	argv;

	argv = split(value, SYNTAX_LISTEN);
	if (argv.size() != 1)
		throw (std::runtime_error(SYNTAX_LISTEN));
	separator = value.find(':');
	if (separator != std::string::npos)
	{
		_socketAddress.first = value.substr(0, separator);
		value.erase(0,separator + 1);
		if (value.empty() || !isValidIP(_socketAddress.first))
			throw (std::runtime_error(SYNTAX_LISTEN));
		_socketAddress.second = getPort(value);
	} else if (isValidIP(value)) {
		_socketAddress.first = value;
	} else {
		_socketAddress.second = getPort(value);
	}
}

void VirtualServerConfig::parseServerName(std::string& value) {
	std::vector<std::string>			argv;
	std::vector<std::string>::iterator	it;

	argv = Config::split(value, SYNTAX_SERVER_NAME);
	for (it = argv.begin(); it != argv.end(); it++) {
		_serverNames.push_back(toLower(*it));
	}
}

void VirtualServerConfig::parseLocation(std::ifstream& configFile, std::string& line) {
	LocationConfig*				locationConfig;
	std::vector<std::string>	argv;

	argv = split(line, SYNTAX_LOCATION);
	if (argv.size() != 3 && argv[3] != "}")
		throw (std::runtime_error(SYNTAX_LOCATION));
	locationConfig = new LocationConfig(*this, argv[1]);
	try {
		locationConfig->parse(configFile);
		locationConfig->print();
		if (isDuplicate(locationConfig))
			throw (std::runtime_error("Location Uri duplicated `" + locationConfig->getUri() + '\''));
		_locationConfig.push_back(locationConfig);
	} catch (std::runtime_error const & e) {
		delete locationConfig;
		throw (e);
	}
}

void VirtualServerConfig::addDefaultLocation() {
	LocationConfig*				locationConfig;
	std::string					uri;

	uri = "/";
	locationConfig = new LocationConfig(*this, uri);
	_locationConfig.push_back(locationConfig);
}

bool VirtualServerConfig::isDuplicate(LocationConfig const * locationConfig) {
	for (std::vector<LocationConfig*>::const_iterator it = _locationConfig.begin(); it != _locationConfig.end(); ++it) {
		if ((*it)->getUri() == locationConfig->getUri())
			return (true);
	}
	return (false);
}

uint16_t VirtualServerConfig::getPort(std::string const & str) {
	double	conversion;
	char*	rest;

	conversion = std::strtod(str.c_str(), &rest);
	if (*rest != '\0'
		|| conversion != round(conversion)
		|| conversion > std::numeric_limits<uint16_t>::max()
		|| conversion < 0)
		throw (std::runtime_error(SYNTAX_PORT));
	return (static_cast<uint16_t>(conversion));
}

void VirtualServerConfig::removeHorizontalTabAndSpace(std::string& line) {
	size_t	index = 0;

	while (line[index] == '\t' || line[index] == ' ')
		++index;
	line.erase(0, index);
}

#include <iostream>
void VirtualServerConfig::print() {
	std::cout << "VIRTUAL SERVER" << std::endl;
	Config::print();
	std::cout << "Port: " << _socketAddress.second << std::endl;
	std::cout << "Address: " << _socketAddress.first << std::endl;
	std::cout << "Server Names: ";
	for (std::vector<std::string>::iterator i = _serverNames.begin(); i !=  _serverNames.end(); i++)
		std::cout << *i << " | ";
	std::cout << std::endl;
}
