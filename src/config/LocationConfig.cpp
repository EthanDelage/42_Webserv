/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ethan <ethan@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/09 02:34:00 by ethan             #+#    #+#             */
/*   Updated: 2023/07/09 02:34:00 by ethan            ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include "config/LocationConfig.hpp"
#include "utils.hpp"

#ifdef UNIT_TESTING
	LocationConfig::LocationConfig() : _allowedHttpMethod(DEFAULT_METHOD_MASK) {}
#endif

LocationConfig::LocationConfig(VirtualServerConfig const & virtualServerConfig, std::string& uri) :
	VirtualServerConfig(virtualServerConfig),
	_uri(uri) {
		if (uri[0] != '/')
			throw (std::runtime_error("Bad URI"));
		if (uri[uri.size() - 1] != '/')
			uri.erase(uri.find('/') + 1);
		_uriDirectories = split_path(uri);
		_allowedHttpMethod = DEFAULT_METHOD_MASK;
}

std::string LocationConfig::getUri() const {return (_uri);}

std::vector<std::string> LocationConfig::getUriDirectories() const {return (_uriDirectories);}

uint8_t LocationConfig::getAllowedHttpMethod() const {return (_allowedHttpMethod);}

bool LocationConfig::getMethodStatus() const {
	return (_allowedHttpMethod & GET_METHOD_MASK);
}

bool LocationConfig::getMethodStatus(uint8_t methodMask) {
	return (methodMask & GET_METHOD_MASK);
}

bool LocationConfig::postMethodStatus() const {
	return (_allowedHttpMethod & POST_METHOD_MASK);
}

bool LocationConfig::postMethodStatus(uint8_t methodMask) {
	return (methodMask & POST_METHOD_MASK);
}

bool LocationConfig::deleteMethodStatus() const {
	return (_allowedHttpMethod & DELETE_METHOD_MASK);
}

bool LocationConfig::deleteMethodStatus(uint8_t methodMask) {
	return (methodMask & DELETE_METHOD_MASK);
}

void LocationConfig::parse(std::ifstream& configFile) {
	std::string	line;

	while (!configFile.eof())
	{
		std::getline(configFile, line);
		if (!line.empty()) {
			removeHorizontalTabAndSpace(line);
			if (line == "}")
				return ;
			parseLine(line);
		}
	}
}

void LocationConfig::parseLine(std::string& line) {
	std::string	directive;
	std::string	value;

	lineLexer(line, directive, value);
	try {
		router(directive, value);
	}
	catch (std::exception const & e) {
		throw (std::runtime_error("Invalid format: `" + line + "`\n"
			+ e.what()));
	}
}

void LocationConfig::router(std::string& directive, std::string& value) {
	std::string	directives[] = {
			"autoindex",
			"deny",
			"error_page",
			"index",
			"root",
			"type",
	};
	parseFunctionType	parseFunction[] = {
			&LocationConfig::parseAutoindex,
			&LocationConfig::parseDeny,
			&LocationConfig::parseErrorPage,
			&LocationConfig::parseIndex,
			&LocationConfig::parseRoot,
			&LocationConfig::parseType,
	};

	for (size_t i = 0; i < (sizeof(directives) / sizeof(*directives)); i++) {
		if (directives[i] == directive) {
			(this->*parseFunction[i])(value);
			return;
		}
	}
	throw (std::runtime_error("Unknown command\n"));
}

void LocationConfig::parseDeny(std::string& value) {
	if (value == "GET")
		_allowedHttpMethod &= (0b11111111 - GET_METHOD_MASK);
	else if (value == "POST")
		_allowedHttpMethod &= (0b11111111 - POST_METHOD_MASK);
	else if (value == "DELETE")
		_allowedHttpMethod &= (0b11111111 - DELETE_METHOD_MASK);
	else
		throw (std::runtime_error(SYNTAX_DENY));
}

std::string LocationConfig::allowedHttpMethodToString(uint8_t methodMask) {
	std::vector<std::string>	allowedMethod;
	std::string					result;

	if (getMethodStatus(methodMask))
		allowedMethod.push_back("GET");
	if (postMethodStatus(methodMask))
		allowedMethod.push_back("POST");
	if (deleteMethodStatus(methodMask))
		allowedMethod.push_back("DELETE");
	if (allowedMethod.empty())
		return ("");
	else if (allowedMethod.size() == 1)
		return (allowedMethod[0]);
	for (std::vector<std::string>::iterator it = allowedMethod.begin(); it != (allowedMethod.end() - 1); ++it) {
		result += *it;
		result += ", ";
	}
	result += allowedMethod[allowedMethod.size() - 1];
	return (result);
}

#include <iostream>
void LocationConfig::print() {
	std::cout << "LOCATION" << std::endl;
	VirtualServerConfig::print();
	std::cout << "Uri:    " << _uri << std::endl;
	std::cout << "Get:    " << (getMethodStatus() ? "Allowed": "Denied") << std::endl;
	std::cout << "Post:   " << (postMethodStatus() ? "Allowed": "Denied") << std::endl;
	std::cout << "Delete: " << (deleteMethodStatus() ? "Allowed": "Denied") << std::endl;
}
