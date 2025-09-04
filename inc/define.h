#pragma once

//error codes
#define SUCCESS 0
#define UNDEFINED_ERROR -1
#define TIMEOUT 1
#define CLIENT_LEFT 2
#define FILE_CLOSED 3
#define RECV_ERROR 5
#define SEND_ERROR 6
#define WRITE_ERROR 10
#define PARTIAL_SEND 7
#define ERROR_EXEC 8
#define ERROR_ACCESS 19
#define ERROR_SOCKET 11
#define ERROR_SETSOCKOPT 12
#define ERROR_BIND 13
#define ERROR_LISTEN 14
#define ERROR_ACCEPT 15
#define REMOVE_CLIENT 30

//fullfillment of socket's reading
#define STARTED 0
#define REQUEST_READ 1
#define HEADERS_READ 2
#define BODY_READ 3

//HTTP status codes
#define CONTINUE 100
#define OK 200
#define BAD_REQUEST 400
#define PAYMENT_REQUIRED 402
#define FORBIDDEN 403
#define NOT_FOUND 404
#define METHOD_NOT_ALLOWED 405
#define LENGTH_REQUIRED 411
#define PAYLOAD_TOO_LARGE 413
#define URI_TOO_LONG 414
#define METHOD_FAILURE 424
#define REQUEST_HEADER_FIELDS_TOO_LARGE 431
#define INTERNAL_SERVER_ERROR 500
#define NOT_IMPLEMENTED 501 
#define BAD_GATEWAY 502
#define GATEWAY_TIME_OUT 504

//max sizes to read (protection)
#define MAX_READ 512
#define MAX_URI_SIZE 4096
#define MAX_HEADERS_SIZE 4096

//define how many seconds is too long for a request
#define REQUEST_TOO_LONG 2
