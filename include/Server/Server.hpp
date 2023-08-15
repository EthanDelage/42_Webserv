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

# define ANY_ADDRESS	"0.0.0.0"

class Server {

private:
	int*	_socketArray;
	size_t	_nbSocket;

	std::vector<socketAddress_t>	getSocketAddresses(std::vector<VirtualServerConfig*> serverConfig);
	static int						initSocket(socketAddress_t const & socketAddress);

public:
	Server();
	~Server();

	void	init(Config const & config);

};

#endif