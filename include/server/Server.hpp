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

# include <poll.h>

# define ANY_ADDRESS	"0.0.0.0"
# define QUEUE_LENGTH	32
# define POLL_TIMEOUT	100
# define POLL_DEFAULT	0

class Server {
	typedef std::vector<pollfd>::iterator socketIterator_t;

private:
	std::vector<pollfd>				_socketArray;
	std::vector<socketAddress_t>	_addressArray;
	size_t							_nbServerSocket;
	std::vector<Request*>			_requestArray;
	char**							_envp;

	void			addAddressArray(std::vector<VirtualServerConfig*> serverConfig);
	void 			removeDuplicateAddress();
	void			connectionHandler(socketIterator_t& it, Config const & config);
	void 			clientHandler(socketIterator_t& it);
	void 			requestHandler(size_t requestIndex, socketIterator_t& it);
	void 			responseHandler(socketIterator_t& it, Config const & config);
	void 			sendResponse(size_t requestIndex, Config const & config);
	void 			requestReset(size_t requestIndex);
	void			clientDisconnect(socketIterator_t& it, size_t requestIndex);
	void 			initSocketDefaultAddress();
	void 			initOtherSocket();
	static int		initSocket(socketAddress_t const & socketAddress);
	static int		acceptClient(int socketFd);
	static uint32_t	ft_inet_addr(std::string ip);

public:
	Server();
	~Server();

	void	init(Config const & config, char** envp);
	void	listener(Config const & config);

};

#endif