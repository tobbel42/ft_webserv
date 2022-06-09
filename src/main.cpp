#include "Socket.hpp"
#include "Engine.hpp"
#include "Config_parser.hpp"

// #include <cstring>

bool	strriseq(const char *s1, const char *s2)
{
	if (s1 == NULL && s2 == NULL)
		return (false);
	if ((s1 == NULL && s2 != NULL) || (s1 != NULL && s2 == NULL))
		return (false);
	int i_s1 = strlen(s1);
	int i_s2 = strlen(s2);
	for (; i_s1 >= 0 && i_s2 >= 0 && s1[i_s1] == s2[i_s2]; --i_s1, --i_s2);
	return i_s2 == -1;
}


int main(int argc, char **argv)
{
	Engine	a;
	//atm müssen die server vor den Sockets initialisiert werden, sonst segfault
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
	a.initServers();
	a.initSockets();
	a.launch();
	return 0;
}