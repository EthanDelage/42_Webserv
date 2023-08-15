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
#include "Server/Server.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>

Server::Server() {
	_socketArray = NULL;
	_nbSocket = 0;
}

Server::~Server() {
	if (_socketArray != NULL) {

	}
}

#include <iostream>
void Server::init(const Config& config) {
	std::vector<socketAddress_t>	socketAddresses;

	socketAddresses = getSocketAddresses(config.getServerConfig());
	_nbSocket = socketAddresses.size();
	_socketArray = new int[_nbSocket];
	for (size_t i = 0; i < _nbSocket; ++i) {
		std::cout << "IP: " << socketAddresses[i].first << std::endl;
		std::cout << "Port: " << socketAddresses[i].second << std::endl << std::endl;
		try {
			_socketArray[i] = initSocket(socketAddresses[i]);
		} catch (std::runtime_error const & e) {
			throw (std::runtime_error(std::string("initSocket() failed: ") + e.what()));
		}
	}
}

std::vector<socketAddress_t> Server::getSocketAddresses(std::vector<VirtualServerConfig*> serverConfig) {
	std::vector<socketAddress_t>	result;
	socketAddress_t					socketAddress;
	std::vector<VirtualServerConfig*>::const_iterator 	it;

	for (it = serverConfig.begin(); it != serverConfig.end(); ++it) {
		socketAddress = (*it)->getSocketAddress();
		if (socketAddress.first == "*")
			socketAddress.first = ANY_ADDRESS;
		if (std::find(result.begin(), result.end(), socketAddress) == result.end())
			result.push_back(socketAddress);
	}
	return (result);
}

int Server::initSocket(socketAddress_t const & socketAddress) {
	int 				socketFd;
	struct sockaddr_in	address;
	int					opt = 1;

	socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketFd == -1)
		throw (std::runtime_error("socket() failed"));
	if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		throw (std::runtime_error("setsockopt() failed"));
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(socketAddress.first.c_str());
	address.sin_port = htons(socketAddress.second);
	if (bind(socketFd, (sockaddr*) &address, sizeof(address)) == -1) {
		close(socketFd);
		throw (std::runtime_error("bind() failed"));
	}
}
