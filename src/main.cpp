/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skienzle <skienzle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/12 11:57:15 by skienzle          #+#    #+#             */
/*   Updated: 2022/03/18 10:29:25 by skienzle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"







int main(int argc, char **argv)
{
	Server(INADDR_ANY, 8080);
	return 0;
}