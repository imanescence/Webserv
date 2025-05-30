#pragma once
#include "../../inc/Libraries.h"
#include "../../inc/define.h"
#include "../HTTP/HttpRequest.hpp"

class HttpRequest;

class Interface
{
    private:
	std::string _requestMethod;
	std::string _serverProtocol;
	std::string _queryString;
	std::string _script;
	std::string _pathInfo;
	std::string _resultCGI;

	std::vector<std::string> _envStr;
	char **_envC;
	char **_args;
	
	bool		setEnv();
	bool		envVarConvert();

    public:
	    Interface();
	    Interface(std::string _queryString, std::string _serverProtocol, std::string _requestMethod, std::string _script, std::string _pathInfo);
	    ~Interface();

	    bool call();
	    std::string	&getResultCGI();
};
