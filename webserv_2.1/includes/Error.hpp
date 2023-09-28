/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/25 16:40:11 by leklund           #+#    #+#             */
/*   Updated: 2023/09/28 17:05:19 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_HPP
# define ERROR_HPP

# include "Webserver.hpp"

class Error
{
	private:
		std::map<int, std::string> _errors;

		Error(Error const &rhs);

		Error	&operator=(Error const &rhs);

	public:
		Error();
		~Error();

		std::string getErrorMsg(int errorValue);
};

#endif



