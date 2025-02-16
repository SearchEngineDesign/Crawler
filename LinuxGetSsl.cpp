#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <frontier.h>
#include <iostream>

int crawl ( ParsedUrl url )
   {
   int returnCode = 0;
   bool headerEnded = false;
   //SSL initialization
   SSL_library_init();
   SSL_CTX *ctx = SSL_CTX_new( SSLv23_method() );
   if ( !ctx )
      {
      std::cerr << "CTX creation failed." << std::endl;
      return 1;  
      }
   SSL *ssl = SSL_new( ctx );
   if ( !ssl )
      {
      std::cerr << "SSL creation failed." << std::endl;
      SSL_CTX_free( ctx );
      return 1;  
      }
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
   
   int status = getaddrinfo( url.Host.c_str(), "80", &hints, &address );
   if ( status != 0 ) 
      {
      std::cerr << "address lookup failed." << std::endl;
      return 1;
      }
   // Create a TCP/IP socket.
   int socketfd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
   if ( socketfd == -1 ) 
      {
      freeaddrinfo( address );
      return 1;
      }


   // Connect the socket to the host address.
   if ( connect( socketfd, address->ai_addr, address->ai_addrlen ) == -1 ) 
      {
      std::cerr << "Socket connection failed." << std::endl;
      returnCode = 1;
      goto Cleanup;
      }
   
   // Build an SSL layer and set it to read/write
   // to the socket we've connected.
   //always returns 1

   SSL_set_fd( ssl, socketfd);
   if (SSL_connect( ssl ) != 1)
      {
      std::cerr << "SSL connection failed." << std::endl;
      returnCode = 1;
      goto Cleanup; 
      }
   send(socketfd, getMessage.c_str(), getMessage.length(), 0);
   char buffer[10240];
   int bytes;
   while ((bytes = SSL_read(ssl, buffer, sizeof(buffer))) > 0) {
      char* response = buffer;
      
      // Skip headers
      if (!headerEnded) {
         char* bodyStart = strstr(buffer, "\r\n\r\n");
         if (bodyStart) {
               response = bodyStart + 4;  // Skip "\r\n\r\n"
               headerEnded = true;
         } else {
               continue;  // Headers are incomplete, keep reading
         }
      }
   
      SSL_write(ssl, response, bytes - (response - buffer));
   }

   // Close the socket and free the address info structure.
   Cleanup:
   SSL_shutdown( ssl );
   SSL_free( ssl );
   close(socketfd);
   freeaddrinfo(address);
   return returnCode;
   }
