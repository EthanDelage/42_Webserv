/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 15:13:00 by edelage           #+#    #+#             */
/*   Updated: 2023/09/08 15:13:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "message/Request.hpp"

class Response {
	typedef void (Response::*responseFunction_t)();

private:
	Request	_request;

	void router();
	void getResponse();
	void postResponse();
	void deleteResponse();

	std::string	httpVersionToString() const;
	static std::string uitoa(unsigned int n);

public:
	Response(Request request);
	~Response();

};

#endif