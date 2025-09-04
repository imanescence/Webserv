#include "HttpFormIn.hpp"

HttpFormIn::HttpFormIn() : HttpForm(){
    _method = "";
    _uri = "";
    _locationPath = "";
    _path = "";
    _fullfillment = STARTED;
}

HttpFormIn::~HttpFormIn(){}

void HttpFormIn::clean() {
    _protocol = "";
    _startLine = "";
    _body = "";
    _method = "";
    _uri = "";
    _locationPath = "";
    _path = "";
    _fullfillment = STARTED;
}

/**
 *  @brief interpret the request line as _method _uri _protocol, and extract _locationPath and _vars
 *  @return an ERROR code if the request does not have the right format, SUCCESS otherwise
 */
int HttpFormIn::parseRequest() {
    size_t start;
    size_t end;

    start = 0;
    end = _startLine.find(' ');
    if (end == std::string::npos) {
        std::cerr << "[parseRequest] Error: failed to find space after method" << std::endl;
        return BAD_REQUEST;
    }
    _method = _startLine.substr(start, end - start);

    start = end + 1;
    end = _startLine.find(' ', start);
    if (end == std::string::npos) {
        std::cerr << "[parseRequest] Error: failed to find space after path" << std::endl;
        return BAD_REQUEST;
    }
    _uri = _startLine.substr(start, end - start);

    start = end + 1;
    end = _startLine.size();
    _protocol = _startLine.substr(start, end - start);

    start = 0;
    end = _uri.find('?');
    if (end == std::string::npos)
        end = _uri.size();
    _locationPath = _uri.substr(start, end - start);

    if (end == _uri.size())
        return SUCCESS;
    start = end + 1;
    end = _uri.size();
    _queryString = _uri.substr(start, end - start);

    return SUCCESS;
}

int HttpFormIn::parseHeadersFromBlock(const std::string &headersPart)
{
    std::istringstream  stream(headersPart);
    std::string         line;

    while (std::getline(stream, line)){
       if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        size_t  colon = line.find(':');
        if (colon == std::string::npos)
            continue ;
        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 1);

        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        _headers[key] = value;
    }
    return SUCCESS;
}

/**
 *  @brief parse incoming request
 *  @param socket the socket where the information to read come from
 *  @param maxBodySize the maximum number of charater in the body or the request
 *  @return an http ERROR CODE if the request does not have the right format
 * 
 *  CONTINUE if the request is not totally parsed
 * 
 *  OK when the request is totally parsed
 */
int HttpFormIn::readSocket(Socket & socket, size_t maxBodySize)
{
    int status;
    size_t pos;
    int len;
    std::string headersBlock;

    status = socket.fillBuffer(MAX_READ); 
    if (status != SUCCESS)
        return status;
    
    switch (_fullfillment){
        case STARTED:
            pos = socket.findDelimiter("\r\n", MAX_URI_SIZE);
            if (pos < 0)
                return URI_TOO_LONG;
            if (pos == 0)
                return CONTINUE;
            socket.consumeBuffer(pos, 2, _startLine);
            status = parseRequest();
            if (status != SUCCESS)
                return status;
            _fullfillment = REQUEST_READ;
        case REQUEST_READ:
            pos = socket.findDelimiter("\r\n\r\n", MAX_HEADERS_SIZE);
            if (pos < 0)
                return REQUEST_HEADER_FIELDS_TOO_LARGE;
            if (pos == 0)
                return CONTINUE;
            socket.consumeBuffer(pos, 4, headersBlock);
            parseHeadersFromBlock(headersBlock);
            len = getLen();
            if (len == -2)
                return BAD_REQUEST;
            if (len == -1 && _method == "POST")
                return LENGTH_REQUIRED;
            if (len == -1)
                return OK;
            if ((size_t)len > maxBodySize){
                return PAYLOAD_TOO_LARGE;
            }
            _fullfillment = HEADERS_READ;
        case HEADERS_READ:
            if (socket.getSizeBuffer() > (size_t)getLen())
                return BAD_REQUEST;
            if (socket.getSizeBuffer() < (size_t)getLen())
                return CONTINUE;
            socket.consumeBuffer(socket.getSizeBuffer(), 0, _body);
            _fullfillment = BODY_READ;
        case BODY_READ:
            return OK;
    }

    return UNDEFINED_ERROR;
}

const std::string & HttpFormIn::getMethod() const{
    return _method;
}

const std::string & HttpFormIn::getUri() const{
    return _uri;
}

const std::string & HttpFormIn::getPath() const{
    return _path;
}

const std::string & HttpFormIn::getLocationPath() const{
    return _locationPath;
}

const std::string & HttpFormIn::getQueryString() const {
    return _queryString;
}

void HttpFormIn::setPath(const std::string & path) {
    _path = path;
}

std::ostream & operator<<(std::ostream & COUT, const HttpFormIn & httpform) {

    int printBody = true;

	COUT << "[HttpFormIn]" << std::endl;
    COUT << "    Method:       " << httpform.getMethod() << std::endl;
    COUT << "    URI:          " << httpform.getUri() << std::endl;
    COUT << "    Protocol:     " << httpform.getProtocol() << std::endl;
    COUT << "    Location:     " << httpform.getLocationPath() << std::endl;
    COUT << "    Path:         " << httpform.getPath() << std::endl;
    COUT << "    Query string: " << httpform.getQueryString() << std::endl;

    COUT << "[HEADERS]" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = httpform.getHeaders().begin(); it != httpform.getHeaders().end(); it++){
        COUT << "    " << it->first << ": " << it->second << std::endl;
    }

    COUT << "[BODY]";
    if (printBody)
        COUT << std::endl << httpform.getBody() << std::endl;
    else
        COUT << " change value of printBody to see body" << std::endl;

    return COUT;
}
