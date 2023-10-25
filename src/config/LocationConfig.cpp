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
#include <sstream>
#include <iostream>

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
		_redirectionUri = "";
}

std::string LocationConfig::getUri() const {return (_uri);}

std::vector<std::string> LocationConfig::getUriDirectories() const {return (_uriDirectories);}

uint8_t LocationConfig::getAllowedHttpMethod() const {return (_allowedHttpMethod);}

std::string LocationConfig::getRedirectionUri() const {return (_redirectionUri);}

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
			"cgi",
			"deny",
			"error_page",
			"index",
			"return",
			"root",
			"type"
	};
	parseFunctionType	parseFunction[] = {
			&LocationConfig::parseAutoindex,
			&LocationConfig::parseCgi,
			&LocationConfig::parseDeny,
			&LocationConfig::parseErrorPage,
			&LocationConfig::parseIndex,
			&LocationConfig::parseReturn,
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

void LocationConfig::parseReturn(std::string& value) {
	std::vector<std::string>	args;

	args = split(value, SYNTAX_ROOT);
	if (args.size() != 1)
		throw (std::runtime_error(SYNTAX_ROOT));
	_redirectionUri = removeQuote(args[0]);
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

void LocationConfig::print() const {
	std::cout << "LOCATION" << std::endl;
	VirtualServerConfig::print();
	std::cout << "Uri:    " << _uri << std::endl;
	std::cout << "Get:    " << (getMethodStatus() ? "Allowed": "Denied") << std::endl;
	std::cout << "Post:   " << (postMethodStatus() ? "Allowed": "Denied") << std::endl;
	std::cout << "Delete: " << (deleteMethodStatus() ? "Allowed": "Denied") << std::endl;
}

void LocationConfig::printResponseConfig(int clientSocket) const {
	std::stringstream			ss;
	std::vector<std::string>	serverNames;
	socketAddress_t				socketAddress;

	serverNames = getServerNames();
	socketAddress = getSocketAddress();
	printColor(std::cout, "Config for response sent to client ", PURPLE);
	ss << clientSocket;
	printColor(std::cout, ss.str(), DEFAULT);
	printColor(std::cout, " ↴\n", PURPLE);
	ss.str("");
	ss << "server {" << std::endl
		<< "\tlisten: " << socketAddress.first << ':' << socketAddress.second << ';' << std::endl;
	if (!serverNames.empty()) {
		ss << "\tserver_name:";
		for (std::vector<std::string>::const_iterator it = serverNames.begin(); it != serverNames.end(); ++it)
			ss << ' ' + *it;
		ss << ';' << std::endl;
	}
	ss << "\tclient_max_body_size: " << getMaxBodySize() << "b;" << std::endl;
	ss << "\tlocation " << _uri << " {" << std::endl;
	if (_autoindex)
		ss << "\t\tautoindex: on;" << std::endl;
	ss << "\t\tcgi: " << _cgiFolder;
	for (std::vector<std::string>::const_iterator it = _cgi.begin(); it != _cgi.end(); ++it)
		ss << ' ' + *it;
	ss << ';' << std::endl;
	if (_allowedHttpMethod != DEFAULT_METHOD_MASK) {
		ss << "\t\tdeny:";
		if (!getMethodStatus())
			ss << " GET";
		if (!deleteMethodStatus())
			ss << " DELETE";
		if (!postMethodStatus())
			ss << " POST";
		ss << ';' << std::endl;
	}
	if (_redirectionUri.empty()) {
		ss << "\t\troot: " << _root << ';'<< std::endl;
		ss << "\t\tindex:";
		for (std::vector<std::string>::const_iterator it = _index.begin(); it != _index.end(); ++it)
			ss << ' ' + *it;
		ss << ';' << std::endl;
	} else {
		ss << "\t\treturn: " << _redirectionUri << ';' << std::endl;
	}
	ss << "\t}" << std::endl
		<< '}' << std::endl;
	printColor(std::cout, ss.str(), DEFAULT);
}
