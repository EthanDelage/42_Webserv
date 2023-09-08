/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 16:11:00 by edelage           #+#    #+#             */
/*   Updated: 2023/08/17 16:11:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <string>

typedef struct httpVersion_s	httpVersion_t;
struct httpVersion_s {
	unsigned int	major;
	unsigned int	minor;
};

class Message {

# ifdef UNIT_TESTING
	friend class RequestTest;
# endif

protected:
	httpVersion_t	_httpVersion;

public:
	Message();
	~Message();

};

#endif