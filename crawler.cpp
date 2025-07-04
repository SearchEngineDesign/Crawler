// crawler.cpp

#include "crawler.h"
#include <memory>




Crawler::Crawler() {
   globalCtx = SSL_CTX_new(TLS_client_method());
   if (globalCtx == nullptr) {
      std::cerr << "Couldn't initialize global SSL context." << std::endl;
      exit(1);
   }
}


Crawler::~Crawler() {

   // This should never execute!
   // but it will if the program shuts down
   SSL_CTX_free(globalCtx);
   globalCtx = nullptr;
}

void Connection::freeSSL() {

   if (ssl != nullptr) {
      
      try {
         SSL_shutdown(ssl);
      } catch (const std::exception &e) {
         std::cerr << "SSL shutdown failed: " << hostname << e.what() << std::endl;
      }

      if (SSL_shutdown(ssl) <= 0) {
          // If the first call to SSL_shutdown returns 0, send a TCP FIN and call SSL_shutdown again
          shutdown(sd, SHUT_RDWR);
          SSL_shutdown(ssl);
      }
      close(sd);
      SSL_free(ssl);
      ssl = nullptr;
   }

   hostname = "";
}

void Crawler::crawl ( ParsedUrl url, char *buffer, size_t &pageSize)
   {

   // Crawler c = Crawler();
   std::unique_ptr<Connection> c = nullptr;


   int bytes;
   string path = url.Path;
   
   const char* route = url.Host.c_str();

   try {
      c = std::make_unique<Connection>(globalCtx, url.Host);      
   } catch (const std::runtime_error &e) {
      std::cerr << "url: | " << url.urlName << std::endl;
      throw;
   }
   // 

   // GET Message construction
   if (*path.at(0) != '/')
      path = string("/") + path;
   
   string getMessage =
      string("GET ") + path + " HTTP/1.1\r\n"
      "Host: " + route + "\r\n"
      "User-Agent: LinuxGetSsl/2.0 codefather@umich.edu (Linux)\r\n"
      "Accept: */*\r\n"
      "Accept-Encoding: identity\r\n"
      "Connection: close\r\n\r\n";

   if (SSL_write( c->ssl, getMessage.c_str(), getMessage.length() ) <= 0 ) {
      std::cerr << "Failed to write over SSL" << std::endl;
      c->freeSSL();
      throw std::runtime_error("Failed to write over SSL.");
   }
   string nstr = string(url.urlName + "\n");
   memcpy(buffer, nstr.c_str(), nstr.length());
   pageSize = nstr.length();
   while ((bytes = SSL_read(c->ssl, buffer + pageSize, 64)) > 0 
           && pageSize < BUFFER_SIZE - 128) {
      pageSize += bytes;
   }
   if (bytes < 0)
      throw std::runtime_error("SSL read failed.");
   }


Connection::Connection() {

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;


   sd = 0;
   ssl = nullptr;
   hostname = "";
}

Connection::Connection(SSL_CTX * ctx, const string hostname_in):  hostname(hostname_in) {


   if (ctx == nullptr) {
      std:: cerr << "Couldn't initialize SSL context for host: \"" <<  hostname << "\"" << std::endl;
      std:: cerr << "global ctx is nullptr" << std::endl;
      throw std::runtime_error("global ssl context is null.");
   }

   ssl = SSL_new(ctx);

   if (!ssl) {
      std::cerr << "SSL initialization failed:" << hostname << std::endl;
      throw std::runtime_error("SSL initialization failed");         
   }
   SSL_set_tlsext_host_name(ssl, hostname.c_str());

   // inefficient?
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;

   int addresult = getaddrinfo(hostname.c_str(), "443", &hints, &address);
   if (addresult != 0) {
      throw std::runtime_error(gai_strerror(addresult));
   }

   for (struct addrinfo *addr = address; addr != NULL; addr = addr->ai_next) {
      sd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

      if (sd == -1) break;
      if (connect(sd, addr->ai_addr, addr->ai_addrlen) == 0) break;

      sd = -1;
   }
   freeaddrinfo(address);

   if (sd == -1) {
      throw std::runtime_error("Couldn't connect to host.");
   }
   
   
   SSL_set_fd(ssl, sd);

   int connect = SSL_connect(ssl);
   if (connect != 1) {
      std::cerr << "SSL connection failed:" << hostname << std::endl;
      freeSSL();
      throw std::runtime_error("SSL connection failed.");
   }
   // return 0;

}

Connection::~Connection() {
   freeSSL();
}
