#pragma once
#include "HttpForm.hpp"

class HttpFormIn : public HttpForm
{
    private:
        std::string _method;
        std::string _uri;
        std::string _locationPath;
        std::string _path;
        std::string _queryString;
        int         _fullfillment;

        //parsing
        int         parseRequest();
        int         parseHeadersFromBlock(const std::string &headersPart);

    public:
        HttpFormIn();
        ~HttpFormIn();

        int     readSocket(Socket & socket, size_t maxBodySize);
        void    clean();

        //accessors
        const std::string & getMethod() const;
        const std::string & getUri() const;
        const std::string & getPath() const;
        const std::string & getLocationPath() const;
        const std::string & getQueryString() const;
        void setPath(const std::string & path);
};

std::ostream & operator<<(std::ostream & COUT,  const HttpFormIn & httpform);
