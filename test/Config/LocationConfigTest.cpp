/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfigTest.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 14:03:00 by edelage           #+#    #+#             */
/*   Updated: 2023/08/11 14:03:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include "LocationConfigTest.hpp"

TEST_F(LocationConfigTest, defaultAllowedHttpMethod) {
	EXPECT_EQ(getAllowedHttpMethod(), DEFAULT_METHOD_MASK);
}

TEST_F(LocationConfigTest, parseDenyValid) {
	uint8_t	allowedHttpMethod;

	allowedHttpMethod = POST_METHOD_MASK | DELETE_METHOD_MASK;
	EXPECT_EQ(parseDenyTest("deny GET;"), allowedHttpMethod);
	clearAllowedHttpMethod();
	allowedHttpMethod = GET_METHOD_MASK | DELETE_METHOD_MASK;
	EXPECT_EQ(parseDenyTest("deny POST;"), allowedHttpMethod);
	clearAllowedHttpMethod();
	allowedHttpMethod = GET_METHOD_MASK | POST_METHOD_MASK;
	EXPECT_EQ(parseDenyTest("deny DELETE;"), allowedHttpMethod);
}

TEST_F(LocationConfigTest, parseDenyDouble) {
	uint8_t	allowedHttpMethod;

	allowedHttpMethod = GET_METHOD_MASK;
	parseDenyTest("deny POST;");
	EXPECT_EQ(parseDenyTest("deny DELETE;"), allowedHttpMethod);
	clearAllowedHttpMethod();
	allowedHttpMethod = POST_METHOD_MASK;
	parseDenyTest("deny GET;");
	EXPECT_EQ(parseDenyTest("deny DELETE;"), allowedHttpMethod);
	clearAllowedHttpMethod();
	allowedHttpMethod = DELETE_METHOD_MASK;
	parseDenyTest("deny GET;");
	EXPECT_EQ(parseDenyTest("deny POST;"), allowedHttpMethod);
}

TEST_F(LocationConfigTest, parseDenyAll) {
	uint8_t	allowedHttpMethod = 0x0;

	parseDenyTest("deny GET;");
	parseDenyTest("deny POST;");
	EXPECT_EQ(parseDenyTest("deny DELETE;"), allowedHttpMethod);
}

TEST_F(LocationConfigTest, parseDenyInvalid) {
	EXPECT_THROW(parseDenyTest("deny  GET;"), std::runtime_error);
	EXPECT_THROW(parseDenyTest("deny GET"), std::runtime_error);
	EXPECT_THROW(parseDenyTest("deny GET ;"), std::runtime_error);
	EXPECT_THROW(parseDenyTest("deny GET POST;"), std::runtime_error);
	EXPECT_THROW(parseDenyTest("deny get;"), std::runtime_error);
	EXPECT_THROW(parseDenyTest("deny Get;"), std::runtime_error);
	EXPECT_THROW(parseDenyTest("deny GETT;"), std::runtime_error);
	EXPECT_THROW(parseDenyTest("deny GGET;"), std::runtime_error);
}

uint8_t LocationConfigTest::parseDenyTest(char *line) {
	std::string		lineStr(line);

	locationConfig.parseLine(lineStr);
	return (locationConfig._allowedHttpMethod);
}

void LocationConfigTest::clearAllowedHttpMethod(void) {
	locationConfig._allowedHttpMethod = DEFAULT_METHOD_MASK;
}
