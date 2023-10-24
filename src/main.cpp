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
#include "utils.hpp"
#include <iostream>
#include <csignal>

static void closeServer(int signal);

int	main(int argc, char** argv, char **envp) {
	Config	config;
	Server	server;

	signal(SIGINT, &closeServer);
	std::cout << DEFAULT;
	if (argc != 2) {
		printColor(std::cerr, "Too few arguments\n", RED);
		return (1);
	}
	try {
		config.parse(argv[1]);
		server.init(&config, envp);
		server.listener();
	} catch (std::exception const & e) {
		printColor(std::cerr, std::string("Error: ") + e.what() + '\n', RED);
		printColor(std::cerr, "Webserv close\n", RED);
		return (1);
	}
	printColor(std::cout, "\nWebserv close\n", GREEN);
	return (0);
}

static void closeServer(int signal) {
	(void) signal;
	Server::setCloseServer();
}