#include "Interface.hpp"

void handle_sigalrm(int signum) {
    (void)signum;
    std::cerr << "kill child" << std::endl;
    exit(0); //code erreur ?
}

Interface::Interface() {
	_requestMethod = "";
	_serverProtocol = "";
	_queryString = "";
	_script = "";
	_pathInfo = "";
	_resultCGI = "";
}

Interface::Interface(std::string requestMethod, std::string serverProtocol, std::string queryString, std::string script, std::string pathInfo):  _requestMethod(requestMethod), _serverProtocol(serverProtocol), _queryString(queryString), _script(script), _pathInfo(pathInfo) {
	_resultCGI = "";
}


Interface::~Interface(){}

bool Interface::call(){
    pid_t   child;
	int		pipefd[2];

	if (pipe(pipefd) == -1){
		std::cerr << "error pipe" << std::endl;
		return false;
	}
    child = fork();
    if (child == -1){
        std::cerr << "error fork" << std::endl;
        return false;
    }
    if (child == 0){
		close(pipefd[0]);
		if (dup2(pipefd[1], 1) == -1){
			std::cerr << "error dup2" << std::endl;
			return false;
		}
        signal(SIGALRM, handle_sigalrm);
        alarm(2);
		if (!setEnv())
			exit(EXIT_FAILURE);
        execve(_args[0], _args, _envC);
        std::cout << "\n\n\nnope :(" << std::endl;
		free(_envC);
		free(_args);
        exit(8);
    }
	else{
		int len = 10;
		char * res = (char*)malloc(sizeof(char) * len);
		memset(res, 0, len);
		close(pipefd[1]);
		read(pipefd[0], res, len);
		close(pipefd[0]);
		waitpid(child, 0, 0);
		_resultCGI.assign(res);
		free(res);
	}
    return true;
}

std::string& Interface::getResultCGI() {

	return _resultCGI;
}

bool Interface::setEnv() {
	
	_envStr.push_back("REQUEST_METHOD=" + _requestMethod);
	_envStr.push_back("QUERY_STRING=" + _queryString);
	_envStr.push_back("SCRIPT_NAME=" + _script);
	_envStr.push_back("SERVER_PROTOCOL=" + _serverProtocol);
	_envStr.push_back("PATH_INFO=" + _pathInfo);

	if (!envVarConvert())
		return false;
	
	return true;
}

bool Interface::envVarConvert() {
	
        _envC = (char**) malloc(sizeof(char*) * 6);
	if (!_envC) {
		std::cerr << "Error : malloc failed" << std::endl;
		return false;
	}
        
	_args = (char**) malloc(sizeof(char*) * 2);
	if (!_args) {
		std::cerr << "Error : malloc failed" << std::endl;
		free(_envC);
		return false;
	}
	
	size_t i;
	for(i = 0; i < _envStr.size(); ++i) {
		_envC[i] = (char *)_envStr[i].c_str();
	}
	_envC[i] = NULL;

	_args[0] = (char *)_script.c_str();
	_args[1] = NULL;
	return true;
}
