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

tm Header::parseDate(std::string const & line) {
	std::vector<std::string>	argv;

	argv = split(line, ' ');
	if (argv.size() == 6)
		return (parseRfc1123Date(argv));
	else if (argv.size() == 5)
		return (parseAsciiDate(argv));
	else if (argv.size() == 4)
		return (parseRfc850Date(argv));
	throw (dateError());
}

tm Header::parseRfc1123Date(std::vector<std::string>& argv) {
	tm				date;
	unsigned int*	time;

	memset(&date, 0, sizeof(tm));
	if (argv[0][3] == ',')
		argv[0].resize(3);
	date.tm_wday = parseWkday(argv[0]);
	date.tm_mday = parseDay(argv[1]);
	date.tm_mon = parseMonth(argv[2]);
	date.tm_year = parseYear(argv[3]);
	time = parseTime(argv[4]);
	date.tm_hour = time[0];
	date.tm_min = time[1];
	date.tm_sec = time[2];
	delete time;
	if (argv[5] != "GMT")
		throw (dateError());
	return (date);
}

tm Header::parseRfc850Date(std::vector<std::string>& argv) {
	tm				date;
	unsigned int*	time;

	memset(&date, 0, sizeof(tm));
	if (argv[0][argv[0].size() - 1] == ',')
		argv[0].resize(argv[0].size() - 1);
	date.tm_wday = parseWeekday(argv[0]);
	//TODO add parse date following this format 10-Jan-23
	time = parseTime(argv[2]);
	date.tm_hour = time[0];
	date.tm_min = time[1];
	date.tm_sec = time[2];
	delete time;
	if (argv[5] != "GMT")
		throw (dateError());
	return (date);
}

tm Header::parseAsciiDate(std::vector<std::string> & argv) {
	tm				date;
	unsigned int*	time;

	memset(&date, 0, sizeof(tm));
	date.tm_wday = parseWkday(argv[0]);
	date.tm_mon = parseMonth(argv[1]);
	if (argv[2][0] == ' ')
		argv[2][0] = '0';
	date.tm_mday = parseDay(argv[2]);
	time = parseTime(argv[3]);
	date.tm_hour = time[0];
	date.tm_min = time[1];
	date.tm_sec = time[2];
	delete time;
	date.tm_year = parseYear(argv[4]);
	return (date);
}

int Header::parseWkday(std::string const & day) {
	std::string	wkday[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

	for (int i = 0; i < 7; ++i) {
		if (day == wkday[i])
			return (i);
	}
	throw (dateError());
}

int Header::parseWeekday(std::string const & day) {
	std::string	weekday[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

	for (int i = 0; i < 7; ++i) {
		if (day == weekday[i])
			return (i);
	}
	throw (dateError());
}

int Header::parseDay(const std::string& day) {
	if (day.size() != 2 || !std::isdigit(day[0]) || !std::isdigit(day[1]))
		throw (dateError());
	return (10 * (day[0] - '0') + day[1] - '0');
}

int Header::parseMonth(std::string const & arg) {
	std::string month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
						   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	for (int i = 0; i < 12; ++i) {
		if (arg == month[i])
			return (i);
	}
	throw (dateError());
}

int Header::parseYear(std::string const & year) {
	int	yearNumber = 0;

	if (year.size() != 4)
		throw (dateError());
	for (int i = 0; i < 4; ++i) {
		if (!std::isdigit(year[i]))
			throw (dateError());
		yearNumber = yearNumber * 10 + year[i] - '0';
	}
	return (yearNumber);
}

unsigned int *Header::parseTime(std::string const & time) {
	std::vector<std::string>	argv;
	unsigned int				*timeValue;

	argv = split(time, ':');
	if (argv.size() != 3)
		throw (dateError());
	timeValue = new unsigned int[3];
	for (int i = 0; i < 3; ++i) {
		if (!std::isdigit(argv[i][0]) || !std::isdigit(argv[i][1])) {
			delete timeValue;
			throw (dateError());
		}
		timeValue[i] = (argv[i][0] - '0') * 10 + argv[i][1] - '0';
	}
	if (timeValue[0] > 23 || timeValue[1] > 59 || timeValue[2] > 59) {
		delete timeValue;
		throw (dateError());
	}
	return (timeValue);
}

// TODO: failed with "Mon Jan   1 10:15:40 2023"
std::vector<std::string> Header::split(std::string const & str, char c) {
	std::vector<std::string>	argv;
	std::string					arg;
	size_t						start;

	for (size_t i = 0; i < str.size(); i++) {
		start = i;
		while (str[i] && str[i] != c)
			i++;
		arg = str.substr(start, i - start);
		argv.push_back(arg);
	}
	return (argv);
}
