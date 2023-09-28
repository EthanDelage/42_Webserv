/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigTest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 23:42:00 by edelage           #+#    #+#             */
/*   Updated: 2023/07/23 23:42:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef CONFIGTEST_HPP
# define CONFIGTEST_HPP
# include <gtest/gtest.h>
# include "config/Config.hpp"

class ConfigTest : public ::testing::Test {

private:
	Config	config;

protected:
	int								parseAutoindexTest(char* line);
	std::map<uint16_t, std::string>	parseErrorPage(char* line);
	std::vector<std::string>		parseIndexTest(char* line);
	size_t							parseMaxBodySizeTest(char* line);
	std::string						parseRootTest(char* line);
	std::map<std::string, std::string>	parseTypeTest(char* line);

	int								getAutoindex() const {return (config._autoindex);};
	size_t 							getMaxBodySize() const {return (config._maxBodySize);};
	std::string						getRoot() const {return (config._root);};
	std::vector<std::string>		getIndex() const {return (config._index);}
	std::map<uint16_t, std::string>	getErrorPage() const {return (config._errorPage);}

};

#endif