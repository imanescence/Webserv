#pragma once
#include "../../inc/Libraries.h"
#include "../configuration/Server.hpp"
#include "gnl.hpp"
#include "../CGI/Interface.hpp"

class HttpRequest
{
    private:
        int                                 _fd;
        ssize_t                             _len;
        Server                              _config;
        std::map<std::string, std::string>  _headers;
        std::string                         _method;
        Location                            _location;
        std::string                         _path;
        std::string                         _protocol;
        std::string                         _content;
        std::map<std::string, std::string>  _formFields;
        std::string                         _uploadedFileName;
        std::string                         _uploadedFileContent;
	    std::vector<std::string> 	        _dirList;
        std::string                         _error;

        void        callCGI(void);
        void        handleGet(void);
        void        handlePost(void);
        void        handleDelete(void);
        bool        parseMultipartFormData(const std::string& boundary);
        std::string extractFileName(const std::string& contentDisposition);
        std::string generateUniqueFileName(const std::string& originalFilename);
        void        saveUploaderFile(const std::string& filePath, const std::string& fileContent);
        void        send404(void);
        void        send400(void);
        void        sendReponse(int _fd, const std::string& status, const std::string& contentType, const std::string& body);
        
        //parsing
        int         parseRequest(char *msg);
        int         parseHeader(char *msg);
        int         parseHeaders();
        bool        parseBody();
        int         find_location(std::string request_uri);
        std::string parseScript();
        std::string parseQuery();

        bool        isValidLength();
    
    public:
        HttpRequest();
        HttpRequest(int fd, Server config);
        ~HttpRequest(void);

        //assessors
        std::string get_method();
        std::string get_path();
        std::string get_protocol();

	    // methods to check files and paths	
	    bool isValidPath(const char *path);
	    bool isValidFile(const char *file);
	    bool isAnExecutable(const char *exec);

	void displayDirList(std::string dirPath);
	void extractDirList(std::string dirPath);

        class ParsingException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
        class MaxBodySizeException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
        class RequestException : public std::exception
        {
            private:
                std::string _msg;
            public:
                RequestException(std::string & msg);
                virtual ~RequestException() throw();
                virtual const char* what() const throw();
        };
};
