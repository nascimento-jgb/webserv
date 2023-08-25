/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/25 16:40:11 by leklund           #+#    #+#             */
/*   Updated: 2023/08/22 09:34:02 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_HPP
# define ERROR_HPP

# include "Webserver.hpp"

class Error
{
	private:
		std::map<int, std::string> _errors;

	public:
		Error();
		~Error();

		std::string getErrorMsg(int error_value);
};

#endif



