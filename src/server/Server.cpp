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
#include <sstream>
#include <signal.h>
#include "message/Request.hpp"
#include "message/Response.hpp"
#include "error/Error.hpp"

Server::Server() {
	_nbServerSocket = 0;
}

Server::~Server() {
	size_t							i;
	std::vector<pollfd>::iterator	it;

	i = 0;
	it = _socketArray.begin();
	while (it != _socketArray.end()) {
		if (i >= _nbServerSocket)
			delete _requestArray[i - _nbServerSocket];
		if (it->fd == -1)
			close(it->fd);
		++it;
		++i;
	}
}

void Server::init(Config const & config, char** envp) {
	_envp = envp;
	addAddressArray(config.getServerConfig());
	_nbServerSocket = _addressArray.size();
	initSocketDefaultAddress();
	initOtherSocket();
	if (_nbServerSocket == 0) {
		throw (std::runtime_error("No server created"));
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
	socketAddress_t			socketAddress;
	VirtualServerConfig*	virtualServerConfig;

	for (; std::distance(_socketArray.begin(), it) != static_cast<long>(_nbServerSocket); ++it) {
		if (it->revents == POLLIN) {
			newClientSocket.fd = acceptClient(it->fd, socketAddress);
			newClientSocket.events = POLLIN;
			newClientSocket.revents = POLL_DEFAULT;
			virtualServerConfig = config.getDefaultServer(socketAddress);
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
		currentRequest->process();
	} catch (clientException const & e) {
		Response::sendClientError(currentRequest->getClientSocket(), e);
		if (currentRequest->getStatus() == REQUEST_LINE) {
			clientDisconnect(it, requestIndex);
		} else {
			requestReset(requestIndex);
		}
	} catch (serverException const & e) {
		Response::sendServerError(
			currentRequest->getClientSocket(),
			e.getErrorPage()
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
		Response response(*currentRequest, _envp);

		response.print();
		response.setDate();
		response.send();
	} catch (clientException const& e) {
		Response::sendClientError(currentRequest->getClientSocket(), e);
	} catch (serverException const& e) {
		Response::sendServerError(
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
	removeDuplicateAddress();
}

void	Server::removeDuplicateAddress() {
	socketAddress_t							anyAddress;
	std::vector<socketAddress_t>::iterator	itAnyAddress;
	std::vector<socketAddress_t>::iterator	it;

	anyAddress.first = ANY_ADDRESS;
	it = _addressArray.begin();
	while (it != _addressArray.end()) {
		anyAddress.second = it->second;
		itAnyAddress = std::find(_addressArray.begin(), _addressArray.end(), anyAddress);
		if (it->first != ANY_ADDRESS && itAnyAddress != _addressArray.end())
			it = _addressArray.erase(it);
		else
			++it;
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

void Server::initSocketDefaultAddress() {
	pollfd	currentServerSocket;

	for (size_t i = 0; i < _nbServerSocket; ++i) {
		try {
			if (_addressArray[i].first == ANY_ADDRESS) {
				currentServerSocket.fd = initSocket(_addressArray[i]);
				currentServerSocket.events = POLLIN;
				currentServerSocket.revents = POLL_DEFAULT;
				_socketArray.push_back(currentServerSocket);
			}
		} catch (std::runtime_error const & e) {
			std::cerr << e.what() << " for " << _addressArray[i].first
					  << ':' << _addressArray[i].second << std::endl;
			_addressArray.erase(_addressArray.begin() + i);
			--_nbServerSocket;
			--i;
		}
	}
}

void Server::initOtherSocket() {
	pollfd									currentServerSocket;

	for (size_t i = 0; i < _nbServerSocket; ++i) {
		try {
			if (_addressArray[i].first != ANY_ADDRESS) {
				currentServerSocket.fd = initSocket(_addressArray[i]);
				currentServerSocket.events = POLLIN;
				currentServerSocket.revents = POLL_DEFAULT;
				_socketArray.push_back(currentServerSocket);
			}
		} catch (std::runtime_error const & e) {
			std::cerr << e.what() << " for " << _addressArray[i].first
					  << ':' << _addressArray[i].second << std::endl;
			_addressArray.erase(_addressArray.begin() + i);
			--_nbServerSocket;
			--i;
		}
	}
}

int Server::initSocket(socketAddress_t const & socketAddress) {
	int 				socketFd;
	struct sockaddr_in	address;

	socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketFd == -1)
		throw (std::runtime_error("socket() failed"));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = ft_inet_addr(socketAddress.first);
	address.sin_port = htons(socketAddress.second);
	if (bind(socketFd, (struct sockaddr*) &address, sizeof(address)) == -1) {
		close(socketFd);
		throw (std::runtime_error("bind() failed"));
	}
	return (socketFd);
}

int Server::acceptClient(int socketFd, socketAddress_t& socketAddress) {
	int 				clientSocketFd;
	struct sockaddr_in	address;
	socklen_t			addressLength;

	addressLength = sizeof(address);
	clientSocketFd = accept(socketFd, (struct sockaddr*) &address, &addressLength);
	if (clientSocketFd == -1)
		throw (std::runtime_error("accept() failed"));
	getsockname(clientSocketFd, (struct sockaddr*) &address, &addressLength);
	socketAddress.first = ft_inet_ntoa(address.sin_addr.s_addr);
	socketAddress.second = ntohs(address.sin_port);
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

std::string Server::ft_inet_ntoa(uint32_t s_addr) {
	std::stringstream	ip;

	s_addr = ntohl(s_addr);
	ip << ((s_addr & 0xff000000) >> 24)
		<< '.' << ((s_addr & 0xff0000) >> 16)
		<< '.' << ((s_addr & 0xff00) >> 8)
		<< '.' << (s_addr & 0xff);
	return (ip.str());
}