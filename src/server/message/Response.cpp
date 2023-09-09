/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 15:13:00 by edelage           #+#    #+#             */
/*   Updated: 2023/09/08 15:13:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include <vector>
#include <sys/socket.h>
#include "message/Response.hpp"
#include "config/LocationConfig.hpp"

Response::Response(Request& request, VirtualServerConfig& virtualServerConfig) : _request(request), _virtualServerConfig(virtualServerConfig) {
//	router();
}

Response::~Response() {}

void Response::send(int clientSocket) {
	recv(clientSocket, (void *)_body.c_str(), _body.size(), 0);
}

void Response::router() {
	uint8_t	httpMethods[] = {GET_METHOD_MASK, POST_METHOD_MASK, DELETE_METHOD_MASK};
	responseFunction_t responseFunction[] = {&Response::responseGet, &Response::responsePost, &Response::responseDelete};

	for (size_t i = 0; i < sizeof(httpMethods) / sizeof(*httpMethods); i++) {
		if (httpMethods[i] == _request.getMethod())
			(this->*responseFunction[i])();
	}
}

#include "iostream"
void Response::responseGet() {
	std::string		root;
	std::ifstream	resource;
	char			buffer[4096];

	root = getResponseRoot();
	std::cout << root << std::endl;
	resource.open(root.c_str());

	if (!resource.is_open())
		throw(ClientException());
	while (!resource.eof())
	{
		resource.read(buffer, sizeof(buffer));
		_body.append(buffer, sizeof(buffer));
	}
}

void Response::responsePost() {
}

void Response::responseDelete() {
}

std::string Response::getResponseRoot() {
	std::string						requestURI;
	std::vector<LocationConfig *>	locationConfig;

	requestURI = _request.getRequestUri();
	locationConfig = _virtualServerConfig.getLocationConfig();
	for (size_t i = 0; i < locationConfig.size(); i++)
	{
		if (locationConfig[i]->getUri() == requestURI)
			return (locationConfig[i]->getRoot() + requestURI);
	}
	return ("");
}

std::string Response::httpVersionToString() const {
	httpVersion_t	httpVersion;

	httpVersion = _request.getHttpVersion();
	return ("HTTP/" + uitoa(httpVersion.major) + '.' + uitoa(httpVersion.minor));
}

std::string Response::getReasonPhrase(uint16_t code) {
	std::string	reasonsPhrases[] = {"Continue", "Ok", "Multiple Choice", "Bad Request", "Internal Server Error"};
	uint16_t	codes[] = {100, 200, 300, 400, 500};

	for (size_t i = 0; sizeof(reasonsPhrases) / sizeof(*reasonsPhrases); ++i) {
		if (code == codes[i])
			return (reasonsPhrases[i]);
	}
}

std::string Response::uitoa(unsigned int n) {
	std::string	result;

	if (n > 9) {
		result = static_cast<char>(n % 10 + '0') + result;
	} else {
		result += static_cast<char>(n + '0');
	}
	return (result);
}
