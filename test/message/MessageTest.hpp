/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MessageTest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 15:17:00 by edelage           #+#    #+#             */
/*   Updated: 2023/09/03 15:17:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef MESSAGETEST_HPP
# define MESSAGETEST_HPP
# include <gtest/gtest.h>
# include <message/Message.hpp>

class MessageTest : public ::testing::Test {

private:

public:
	MessageTest();

	~MessageTest();

};

#endif