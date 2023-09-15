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
#include <unistd.h>
#include <sstream>
#include "utils.hpp"
#include "message/Response.hpp"
#include "config/LocationConfig.hpp"

Response::Response(Request& request, VirtualServerConfig& virtualServerConfig) : _request(request), _virtualServerConfig(virtualServerConfig) {
	router();
}

Response::~Response() {}

void Response::send(int clientSocket) {
	write(clientSocket, _body.c_str(), _body.size());
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
	std::string					path;
	std::ifstream				resource;
	std::stringstream			buffer;

	path = getResourcePath();
	std::cout << "Path: " << path << std::endl;
	resource.open(path.c_str());
	if (!resource.is_open())
		throw(clientException());
	buffer << resource.rdbuf();
	_body = buffer.str();
}

void Response::responsePost() {
}

void Response::responseDelete() {
}

/**
 * Returns the path of the resource requested.
 * @throw clientException Thrown if the the requestURI is badly formatted or if the resource can't be found.
 */
std::string Response::getResourcePath() {
	std::string 				requestUri;
	std::string 				path;
	std::vector<std::string>	index;
	LocationConfig*				location;

	requestUri = _request.getRequestUri();
	location = getResponseLocation();
	if (requestUri[requestUri.size() - 1] != '/')
		return (location->getRoot() + requestUri);
	index = location->getIndex();
	for (size_t i = 0; i < index.size(); i++) {
		if (index[i][0] == '/') {
			if (access(index[i].c_str(), F_OK) == 0)
				return (index[i]);
		}
		else {
			path = location->getRoot() + '/';
			path += (requestUri.erase(0, location->getUri().size())) + index[i];
			if (access(path.c_str(), F_OK) == 0)
				return (path);
		}
	}
	throw(clientException());
}

LocationConfig*	Response::getResponseLocation() {
	std::vector<LocationConfig*>	locationConfig;
	std::string						requestUri;
	std::vector<std::string>		requestUriDirectories;

	locationConfig = _virtualServerConfig.getLocationConfig();
	requestUri = _request.getRequestUri();
	if (requestUri[requestUri.size() - 1] != '/') {
		requestUri.erase(requestUri.find('/') + 1);
	}
	requestUriDirectories = split_path(requestUri);
	 do {
		for (size_t i = 0; i < locationConfig.size(); i++)
			if (locationConfig[i]->getUriDirectories() == requestUriDirectories)
				return (locationConfig[i]);
		requestUriDirectories.pop_back();
	} while (requestUriDirectories.size() != 0);
	throw(clientException());
}

std::string Response::getResponseRoot() {
	std::string		requestURI;
	std::string 	locationRoot;

	requestURI = _request.getRequestUri();
	locationRoot = getResponseLocation()->getRoot();
	return (locationRoot + requestURI);
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
