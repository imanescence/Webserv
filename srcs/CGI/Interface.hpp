#pragma once
#include "../HTTP/HttpForm.hpp"

class HttpForm;

class Interface
{
    private:
	std::string _requestMethod;
	std::string _script;
	std::string _queryString;
	std::string _serverProtocol;
	std::string _resultCGI;
	std::string _contentLength;
	std::string _contentType;

	size_t	_len;

	std::vector<std::string> _envStr;
	char **_envC;
	char **_args;
	
	bool		setEnv();
	bool		envVarConvert();
	void		freeptr();

    public:
	    Interface();
	    Interface(std::string requestMethod, std::string script, std::string queryString, std::string serverProtocol, int len);
	    ~Interface();

	    int call();
	    std::string	&getResultCGI();
};
