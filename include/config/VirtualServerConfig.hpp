/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServerConfig.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ethan <ethan@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/09 02:30:00 by ethan             #+#    #+#             */
/*   Updated: 2023/07/09 02:30:00 by ethan            ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef VIRTUALSERVERCONFIG_HPP
# define VIRTUALSERVERCONFIG_HPP

# include <string>
# include <vector>
# include <map>
# include <fstream>
# include <stdint.h>
# include "config/Config.hpp"

# define DEFAULT_PORT		80
# define DEFAULT_ADDRESS	"*"

# define SYNTAX_LISTEN		"Syntax: \"listen\" SP port | ( address [ \":\" port ] ) \";\""
# define SYNTAX_SERVER_NAME	"Syntax: \"server_name\" 1*( SP hostname | address ) \";\""
# define SYNTAX_LOCATION	"Syntax: \"location\" SP uri SP \"{\" 1*( LF *( HT | SP ) <rules with location in context> ) LF *HT \"}\""
# define SYNTAX_ADDRESS		"Syntax: 1*3DIGIT 3 ( \".\" 1*3DIGIT )"
# define SYNTAX_PORT		"Syntax: 1*DIGIT"

class LocationConfig;

class VirtualServerConfig : public Config {
	typedef void (VirtualServerConfig::*parseFunctionType)(std::string&);

# ifdef UNIT_TESTING
	friend class VirtualServerConfigTest;
public:
	VirtualServerConfig();
# endif

private:
	std::vector<std::string>		_serverNames;
	socketAddress_t					_socketAddress;
	std::vector<LocationConfig *>	_locationConfig;

	void			parseLine(std::string& line, std::ifstream& configFile);
	void			router(std::string& directive, std::string& value);
	void			parseLocation(std::ifstream& configFile, std::string& line);
	static uint16_t	getPort(std::string const & str);

protected:
	void		parseListen(std::string& value);
	void		parseServerName(std::string& value);
	static void	removeHorizontalTabAndSpace(std::string& line);

public:
	VirtualServerConfig(Config const & config);
	VirtualServerConfig(VirtualServerConfig const & other);
	~VirtualServerConfig() {};

	std::string						getIp() const;
	uint16_t						getPort() const;
	socketAddress_t					getSocketAddress() const;
	std::vector<LocationConfig *>	getLocationConfig() const;
	std::vector<std::string>		getServerNames() const;


	virtual void	parse(std::ifstream& configFile);
	virtual void	print();
};

#endif