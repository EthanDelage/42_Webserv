/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hferraud <hferraud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 14:02:00 by hferraud          #+#    #+#             */
/*   Updated: 2023/09/19 14:02:00 by hferraud         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef ERROR_HPP
# define ERROR_HPP

# include <exception>

class clientException : public std::exception {};
class serverException : public std::exception {};
class headerException : public std::exception {};

#endif