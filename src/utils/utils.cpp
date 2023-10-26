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
#include <fstream>
#include <iostream>
#include <iomanip>
#include "utils.hpp"

extern std::ofstream logFile;
extern std::ofstream errorLogFile;

static void	printTimestamp(std::ostream & os);

/**
 * @brief Split the string in a vector of strings that represent every directory.
 * The first string of the vector will always be an empty string representing the current directory.
 * The path must start and end with '/'.
 * @throw runtime_error Thrown when the path is badly formatted.
 */
std::vector<std::string> split_path(std::string& path) {
	std::vector<std::string>	argv;
	std::string					arg;
	size_t						start;
	size_t						i;

	argv.push_back("");
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

/**
 * add a CRLF to a string.\n
 * A CRLF represent the end of line in the HTTP protocol and correspond to "\\r\\n".
 */
void addCRLF(std::string& str) {
	str += "\r\n";
}

std::string toLower(std::string const & str) {
	std::string result;

	for (size_t i = 0; i < str.size(); ++i)
		result += static_cast<char>(tolower(str[i]));
	return (result);
}

void printColor(std::ostream& os, std::string const & str, std::string const & color) {
	static char	lastCharPrint = '\n';

	os << color << str << DEFAULT << std::flush;
	if (logFile.is_open()) {
		if (lastCharPrint == '\n')
			printTimestamp(logFile);
		logFile << str;
	} if (&os == &std::cerr && errorLogFile.is_open()) {
		if (lastCharPrint == '\n')
			printTimestamp(errorLogFile);
		errorLogFile << str;
	}
	lastCharPrint = str[str.size() - 1];
}

static void	printTimestamp(std::ostream & os) {
	time_t	current_time;
	tm		*timeinfo;

	time(&current_time);
	timeinfo = localtime(&current_time);
	os	<< '['
		<< std::setfill('0') << std::setw(4) << timeinfo->tm_year + 1900 << '-'
		<< std::setfill('0') << std::setw(2) << timeinfo->tm_mon + 1 << '-'
		<< std::setfill('0') << std::setw(2) << timeinfo->tm_mday
		<< ' '
		<< std::setfill('0') << std::setw(2) << timeinfo->tm_hour << ':'
		<< std::setfill('0') << std::setw(2) << timeinfo->tm_min << ':'
		<< std::setfill('0') << std::setw(2) << timeinfo->tm_sec
		<< "] ";
}
