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
#include <Config/Config.hpp>
#include <iostream>

int	main(int argc, char** argv) {
	(void) argv;
	if (argc != 2) {
		std::cerr << "Too few arguments" << std::endl;
		return (1);
	}
	try {
		Config config;
	} catch (std::exception const &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}