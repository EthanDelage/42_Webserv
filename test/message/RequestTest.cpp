/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestTest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 15:17:00 by edelage           #+#    #+#             */
/*   Updated: 2023/09/03 15:17:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include "RequestTest.hpp"

TEST_F(RequestTest, parseMethodValid) {
	EXPECT_EQ(parseMethodTest("GET"), GET_METHOD_MASK);
	EXPECT_EQ(parseMethodTest("POST"), POST_METHOD_MASK);
	EXPECT_EQ(parseMethodTest("DELETE"), DELETE_METHOD_MASK);
}

TEST_F(RequestTest, parseMethodInvalid) {
	EXPECT_THROW(parseMethodTest("get"), std::runtime_error);
	EXPECT_THROW(parseMethodTest("post"), std::runtime_error);
	EXPECT_THROW(parseMethodTest("delete"), std::runtime_error);
	EXPECT_THROW(parseMethodTest("GETT"), std::runtime_error);
	EXPECT_THROW(parseMethodTest("gET"), std::runtime_error);
	EXPECT_THROW(parseMethodTest("POSt"), std::runtime_error);
	EXPECT_THROW(parseMethodTest(""), std::runtime_error);
}

TEST_F(RequestTest, parseHttpVersionValid) {
	httpVersion_t	versionExpect;
	httpVersion_t	versionReturned;

	versionExpect = {1, 1};
	versionReturned = parseHttpVersionTest("HTTP/1.1\r\n");
	EXPECT_EQ(versionReturned.major, versionExpect.major);
	EXPECT_EQ(versionReturned.minor, versionExpect.minor);
	versionExpect = {0, 1};
	versionReturned = parseHttpVersionTest("HTTP/0.1\r\n");
	EXPECT_EQ(versionReturned.major, versionExpect.major);
	EXPECT_EQ(versionReturned.minor, versionExpect.minor);
	versionExpect = {0, 13};
	versionReturned = parseHttpVersionTest("HTTP/0.13\r\n");
	EXPECT_EQ(versionReturned.major, versionExpect.major);
	EXPECT_EQ(versionReturned.minor, versionExpect.minor);
	versionExpect = {1, 0};
	versionReturned = parseHttpVersionTest("HTTP/1.0\r\n");
	EXPECT_EQ(versionReturned.major, versionExpect.major);
	EXPECT_EQ(versionReturned.minor, versionExpect.minor);
}

TEST_F(RequestTest, parseHttpVersionInvalid) {
	EXPECT_THROW(parseHttpVersionTest("\r\n"), std::runtime_error);
	EXPECT_THROW(parseHttpVersionTest("HTTP/\r\n"), std::runtime_error);
	EXPECT_THROW(parseHttpVersionTest("HTTP/ \r\n"), std::runtime_error);
	EXPECT_THROW(parseHttpVersionTest("HTTP/1.1 \r\n"), std::runtime_error);
	EXPECT_THROW(parseHttpVersionTest("HTTP/1 .1\r\n"), std::runtime_error);
	EXPECT_THROW(parseHttpVersionTest("HTTP/1. 1\r\n"), std::runtime_error);
	EXPECT_THROW(parseHttpVersionTest("HTTP/ 1.1\r\n"), std::runtime_error);
	EXPECT_THROW(parseHttpVersionTest("HTTP/-1.1\r\n"), std::runtime_error);
	EXPECT_THROW(parseHttpVersionTest("HTTP/1.-1\r\n"), std::runtime_error);
	EXPECT_THROW(parseHttpVersionTest("HTTP/1.2\r\n"), std::runtime_error);
	EXPECT_THROW(parseHttpVersionTest("HTTP/2.0\r\n"), std::runtime_error);
	EXPECT_THROW(parseHttpVersionTest("HTPP/1.0\r\n"), std::runtime_error);
	EXPECT_THROW(parseHttpVersionTest("HTP/1.0\r\n"), std::runtime_error);
	EXPECT_THROW(parseHttpVersionTest("HTTP1.0\r\n"), std::runtime_error);
	EXPECT_THROW(parseHttpVersionTest("http/.0\r\n"), std::runtime_error);
}

TEST_F(RequestTest, splitValid) {
	std::vector<std::string>	argv;

	argv.push_back("this");
	argv.push_back("is");
	argv.push_back("a");
	argv.push_back("test");
	EXPECT_EQ(argv, splitTest("this is a test"));
}

TEST_F(RequestTest, splitSingleValid) {
	std::vector<std::string>	argv;

	argv.push_back("thisisatest");
	EXPECT_EQ(argv, splitTest("thisisatest"));
}

TEST_F(RequestTest, splitInvalid) {
	EXPECT_THROW(splitTest(" test"), std::runtime_error);
	EXPECT_THROW(splitTest("test "), std::runtime_error);
	EXPECT_THROW(splitTest("test  test"), std::runtime_error);
	EXPECT_THROW(splitTest("  test  "), std::runtime_error);
	EXPECT_THROW(splitTest(" test test "), std::runtime_error);
	EXPECT_THROW(splitTest("  test  test  test  "), std::runtime_error);
	EXPECT_THROW(splitTest("           test  test  test   "), std::runtime_error);
	EXPECT_THROW(splitTest(" "), std::runtime_error);
	EXPECT_THROW(splitTest("               "), std::runtime_error);
}

uint8_t RequestTest::parseMethodTest(const std::string& arg) {
	request.parseMethod(arg);
	return (request._method);
}

httpVersion_t RequestTest::parseHttpVersionTest(std::string const & arg) {
	request.parseHttpVersion(arg);
	return (request._httpVersion);
}

std::vector<std::string> RequestTest::splitTest(std::string const & str) {
	return (request.split(str));
}