/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 21:04:00 by edelage           #+#    #+#             */
/*   Updated: 2023/07/06 21:04:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include "config/Config.hpp"
#include "server/Server.hpp"
#include <iostream>
#include <csignal>

static void closeServer(int signal);

int	main(int argc, char** argv, char **envp) {
	Config	config;
	Server	server;

	signal(SIGINT, &closeServer);
	if (argc != 2) {
		std::cerr << "Too few arguments" << std::endl;
		return (1);
	}
	try {
		config.parse(argv[1]);
		server.init(&config, envp);
		server.listener();
	} catch (std::exception const & e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	std::cout << std::endl << "Webserv close" << std::endl;
	return (0);
}

static void closeServer(int signal) {
	(void) signal;
	Server::setCloseServer();
}