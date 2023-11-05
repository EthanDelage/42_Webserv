/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/14 11:18:00 by edelage           #+#    #+#             */
/*   Updated: 2023/08/14 11:18:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef SERVER_HPP
# define SERVER_HPP

# include "config/Config.hpp"
# include "config/VirtualServerConfig.hpp"
# include "message/Request.hpp"
# include "message/Response.hpp"

# include <poll.h>

# define ANY_ADDRESS	"0.0.0.0"
# define QUEUE_LENGTH	32
# define POLL_TIMEOUT	(REQUEST_TIMEOUT * 1000)
# define CGI_TIMEOUT	1
# define POLL_DEFAULT	0

class Server {
	typedef std::vector<pollfd>::iterator socketIterator_t;

private:
	Config*							_config;
	std::vector<pollfd>				_socketArray;
	std::vector<socketAddress_t>	_addressArray;
	size_t							_nbServerSocket;
	std::vector<Request>			_requestArray;
	std::vector<Response>			_responseArray;
	char**							_envp;
	static bool						_exit;

	void				addAddressArray(std::vector<VirtualServerConfig*> serverConfig);
	void 				removeDuplicateAddress();
	void				connectionHandler(socketIterator_t& it);
	void 				clientHandler(socketIterator_t& it);
	void 				cgiHandler(socketIterator_t &it);
	void 				requestHandler(size_t requestIndex, socketIterator_t& it);
	void 				responseHandler(size_t responseIndex, socketIterator_t& it);
	void 				sendResponse(size_t requestIndex);
	void 				requestReset(size_t requestIndex);
	void				clientDisconnect(socketIterator_t& it, size_t requestIndex);
	void				cgiResponseDelete(socketIterator_t& it, size_t responseIndex);
	void 				initSocketDefaultAddress();
	void 				initOtherSocket();
	static int			initSocket(socketAddress_t const & socketAddress);
	static int			acceptClient(int socketFd, socketAddress_t& socketAddress);
	static uint32_t		ft_inet_addr(std::string ip);
	static std::string	ft_inet_ntoa(uint32_t s_addr);

public:
	Server();
	~Server();

	void		init(Config* config, char** envp);
	void		listener();
	static void	setCloseServer();

};

#endif