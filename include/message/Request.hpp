/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 13:34:00 by edelage           #+#    #+#             */
/*   Updated: 2023/09/03 13:34:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <stdint.h>
# include <vector>
# include <string>
# include <cstring>
# include <cstdlib>
# include <limits>
# include <cerrno>
# include <ctime>
# include "message/Message.hpp"
# include "method.hpp"
# include "config/VirtualServerConfig.hpp"

# define HTTP_HIGHEST_MAJOR_VERSION_SUPPORTED	1
# define HTTP_HIGHEST_MINOR_VERSION_SUPPORTED	1

# define REQUEST_TIMEOUT	60

typedef enum requestStatus_e {
	REQUEST_LINE,
	HEADER,
	BODY,
	CHUNKED,
	END
} requestStatus_t;

class Request : public Message {

# ifdef UNIT_TESTING
	friend class RequestTest;

public:
	Request() : Message(1) {};
# endif

private:
	Config*					_config;
	requestStatus_t			_status;
	uint8_t					_method;
	std::string				_requestURI;
	std::string				_buffer;
	VirtualServerConfig*	_serverConfig;
	VirtualServerConfig*	_defaultServerConfig;
	time_t					_timeLastAction;

	void	parseRequestLine();
	void	parseRequestHeader();
	void	parseRequestBody();
	void 	parseRequestChunk();
	void	parseMethod(std::string const & arg);
	void	parseHttpVersion(std::string const & arg);
	bool 	requestContainBody() const;
	bool	requestChunked() const;

	void					readBuffer();
	void					findLocationConfig();
	uint8_t					getMethodByName(std::string const & method) const;
	std::vector<std::string> split(std::string const & str) const;

public:
	Request(int clientSocket, VirtualServerConfig* virtualServerConfig, Config* config);
	Request(Request* oldRequest);
	~Request();

	uint8_t					getMethod() const;
	std::string 			getRequestUri() const;
	std::string 			getBuffer() const;
	requestStatus_t 		getStatus() const;
	time_t					getTimeLastAction() const;
	Config*					getConfig() const;
	VirtualServerConfig*	getServerConfig() const;
	VirtualServerConfig*	getDefaultServerConfig() const;
	LocationConfig*			getLocationConfig() const;

	void 		router();
	void		process();
	void 		print() const;

};

#endif