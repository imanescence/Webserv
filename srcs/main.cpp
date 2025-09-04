#include "../inc/Libraries.h"
#include "NGINX/Parsing.hpp"
#include "ServerHandler.hpp"
#include "NGINX/ServerSettings.hpp"
#include "NGINX/Config.hpp"
#include <fcntl.h>
#include <time.h>

void setup_logs(bool redirectSTDOUT, bool redirectSTDERR, int *log, int *errlog){
	if (redirectSTDERR){
		time_t t = time(0);
		struct tm tm = *localtime(&t);
		int date = tm.tm_year - 100;
		date = date * 100 + tm.tm_mon + 1;
		date = date * 100 + tm.tm_mday;
		std::stringstream logfilename;
		const char* user = std::getenv("USER");
		logfilename << "www/log/" << date << "_" << user << "_" << "error.log";
		*errlog = open(logfilename.str().c_str(), O_CREAT | O_RDWR | O_APPEND, 0644);
		if (*errlog == -1)
			std::cout << "[INIT] errors will be displayed in terminal" << std::endl;
		else if (dup2(*errlog, 2) == -1)
			std::cerr << "[INIT] log/error.log initialized but dup2 failed" << std::endl;
		else{
			std::cout << "[INIT] errors will be stored in log/error.log" << std::endl;
			std::cerr << "------------ ...";
			if (tm.tm_hour < 10)
				std::cerr << "0";
			std::cerr << tm.tm_hour << ":";
			if (tm.tm_min < 10)
				std::cerr << "0";
			std::cerr << tm.tm_min << ":";
			if (tm.tm_sec < 10)
				std::cerr << "0";
			std::cerr << tm.tm_sec << "... ------------" << std::endl;
		}
	}

	if (redirectSTDOUT){
		*log = open("www/log/stdout.log", O_CREAT | O_RDWR | O_TRUNC, 0644);
		if (*log == -1)
			std::cout << "[INIT] stdout will be displayed in terminal" << std::endl;
		else
			std::cout << "[INIT] stdout will be stored in log/stdout.log" << std::endl;
		if (dup2(*log, 1) == -1)
			std::cerr << "[INIT] log/stdout.log initialized but dup2 failed" << std::endl;
	}
}

int main (int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);

    if (argc > 2)
    {
        std::cerr << "[INIT] error: too many arguments" << std::endl;
        return -1;
    }

	int log = -1;
	int errlog = -1;
	setup_logs(true, true, &log, &errlog);

    if (argc == 2)
    {
	    std::string fileName = argv[1];

	    if (fileName.substr(fileName.length() - 5) != ".conf") {
		    std::cerr << "[PARSING] Error : the file must be a configuration file NGNIX style" << std::endl;
		    return 1;
	    }
	    else {
		    Config config; 
		    Parsing parsing;
		    parsing.parseFile(fileName, config);
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

	if (log != -1)
		close(log);
	if (errlog != -1)
		close(errlog);
    return 0;
}
