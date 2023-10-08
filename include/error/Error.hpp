/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hferraud <hferraud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 14:02:00 by hferraud          #+#    #+#             */
/*   Updated: 2023/09/19 14:02:00 by hferraud         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef ERROR_HPP
# define ERROR_HPP

# include <exception>

# define INFORMATIONAL_STATUS_CODE		100
# define SUCCESS_STATUS_CODE			200
# define REDIRECTION_STATUS_CODE		300
# define CLIENT_ERROR_STATUS_CODE		400
# define SERVER_ERROR_STATUS_CODE		500

# define INFORMATIONAL_REASON_PHRASE	"Continue"
# define SUCCESS_REASON_PHRASE			"Ok"
# define REDIRECTION_REASON_PHRASE		"Multiple Choices"
# define CLIENT_ERROR_REASON_PHRASE		"Bad Request"
# define SERVER_ERROR_REASON_PHRASE		"Internal Server Error"
# include <string>
# include "config/Config.hpp"

class clientException : public std::exception {
private:
	std::string	_errorPage;
	uint8_t 	_methodMask;
public:
	clientException(Config const * config);
	clientException(Config const * config, uint8_t methodMask);
	~clientException() throw();

	std::string	getErrorPage() const;
	uint8_t		getMethodMask() const;
};

class serverException : public std::exception {
private:
	std::string	_errorPage;
public:
	serverException(Config const * config);
	~serverException() throw();

	std::string	getErrorPage() const;
};

class headerException : public std::exception {};

class redirectionException : public std::exception {
private:
	std::string _errorPage;
public:
	redirectionException(Config const * config);
	~redirectionException() throw();

	std::string	getErrorPage() const;
};

#endif