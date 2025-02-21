#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include "frontier.h"
#include <iostream>

int crawl ( ParsedUrl url, char *buffer, size_t &pageSize)
   {
   int returnCode = 0;
   bool headerEnded = false;
   
   //GET Message construction
   std::string path = url.Path;
   if (path[0] != '/')
      path = "/" + path;
   
   std::string getMessage =
      "GET " + path + " HTTP/1.1\r\n"
      "Host: " + std::string(url.Host) + "\r\n"
      "User-Agent: LinuxGetSsl/2.0 codefather@umich.edu (Linux)\r\n"
      "Accept: */*\r\n"
      "Accept-Encoding: identity\r\n"
      "Connection: close\r\n\r\n";

   // Get the host address.
   struct addrinfo *address, hints;
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;
   
   if (getaddrinfo(url.Host.c_str(), "443", &hints, &address) < 0) {
      std::cerr << "Address lookup failed." << std::endl;
      exit(1);
   }

   int sd = 0;
   for (struct addrinfo *addr = address; addr != NULL; addr = addr->ai_next) {
      sd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

      if (sd == -1) break;
      if (connect(sd, addr->ai_addr, addr->ai_addrlen) == 0) break;

      close(sd);
      sd = -1;
   }
   freeaddrinfo(address);

   if (sd == -1) {
      std::cerr << "Couldn't connect to host." << std::endl;
      return 1;
   }
   
   // Build an SSL layer and set it to read/write
   // to the socket we've connected.
   //always returns 1

   OpenSSL_add_all_algorithms();
   SSL_load_error_strings();
   SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());

   if (ctx == NULL) {
      fprintf(stderr, "ERROR: could not initialize the SSL context: %s\n",
               strerror(errno));
      return 1;
   }

   SSL *ssl = SSL_new(ctx);
   SSL_set_fd(ssl, sd);

   if (SSL_connect(ssl) != 1) {
      std::cerr << "SSL connection failed." << std::endl;
      std::cerr << "Error: " << errno << std::endl;
      returnCode = 1;
      goto Cleanup; 
   }
   if (SSL_write( ssl, getMessage.c_str(), getMessage.length() ) <= 0 )
      {
      std::cerr << "Failed to write over SSL" << std::endl;
      returnCode = 1;
      goto Cleanup;
      }
   int bytes, oldcount;
   oldcount = 0;
   pageSize = 0;
   while ((bytes = SSL_read(ssl, buffer + pageSize, sizeof(buffer))) > 0) {
      pageSize += bytes;
      // for debug 
      for (int i = oldcount; i < pageSize; i++) {
         std::cout << buffer[i];
      }
      oldcount += bytes;
   }

   // Close the socket and free the address info structure.
   Cleanup:
   SSL_shutdown( ssl );
   SSL_free( ssl );
   close(sd);
   return returnCode;
   }
