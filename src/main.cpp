/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrossma <tgrossma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/12 11:57:15 by skienzle          #+#    #+#             */
/*   Updated: 2022/04/06 13:06:44 by tgrossma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "Engine.hpp"






int main(int argc, char **argv)
{
	Engine	a;
	a.initServers();
	a.initSockets();
	a.launch();
	return 0;
}