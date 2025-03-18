#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "utils/ParsedUrl.h"
#include <iostream>

const size_t BUFFER_SIZE = 2000000;

//Fetch the header + HTML of a site at location URL.
//Store information in buffer and modify pagesize accordingly.
int crawl ( ParsedUrl url, char *buffer, size_t &pageSize);

class Crawler

    {
    public:
        int crawl ( ParsedUrl url, char *buffer, size_t &pageSize);

        /*

        bool robots( UrlFrontier &frontier, char *buffer, size_t &pageSize, ParsedUrl url ) {
            if (frontier.robotFound(url.Host))
                return true;
            url.Path = string("/robots.txt");
            crawl(url, buffer, pageSize);
            return frontier.parseRobots(buffer, pageSize, url.Service + "://" + url.Host);
        }

        */

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