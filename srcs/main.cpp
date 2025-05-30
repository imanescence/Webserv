#include "../inc/Libraries.h"
#include "configuration/Parsing.hpp"
#include "ServerHandler.hpp"
#include "configuration/Server.hpp"
#include "configuration/Config.hpp"


int main (int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN); // ignorer le signal de broken pipe pour pas kill el programme

    if (argc > 2)
    {
        std::cerr << "error: too many arguments" << std::endl;
        return -1;
    }
    
    if (argc == 2)
    {
	    std::string fileName = argv[1];

	    if (fileName.substr(fileName.length() - 5) != ".conf") {
		    std::cerr << "Error : the file must be a configuration file NGNIX style" << std::endl;
		    return 1;
	    }
	    else {
		    Config config; 
		    Parsing parsing;
		    parsing.parseFile(fileName, config);
		    //std::cout << config;
		    ServerHandler handler(&config);
		    handler.serverStarter();
	    }
    }
    else if (argc == 1)
    {
	    Config defConf("By default if the conf file is not provided");
        ServerHandler handler(&defConf);
        handler.serverStarter();
    }
    return 0;
}
