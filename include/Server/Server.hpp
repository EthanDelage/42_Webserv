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

# include "Config/Config.hpp"
# include "Config/VirtualServerConfig.hpp"

# include <poll.h>

# define ANY_ADDRESS	"0.0.0.0"
# define QUEUE_LENGTH	32
# define POLL_TIMEOUT	100
# define POLL_DEFAULT	0

class Server {

private:
	pollfd*							_socketArray;
	std::vector<socketAddress_t>	_addressArray;
	size_t							_nbSocket;
	Config							_config;

	void		getAddressArray(std::vector<VirtualServerConfig*> serverConfig);
	static int	initSocket(socketAddress_t const & socketAddress);
	static int	acceptClient(int socketFd);

public:
	Server();
	~Server();

	void	init(Config const & config);
	void	listener();

};

#endif