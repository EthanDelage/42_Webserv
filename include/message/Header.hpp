/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 13:38:00 by edelage           #+#    #+#             */
/*   Updated: 2023/09/04 13:38:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef HEADER_HPP
# define HEADER_HPP

# include <string>
# include <cstring>
# include <ctime>
# include <vector>
# include <iostream>
# include <exception>

class Header {

# ifdef UNIT_TESTING
	friend class HeaderTest;
# endif

public:
	static tm				parseDate(std::string const & line);
	static int				parseWkday(std::string const & day);
	static int				parseWeekday(std::string const & day);
	static int				parseDay(std::string const & day);
	static int				parseMonth(std::string const & arg);
	static int				parseYear(std::string const & year);
	static unsigned int*	parseTime(std::string const & time);

	class dateError : public std::exception {};

	static std::vector<std::string> split(std::string const & str, char c);

private:
	static tm parseRfc1123Date(std::vector<std::string>& argv);
	static tm parseRfc850Date(std::vector<std::string>& argv);
	static tm parseAsciiDate(std::vector<std::string>& argv);

};

#endif