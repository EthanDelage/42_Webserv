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
# include "Config/Config.hpp"

class ConfigTest : public ::testing::Test {

private:
	Config	config;

protected:
	int		parseAutoindexTest(char* line);
	size_t	parseMaxBodySize(char *line);

};

#endif