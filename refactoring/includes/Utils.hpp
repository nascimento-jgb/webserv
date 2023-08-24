/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 09:21:24 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/22 10:13:33 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <sstream>

//Utilss
int				ft_stoi(std::string str);
std::string		ft_itoa(int integer);
unsigned int	fromHexToDec(const std::string& nb);
void			to_lower(std::string &str);
