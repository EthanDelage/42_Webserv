/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 13:38:00 by edelage           #+#    #+#             */
/*   Updated: 2023/09/04 13:38:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include "message/Header.hpp"
#include "utils.hpp"

std::string Header::getHeaderByKey(std::string const & key) {
	std::map<std::string, std::string>::iterator	value;

	value = _header.find(key);
	if (value == _header.end())
		throw (headerException());
	return (value->second);
}

#include <iostream>
void Header::parseHeader(std::string const & line) {
	std::string key;
	std::string value;
	size_t		separatorIndex;

	separatorIndex = line.find(':');
	key = line.substr(0, separatorIndex);
	if (!isValidHeader(key)) {
		std::cout << "not valid header" << std::endl;
		return;
	}
	value = line.substr(separatorIndex + 1, line.size() - (separatorIndex + 2));
	value = trim(value);
	addHeader(key, value);
}

void Header::addHeader(std::string const & key, std::string const & value) {
	if (_header.find(key) != _header.end())
		throw (headerException());
	_header[key] = value;
}

std::string Header::toString() const {
	std::string	result;

	for (std::map<std::string, std::string>::const_iterator it = _header.begin(); it != _header.end(); ++it) {
		result += (it->first + ": " + it->second);
		addCRLF(result);
	}
	return (result);
}

bool Header::isValidHeader(std::string const & headerKey) {
	std::string const	 validHeader[] = {
		"Host",
		"Content-Length"};

	for (size_t i = 0; i != sizeof(validHeader) / sizeof (*validHeader); ++i) {
		if (headerKey == validHeader[i])
			return (true);
	}
	return (false);
}

std::string Header::trim(std::string& str) {
	size_t	start;
	size_t	end;

	start = 0;
	end = str.size() - 1;
	while (str[start] == ' ')
		start++;
	while (str[end] == ' ')
		end--;
	return (str.substr(start, end - start));
}