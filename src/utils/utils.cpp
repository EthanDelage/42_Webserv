/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ethan <ethan@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 16:41:00 by ethan             #+#    #+#             */
/*   Updated: 2023/09/12 16:41:00 by ethan            ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include <string>
#include <vector>
#include <iostream>

std::vector<std::string> split_path(std::string str) {
	std::vector<std::string>	argv;
	std::string					arg;
	size_t						start;
	size_t						i;

	i = 0;
	while (str[i]) {
		if (str[i] != '/')
			throw (std::runtime_error("split_path()"));
		i++;
		start = i;
		while (str[i] != '/')
			i++;
		if (start == i)
			throw (std::runtime_error("split_path()"));
		arg = str.substr(start, i - start);
		argv.push_back(arg);
		i++;
	}
	return (argv);
}