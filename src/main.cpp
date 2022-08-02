#include "ConfigParser.hpp"
#include "Socket.hpp"
#include "Engine.hpp"

int main(int argc, char **argv, char **)
{
	if (argc > 2)
	{
		std::cerr << "Error:\nwrong number of arguments\nusage: "
					<< argv[0] << " <config file>" << std::endl;
		return 1;
	}

	Engine	e;
	ConfigParser parser(e.get_servers());
	
	try
	{
		if (argc == 1)
			parser.assign_file("config/default.conf");
		else
			parser.assign_file(argv[1]);
		parser.run();

	//atm müssen die server vor den Sockets initialisiert werden, sonst segfault

		if (e.init_sockets() == false)
			return EXIT_FAILURE;
		if (e.launch() == false)
			return EXIT_FAILURE;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
	return EXIT_SUCCESS;
}
