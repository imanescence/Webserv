#pragma once
#include "HttpForm.hpp"

class HttpFormOut : public HttpForm
{
    private:
        int                                 _code;
        std::string                         _status;

    public:
        HttpFormOut();
        HttpFormOut(int error, const std::map<int, std::string> & errors);
        HttpFormOut(std::string contentType, const std::string & body);
	    HttpFormOut(std::string contentType, const std::string & body, int code, const std::string& redirectPath);
	    ~HttpFormOut();

        std::string         toString();
        const std::string   read_error_file(const std::map<int, std::string> & errors);
        void                clean();

        //accessors
        const std::string & getStatus() const;
        int getCode() const;
};

std::ostream & operator<<(std::ostream & COUT,  const HttpFormOut & httpform);
