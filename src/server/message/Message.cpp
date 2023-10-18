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
