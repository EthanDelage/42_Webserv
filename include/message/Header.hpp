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
# include <map>
# include <exception>

class Header {

# ifdef UNIT_TESTING
	friend class HeaderTest;
# endif

public:
	Header();
	std::string	getHeaderByKey(std::string const & key) const;

	void		parseHeader(std::string const &line);
	void		addHeader(std::string const & key, std::string const & value);
	void		addContentLength(size_t bodySize);
	void 		addDate();
	std::string toString() const;

	static std::string dateToString(tm* dateInfo);

private:
	std::map<std::string, std::string>	_header;

	std::string	trim(std::string & str);
	static bool	isValidHeader(std::string const & headerKey);

};

#endif