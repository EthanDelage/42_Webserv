/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 16:11:00 by edelage           #+#    #+#             */
/*   Updated: 2023/08/17 16:11:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include "message/Message.hpp"
#include "utils.hpp"
#include "error/Error.hpp"

Message::Message(int clientSocket) {
	_httpVersion.major = 0;
	_httpVersion.minor = 0;
	_clientSocket = clientSocket;
}

Message::~Message() {}

httpVersion_t	Message::getHttpVersion() const {return (_httpVersion);}

std::string Message::getBody() const {return (_body);}

Header Message::getHeader() const {return(_header);}

int Message::getClientSocket() const {return (_clientSocket);}

LocationConfig* Message::getLocation() const {return (_locationConfig);}

LocationConfig*	Message::getMessageLocation(const VirtualServerConfig &virtualServerConfig, std::string requestUri) {
	std::vector<LocationConfig*>	locationConfig;
	std::vector<std::string>		requestUriDirectories;

	locationConfig = virtualServerConfig.getLocationConfig();
	if (requestUri[requestUri.size() - 1] != '/')
		requestUri.erase(requestUri.rfind('/') + 1);
	requestUriDirectories = split_path(requestUri);
	while (!requestUriDirectories.empty()) {
		for (size_t i = 0; i < locationConfig.size(); i++)
			if (locationConfig[i]->getUriDirectories() == requestUriDirectories)
				return (locationConfig[i]);
		requestUriDirectories.pop_back();
	}
	throw(clientException(&virtualServerConfig));
}

std::string Message::convertHexa(const std::string& str, Config* config) {
	std::string	result;
	char		conversion;

	for (size_t i = 0; i < str.size(); ++i) {
		if (str[i] == '%') {
			if (i + 2 > str.size())
				throw (clientException(config));
			conversion = getHexaChar(str[i + 1], str[i + 2], config);
			result += conversion;
			i += 2;
		} else {
			result += str[i];
		}
	}
	return (result);
}

char Message::getHexaChar(char first, char second, Config *config) {
	ssize_t	ret;
	char 	result;

	ret = getHexaValue(first);
	if (ret == static_cast<ssize_t>(-1))
		throw (clientException(config));
	result = ret << 4;
	ret = getHexaValue(second);
	if (ret == static_cast<ssize_t>(-1))
		throw (clientException(config));
	result = result + ret;
	return (result);
}

ssize_t Message::getHexaValue(char c) {
	std::string const	hexaLowerBase("0123456789abcdef");
	std::string const	hexaUpperBase("0123456789ABCDEF");
	size_t				index;

	index = hexaLowerBase.find(c);
	if (index == std::string::npos)
		index = hexaUpperBase.find(c);
	if (index == std::string::npos)
		return (-1);
	return (static_cast<ssize_t>(index));
}
