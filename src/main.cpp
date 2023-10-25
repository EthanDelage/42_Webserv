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

std::ofstream	logFile;
std::ofstream	errorLogFile;

static void closeServer(int signal);
static void	createLogFile();
static void closeLogFile();

int	main(int argc, char** argv, char **envp) {
	Config	config;
	Server	server;

	signal(SIGINT, &closeServer);
	if (logFile.is_open())
		std::cout << "logfile" << std::endl;
	createLogFile();
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
		closeLogFile();
		return (1);
	}
	std::cout << std::endl;
	printColor(std::cout, "Webserv close\n", GREEN);
	closeLogFile();
	return (0);
}

static void closeServer(int signal) {
	(void) signal;
	Server::setCloseServer();
}

static void	createLogFile() {
	logFile.open("webserv.log");
	errorLogFile.open("webservError.log");
}

static void closeLogFile() {
	if (logFile.is_open())
		logFile.close();
	if (errorLogFile.is_open())
		errorLogFile.close();
}