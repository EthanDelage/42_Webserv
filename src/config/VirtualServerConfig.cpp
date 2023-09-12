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
	_isDefaultIndex = other._isDefaultIndex;
	_root = other._root;
	_errorPage = other._errorPage;
	_maxBodySize = other._maxBodySize;
	_autoindex = other._autoindex;
	_serverNames = other._serverNames;
	_socketAddress = other._socketAddress;
	_locationConfig = other._locationConfig;
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
			if (line == "}")
				return ;
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
		"server_name"
	};
	parseFunctionType	parseFunction[] = {
		&VirtualServerConfig::parseAutoindex,
		&VirtualServerConfig::parseMaxBodySize,
		&VirtualServerConfig::parseErrorPage,
		&VirtualServerConfig::parseIndex,
		&VirtualServerConfig::parseListen,
		&VirtualServerConfig::parseRoot,
		&VirtualServerConfig::parseServerName,
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
	locationConfig->parse(configFile);
	locationConfig->print();
	_locationConfig.push_back(locationConfig);
}

bool	VirtualServerConfig::isValidIP(std::string const & str) {
	size_t	index = 0;

	if (!isValidIpByte(str, index))
		return (false);
	for (int i = 0; i < 3; ++i) {
		if (str[index] != '.')
			return (false);
		index++;
		if (!isValidIpByte(str, index))
			return (false);
	}
	if (str[index])
		return (false);
	return (true);
}

bool VirtualServerConfig::isValidIpByte(std::string const & address, size_t& index) {
	uint8_t	byte = 0;

	if (!std::isdigit(address[index]))
		return (false);
	for (int i = 0; i < 3 && std::isdigit(address[index]); ++i) {
		if (((uint8_t) (byte * 10 + (address[index] - '0'))) / 10 != byte)
			return (false);
		byte = byte * 10 + (address[index] - '0');
		++index;
	}
	if (std::isdigit(address[index]))
		return (false);
	return (true);
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

std::string VirtualServerConfig::toLower(std::string const & str) {
	std::string result;

	for (size_t i = 0; i < str.size(); ++i)
		result[i] = static_cast<char>(tolower(str[i]));
	return (result);
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
