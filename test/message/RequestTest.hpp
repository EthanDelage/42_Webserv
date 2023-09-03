/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestTest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 15:17:00 by edelage           #+#    #+#             */
/*   Updated: 2023/09/03 15:17:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef REQUESTTEST_HPP
# define REQUESTTEST_HPP
# include <gtest/gtest.h>
# include <message/Request.hpp>

class RequestTest : public ::testing::Test {

private:
	Request request;

protected:
	uint8_t						parseMethodTest(std::string const & arg);
	httpVersion_t				parseHttpVersionTest(std::string const & arg);
	std::vector<std::string>	splitTest(std::string const & str);
};

#endif