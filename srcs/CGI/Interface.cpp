#include "Interface.hpp"

void handle_sigalrm(int signum) {
    (void)signum;
    std::cerr << "[CGI] kill child" << std::endl;
    exit(TIMEOUT);
}

Interface::Interface() {
	_requestMethod = "";
	_serverProtocol = "";
	_queryString = "";
	_script = "";
	_resultCGI = "";
	_contentType = "";
	_contentLength = "";
	_len = 0;
}

Interface::Interface(std::string requestMethod, std::string script, std::string queryString, std::string serverProtocol, int len) : _requestMethod(requestMethod), _script(script), _queryString(queryString), _serverProtocol(serverProtocol), _len(len) {

	std::ostringstream oss;
	oss << _len;
    _contentLength = oss.str();
	_resultCGI = "";
	_contentType = "";
}

Interface::~Interface(){}

int Interface::call(){
	pid_t   child;
	int		pipe_out[2];
	int		pipe_in[2];

	if (pipe(pipe_out) == -1){
		std::cerr << "[CGI] error pipe" << std::endl;
		return false;
	}
    
	if (pipe(pipe_in) == -1){
		std::cerr << "[CGI] error pipe" << std::endl;
		close(pipe_out[0]);
		close(pipe_out[1]);
		return false;
	}

	child = fork();
	if (child == -1){
        std::cerr << "[CGI] error fork" << std::endl;
		close(pipe_out[0]);
		close(pipe_out[1]);
		close(pipe_in[0]);
		close(pipe_in[1]);
        return false;
    }
	
	if (child == 0){
		close(pipe_out[0]);
		close(pipe_in[1]);
		
		if (dup2(pipe_out[1], STDOUT_FILENO) == -1) {
			std::cerr << "[CGI] error dup2" << std::endl;
			close(pipe_out[1]);
			close(pipe_in[0]);
			return false;
		}
		
		if (dup2(pipe_in[0], STDIN_FILENO) == -1) {
			std::cerr << "[CGI] error dup2" << std::endl;
			close(pipe_out[1]);
			close(pipe_in[0]);
			return false;
		}

		close(pipe_out[1]);
		close(pipe_in[0]);

		if (!setEnv())
			exit(EXIT_FAILURE);

		if (access(_args[0], X_OK) != 0) {
			freeptr();
			exit(ERROR_ACCESS);
		}

		execve(_args[0], _args, _envC);
		
		std::cerr << "[CGI] execution failed" << std::endl;
		
		freeptr();

        exit(ERROR_EXEC);
    }
	else {
		close(pipe_out[1]);
		close(pipe_in[0]);
		
		int status = 0;
		int wait = 0;
		int timeout = REQUEST_TOO_LONG;
	
		if (!_queryString.empty()) {

			ssize_t written = write(pipe_in[1], _queryString.c_str(), _queryString.size());
			if (written == -1) {
				std::cerr << "[CGI] error writing to stdin" << std::endl;
			}
		}

		close(pipe_in[1]);

		while (wait < timeout) {
			pid_t rslt = waitpid(child, &status, WNOHANG);
			if (rslt == 0) {
				sleep(1);
				wait++;
			} 
			else {
				break;
			}
		}

		if (wait >= timeout) {
			kill(child, SIGKILL);
			waitpid(child, &status, 0);
			return GATEWAY_TIME_OUT;
		}

		std::string output;
		char buffer[1024];
		ssize_t bytesRead;

		
		while ((bytesRead = read(pipe_out[0], buffer, sizeof(buffer))) > 0) {
			output.append(buffer, bytesRead);
		}
		close(pipe_out[0]);
	    
		if (WIFSIGNALED(status)) {
			int sig = WTERMSIG(status);
			std::cerr << "[CGI] Process killed by signal " << sig << std::endl;
			return INTERNAL_SERVER_ERROR;
		}
		
		if (WIFEXITED(status)) {
			int code = WEXITSTATUS(status);
			if (code == ERROR_EXEC || code == ERROR_ACCESS)
				return NOT_IMPLEMENTED;
			if (code != SUCCESS) {
				return INTERNAL_SERVER_ERROR;
			}
		}

	    _resultCGI = output;
	    return SUCCESS;
	}
}

std::string& Interface::getResultCGI() {
	return _resultCGI;
}

bool Interface::setEnv() {
	
	_envStr.push_back("REQUEST_METHOD=" + _requestMethod);
	_envStr.push_back("QUERY_STRING=" + _queryString);
	_envStr.push_back("SCRIPT_NAME=" + _script);
	_envStr.push_back("SERVER_PROTOCOL=" + _serverProtocol);
	_envStr.push_back("CONTENT_LENGTH=" + _contentLength);

	if (!envVarConvert())
		return false;

	return true;
}

bool Interface::envVarConvert() {
	
    _envC = (char**) malloc(sizeof(char*) * 6);
	if (!_envC) {
		std::cerr << "[CGI] Error : malloc failed" << std::endl;
		return false;
	}

	_args = (char**) malloc(sizeof(char*) * 2);
	if (!_args) {
		std::cerr << "[CGI] Error : malloc failed" << std::endl;
		free(_envC);
		return false;
	}
	size_t i = 0;	
	while (i < _envStr.size())
	{
		_envC[i] = strdup(_envStr[i].c_str());
		i++;
	}
	_envC[i] = 0;

	_args[0] = strdup(_script.c_str());
	_args[1] = 0;
	return true;
}

void Interface::freeptr(){
	for (size_t i = 0; _envC[i] != 0; i++)
		free(_envC[i]);
	free(_envC);

	for (size_t i = 0; _args[i] != 0; i++)
		free(_args[i]);
	free(_args);
}
