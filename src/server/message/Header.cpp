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
#include "error/Error.hpp"
#include <sstream>
#include <ctime>

Header::Header() : _header() {}

std::string Header::getHeaderByKey(std::string const & key) const {
	std::map<std::string, std::string>::const_iterator	value;

	value = _header.find(key);
	if (value == _header.end())
		throw (headerException());
	return (value->second);
}

std::string Header::getAttribute(std::string const & key, std::string const & attribute) {
	std::string const	separator = "; ";
	std::string 		header;
	std::string 		current_attribute;
	size_t				index;

	//TODO handle quote
	header = getHeaderByKey(key);
	index = header.find(separator);
	if (index == std::string::npos)
		throw (headerException());
	header.erase(0, index + separator.size());
	while (!header.empty()) {
		index = header.find(separator);
		current_attribute = header.substr(0, index);
		if (current_attribute.find(attribute) == 0) {
			index = current_attribute.find('=');
			if (index == std::string::npos)
				throw (headerException());
			current_attribute.erase(0, index + 1);
			return (current_attribute);
		}
		if (index == std::string::npos)
			header.clear();
		else
			header.erase(0, index + separator.size());
	}
	throw (headerException());
}

#include <iostream>
void Header::parseHeader(std::string const & line) {
	std::string	key;
	std::string	value;
	size_t		separatorIndex;

	separatorIndex = line.find(':');
	key = line.substr(0, separatorIndex);
	if (!isValidHeader(key)) {
		std::cout << "Unknown header: " << key << ": " << value << std::endl;
		return;
	}
	value = line.substr(separatorIndex + 1, line.size() - (separatorIndex + 2));
	value = trim(value);
	if (key == "Host" && value.empty())
		throw (headerException());
	addHeader(key, value);
}

void Header::addHeader(std::string const & key, std::string const & value) {
	if (_header.find(key) != _header.end())
		throw (headerException());
	_header[key] = value;
}

void Header::addContentLength(size_t bodySize) {
	std::stringstream	contentLength;

	contentLength << bodySize;
	addHeader("Content-Length", contentLength.str());
}

void Header::addDate() {
	std::time_t	rawTime;
	std::tm*	dateInfo;

	time(&rawTime);
	dateInfo = localtime(&rawTime);
	if (dateInfo == NULL)
		return;
	addHeader("Date", dateToString(dateInfo));
}

std::string Header::dateToString(tm *dateInfo) {
	std::string weekday[] = {"Mon", "Tue", "Wed", "Thu",
							 "Fri", "Sat", "Sun"};
	std::string month[] = {"Jan", "Feb", "Mar", "Apr",
						   "May", "Jun", "Jul", "Aug",
						   "Sep", "Oct", "Nov", "Dec"};
	std::stringstream	date;

	date << weekday[dateInfo->tm_wday] << ", ";
	if (dateInfo->tm_mday < 10)
		date << '0';
	date << dateInfo->tm_mday << ' '
		<< month[dateInfo->tm_mon] << ' '
		<< dateInfo->tm_year + 1900 << ' ';
	if (dateInfo->tm_hour < 10)
		date << '0';
	date << dateInfo->tm_hour << ':';
	if (dateInfo->tm_min < 10)
		date << '0';
	date << dateInfo->tm_min << ':';
	if (dateInfo->tm_sec < 10)
		date << '0';
	date << dateInfo->tm_sec << " GMT";
	return (date.str());
}

bool Header::contain(std::string const & header) const {
	try {
		getHeaderByKey(header);
	} catch (headerException const & e) {
		return (false);
	}
	return (true);
}

std::string Header::toString() const {
	std::string	result;

	for (std::map<std::string, std::string>::const_iterator it = _header.begin(); it != _header.end(); ++it) {
		result += (it->first + ": " + it->second);
		addCRLF(result);
	}
	addCRLF(result);
	return (result);
}

bool Header::isValidHeader(std::string const & headerKey) {
	std::string const	 validHeader[] = {
		"Host",
		"Content-Length",
		"Content-Type",
		"Accept",
		"Transfer-Encoding",
		"Content-Disposition"
	};

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