#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../utils/ParsedUrl.h"
#include <iostream>

const size_t BUFFER_SIZE = 1000000;

class Crawler

    {
    public:
        void crawl ( ParsedUrl url, char *buffer, size_t &pageSize);
        Crawler();
        ~Crawler();

        /*

        bool robots( UrlFrontier &frontier, char *buffer, size_t &pageSize, ParsedUrl url ) {
            if (frontier.robotFound(url.Host))
                return true;
            url.Path = string("/robots.txt");
            crawl(url, buffer, pageSize);
            return frontier.parseRobots(buffer, pageSize, url.Service + "://" + url.Host);
        }

        */

    private:
        SSL_CTX *globalCtx; //global ssl context

};



class Connection {
    public: 

        Connection();
        Connection(SSL_CTX * ctx, const string hostname);
        void freeSSL();
        ~Connection();

    private: 
        string hostname;
        SSL *ssl;
        int sd;
        struct addrinfo *address, hints;

        friend class Crawler;
};