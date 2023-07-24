/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 21:04:00 by edelage           #+#    #+#             */
/*   Updated: 2023/07/06 21:04:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include <gtest/gtest.h>
#include "Config/ConfigTest.hpp"

int	main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
}