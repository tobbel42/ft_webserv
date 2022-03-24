/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrossma <tgrossma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/12 11:57:15 by skienzle          #+#    #+#             */
/*   Updated: 2022/03/24 15:15:44 by tgrossma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Engine.hpp"






int main(int argc, char **argv)
{
	//Server(INADDR_ANY, 8080);
	Engine	a;
	a.initServers();
	a.launch();
	return 0;
}