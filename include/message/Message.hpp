/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 16:11:00 by edelage           #+#    #+#             */
/*   Updated: 2023/08/17 16:11:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <string>
# include "message/Header.hpp"
# include "config/LocationConfig.hpp"

# define CRLF "\r\n"

typedef struct httpVersion_s	httpVersion_t;
struct httpVersion_s {
	unsigned int	major;
	unsigned int	minor;
};

class Message{

# ifdef UNIT_TESTING
	friend class RequestTest;
# endif

protected:
	int				_clientSocket;
	Header			_header;
	std::string		_body;
	httpVersion_t	_httpVersion;
	LocationConfig*	_locationConfig;

public:
	Message(int clientSocket);
	~Message();

	std::string		getBody() const;
	Header			getHeader() const;
	httpVersion_t	getHttpVersion() const;
	int 			getClientSocket() const;

	LocationConfig*		getMessageLocation(VirtualServerConfig const & virtualServerConfig, std::string requestUri);

};

#endif