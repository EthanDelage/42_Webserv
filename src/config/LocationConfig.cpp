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
#include "LocationConfig.hpp"

LocationConfig::LocationConfig(VirtualServerConfig const & virtualServerConfig)
	: VirtualServerConfig(virtualServerConfig) {
	_allowedHttpMethod = 0b00000111;
}

bool LocationConfig::getMethodStatus() const {
	return (_allowedHttpMethod & GET_METHOD_MASK);
}

bool LocationConfig::postMethodStatus() const {
	return (_allowedHttpMethod & POST_METHOD_MASK);
}

bool LocationConfig::deleteMethodStatus() const {
	return (_allowedHttpMethod & DELETE_METHOD_MASK);
}

void LocationConfig::parse(std::ifstream& configFile) {
	std::string	line;

	while (!configFile.eof())
	{
		std::getline(configFile, line);
		if (!line.empty())
			parseLine(line);
	}
}

void LocationConfig::parseLine(std::string& line) {
	std::string	directive;
	std::string	value;

	lineLexer(line, directive, value);

}

void LocationConfig::router(std::string& directive, std::string& value) {
	std::string	directives[] = {
			"autoindex",
			"deny",
			"error_page",
			"index",
			"root",
	};
	parseFunctionType	parseFunction[] = {
			&LocationConfig::parseAutoindex,
			&LocationConfig::parseDeny,
			&LocationConfig::parseErrorPage,
			&LocationConfig::parseIndex,
			&LocationConfig::parseRoot,
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
	(void)value;
}
