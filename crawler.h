#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>


#include <cf/ParsedUrl.h>
#include <cf/searchstring.h>


const size_t BUFFER_SIZE = 10000000;



class Crawler

    {
    public:
        void crawl ( ParsedUrl url, char *buffer, size_t &pageSize);
        Crawler();
        ~Crawler();

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