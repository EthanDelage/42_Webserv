/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ethan <ethan@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 16:41:00 by ethan             #+#    #+#             */
/*   Updated: 2023/09/12 16:41:00 by ethan            ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>

# define RED 		"\033[1;31m"
# define GREEN 		"\033[1;32m"
# define YELLOW 	"\033[1;33m"
# define BLUE 		"\033[1;34m"
# define GREY		"\033[1;30m"
# define PURPLE		"\033[1;35m"
# define DEFAULT	"\033[1;37m"

std::vector<std::string>	split_path(std::string& path);
void 						addCRLF(std::string& str);
std::string 				toLower(std::string const & str);
void						printColor(std::ostream& os, std::string const &str, std::string const & color);

#endif
