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

int	main(int argc, char** argv) {
	Config	*config;
	Server	server;

	if (argc != 2) {
		std::cerr << "Too few arguments" << std::endl;
		return (1);
	}
	config = new Config;
	try {
		config->parse(argv[1]);
		server.init(*config);
		server.listener();
	} catch (std::exception const & e) {
		delete config;
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	delete config;
	return (0);
}