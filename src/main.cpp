/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skienzle <skienzle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/12 11:57:15 by skienzle          #+#    #+#             */
/*   Updated: 2022/04/16 09:50:10 by skienzle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Engine.hpp"
#include "Config_parser.hpp"

// #include <cstring>

bool	strriseq(const char *s1, const char *s2)
{
	int	i_s1;
	int	i_s2;

	if (s1 == NULL && s2 == NULL)
		return (false);
	if ((s1 == NULL && s2 != NULL) || (s1 != NULL && s2 == NULL))
		return (false);
	i_s1 = strlen(s1);
	i_s2 = strlen(s2);
	for (; i_s1 >= 0 && i_s2 >= 0 && s1[i_s1] == s2[i_s2]; --i_s1, --i_s2);
	return i_s2 == -1;
}


int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Error:\nwrong number of arguments\nusage: "
					<< argv[0] << " <config file>" << std::endl;
		return 1;
	}
	else if (!strriseq(argv[1], ".conf"))
	{
		std::cerr << "Error:\nwrong file extension\n"
				<< "the config file has to end in .conf" << std::endl;
		return 1;
	}
	Config_parser parser(argv[1]);
	try
	{
		parser.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
	catch (int& i) {}
	// Engine	a;
	// a.initServers();
	// a.launch();
	return 0;
}