#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "frontier.h"
#include <iostream>

//Fetch the header + HTML of a site at location URL.
//Store information in buffer and modify pagesize accordingly.
int crawl ( ParsedUrl url, char *buffer, size_t &pageSize);

class Crawler

    {
    public:
        int crawl ( ParsedUrl url, char *buffer, size_t &pageSize);

        Crawler();

    private:
        int setupConnection(string hostName);
        bool verifySSL();
        void freeSSL();
        struct addrinfo *address, hints;
        string currentHost;
        SSL *ssl; //ssl socket
        int sd; //socket descriptor
    };