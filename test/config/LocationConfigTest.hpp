/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfigTest.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 14:03:00 by edelage           #+#    #+#             */
/*   Updated: 2023/08/11 14:03:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef LOCATIONCONFIGTEST_HPP
# define LOCATIONCONFIGTEST_HPP

# include "gtest/gtest.h"
# include "config/LocationConfig.hpp"

class LocationConfigTest : public ::testing::Test {

private:
	LocationConfig	locationConfig;

public:
	uint8_t	parseDenyTest(char* line);

	uint8_t	getAllowedHttpMethod(void) {return (locationConfig._allowedHttpMethod); }
	void	clearAllowedHttpMethod(void);
};

#endif