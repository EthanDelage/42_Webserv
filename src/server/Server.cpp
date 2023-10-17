/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/14 11:18:00 by edelage           #+#    #+#             */
/*   Updated: 2023/08/14 11:18:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include "server/Server.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>
#include <iterator>
#include <iostream>
#include "message/Request.hpp"
#include "message/Response.hpp"
#include "error/Error.hpp"

Server::Server() {
	_nbServerSocket = 0;
}

Server::~Server() {
	for (std::vector<pollfd>::iterator it = _socketArray.begin(); it != _socketArray.end(); ++it) {
		if (it->fd == -1)
			close(it->fd);
	}
}

void Server::init(Config const & config) {
	pollfd	currentServerSocket;

	addAddressArray(config.getServerConfig());
	_nbServerSocket = _addressArray.size();
	for (size_t i = 0; i < _nbServerSocket; ++i) {
		try {
			currentServerSocket.fd = initSocket(_addressArray[i]);
			currentServerSocket.events = POLLIN;
			currentServerSocket.revents = POLL_DEFAULT;
			_socketArray.push_back(currentServerSocket);
		} catch (std::runtime_error const & e) {
			std::cerr << e.what() << " for " << _addressArray[i].first
				<< ':' << _addressArray[i].second << std::endl;
			_addressArray.erase(_addressArray.begin() + i);
			--i;
		}
	}
}

void Server::listener(Config const & config) {
	socketIterator_t	it;

	for (size_t i = 0; i < _nbServerSocket; ++i) {
		if (listen(_socketArray[i].fd, QUEUE_LENGTH) == -1)
			throw(std::runtime_error("listen() failed"));
	}
	while (true) {
		if (poll(_socketArray.data(), _socketArray.size(), POLL_TIMEOUT) == -1)
			throw (std::runtime_error("poll() failed"));
		it = _socketArray.begin();
		connectionHandler(it, config);
		it = _socketArray.begin() + _nbServerSocket;
		clientHandler(it);
		it = _socketArray.begin() + _nbServerSocket;
		responseHandler(it, config);
	}
}

void Server::connectionHandler(socketIterator_t& it, Config const & config) {
	pollfd					newClientSocket;
	Request*				newRequest;
	VirtualServerConfig*	virtualServerConfig;

	for (; std::distance(_socketArray.begin(), it) != static_cast<long>(_nbServerSocket); ++it) {
		if (it->revents == POLLIN) {
			newClientSocket.fd = acceptClient(it->fd);
			newClientSocket.events = POLLIN;
			newClientSocket.revents = POLL_DEFAULT;
			virtualServerConfig = config.getDefaultServer(_addressArray[std::distance(_socketArray.begin(), it)]);
			newRequest = new Request(newClientSocket.fd, virtualServerConfig);
			_socketArray.push_back(newClientSocket);
			_requestArray.push_back(newRequest);
			return;
		} else if (it->revents != POLL_DEFAULT) {
			throw (std::runtime_error("poll error occured"));
		}
	}
}

void Server::clientHandler(socketIterator_t& it) {
	std::string		line;
	size_t			requestIndex;

	requestIndex = 0;
	for (; it != _socketArray.end(); ++it) {
		if (it->revents == POLLIN) {
			requestHandler(requestIndex, it);
		} else if (difftime(time(NULL), _requestArray[requestIndex]->getTimeLastAction()) >= REQUEST_TIMEOUT) {
			clientDisconnect(it, requestIndex);
		}
		++requestIndex;
	}
}

void Server::requestHandler(size_t requestIndex, socketIterator_t& it) {
	Request*			currentRequest;

	currentRequest = _requestArray[requestIndex];
	try {
		currentRequest->readBuffer();
	} catch (clientException const & e) {
		Response::sendClientError(
			CLIENT_ERROR_STATUS_CODE,
			currentRequest->getClientSocket(),
			e
		);
		if (currentRequest->getStatus() == REQUEST_LINE) {
			clientDisconnect(it, requestIndex);
		} else {
			requestReset(requestIndex);
		}
	} catch (clientDisconnected const & e) {
		clientDisconnect(it, requestIndex);
	}
}

void Server::responseHandler(socketIterator_t& it, Config const & config) {
	size_t requestIndex;

	requestIndex = 0;
	for (; it != _socketArray.end(); ++it) {
		if (_requestArray[requestIndex]->getStatus() == END) {
			try {
				sendResponse(requestIndex, config);
			} catch (clientDisconnected const& e) {
				clientDisconnect(it, requestIndex);
			}
		}
		++requestIndex;
	}
}

void Server::sendResponse(size_t requestIndex, Config const & config) {
	Request*			currentRequest;

	currentRequest = _requestArray[requestIndex];
	try {
		currentRequest->updateServerConfig(config);
		Response response(*currentRequest);

		response.print();
		response.setDate();
		response.send();
	} catch (clientException const& e) {
		Response::sendClientError(
			CLIENT_ERROR_STATUS_CODE,
			currentRequest->getClientSocket(),
			e
		);
	} catch (serverException const& e) {
		Response::sendServerError(
			SERVER_ERROR_STATUS_CODE,
			currentRequest->getClientSocket(),
			e.getErrorPage()
		);
	}
	requestReset(requestIndex);
}

void Server::requestReset(size_t requestIndex) {
	Request*	oldRequest;
	Request*	newRequest;

	oldRequest = _requestArray[requestIndex];
	newRequest = new Request(oldRequest->getClientSocket(), oldRequest->getDefaultServerConfig());
	_requestArray[requestIndex] = newRequest;
	delete oldRequest;
}

void	Server::addAddressArray(std::vector<VirtualServerConfig*> serverConfig) {
	socketAddress_t										socketAddress;
	std::vector<VirtualServerConfig*>::const_iterator 	it;

	for (it = serverConfig.begin(); it != serverConfig.end(); ++it) {
		socketAddress = (*it)->getSocketAddress();
		if (socketAddress.first == "*")
			socketAddress.first = ANY_ADDRESS;
		if (std::find(_addressArray.begin(), _addressArray.end(), socketAddress) == _addressArray.end())
			_addressArray.push_back(socketAddress);
	}
}

void Server::clientDisconnect(Server::socketIterator_t& it, size_t requestIndex) {
	close(_requestArray[requestIndex]->getClientSocket());
	delete _requestArray[requestIndex];
	(void) _requestArray.erase(_requestArray.begin() + requestIndex);
	it = _socketArray.erase(_socketArray.begin() + _nbServerSocket + requestIndex);
	if (it != _socketArray.begin())
		--it;
}

int Server::initSocket(socketAddress_t const & socketAddress) {
	int 				socketFd;
	struct sockaddr_in	address;
	int					opt = 1;

	socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketFd == -1)
		throw (std::runtime_error("socket() failed"));
	if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
		throw (std::runtime_error("setsockopt() failed"));
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = ft_inet_addr(socketAddress.first);
	address.sin_port = htons(socketAddress.second);
	if (bind(socketFd, (struct sockaddr*) &address, sizeof(address)) == -1) {
		close(socketFd);
		throw (std::runtime_error("bind() failed"));
	}
	return (socketFd);
}

int Server::acceptClient(int socketFd) {
	int 				clientSocketFd;
	struct sockaddr_in	address;
	socklen_t			addressLength;

	addressLength = sizeof(address);
	clientSocketFd = accept(socketFd, (struct sockaddr*) &address, &addressLength);
	if (clientSocketFd == -1)
		throw (std::runtime_error("accept() failed"));
	return (clientSocketFd);
}

uint32_t Server::ft_inet_addr(std::string ip) {
	uint32_t	s_addr;
	size_t		separatorIndex;

	s_addr = 0;
	while (!ip.empty()) {
		s_addr = (s_addr << 8) + atoi(ip.c_str());
		separatorIndex = ip.find('.');
		if (separatorIndex != std::string::npos)
			ip = ip.substr(separatorIndex + 1, ip.size() - (separatorIndex + 1));
		else
			ip = "";
	}
	s_addr = htonl(s_addr);
	return (s_addr);
}
