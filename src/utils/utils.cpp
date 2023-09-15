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

/**
 * @brief Split the string in a vector of strings that represent every directory.
 * The path must start and end with '/'.
 * @throw runtime_error Thrown when the path is badly formatted.
 */
std::vector<std::string> split_path(std::string path) {
	std::vector<std::string>	argv;
	std::string					arg;
	size_t						start;
	size_t						i;

	i = 0;
	while (path[i]) {
		if (path[i] != '/')
			throw (std::runtime_error("split_path()"));
		i++;
		start = i;
		while (path[i] && path[i] != '/')
			i++;
		if (path[i] && start == i)
			throw (std::runtime_error("split_path()"));
		if (start != i) {
			arg = path.substr(start, i - start);
			argv.push_back(arg);
		}
	}
	return (argv);
}