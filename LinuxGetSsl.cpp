#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include "frontier.h"
#include <iostream>

int verify_callback(int preverify, X509_STORE_CTX* x509_ctx)
{
    int depth = X509_STORE_CTX_get_error_depth(x509_ctx);
    int err = X509_STORE_CTX_get_error(x509_ctx);
    
    X509* cert = X509_STORE_CTX_get_current_cert(x509_ctx);
    X509_NAME* iname = cert ? X509_get_issuer_name(cert) : NULL;
    X509_NAME* sname = cert ? X509_get_subject_name(cert) : NULL;
    
    std::cout << "Issuer: " << iname << std::endl;
    std::cout << "Subject: " << sname << std::endl;
    
    if(depth == 0) {
        /* If depth is 0, its the server's certificate. Print the SANs too */
       std::cout << "Subject (SAN): " << cert << std::endl;
    }

    return preverify;
}

int crawl ( ParsedUrl url )
   {
   int returnCode = 0;
   bool headerEnded = false;
   //SSL initialization
   SSL_library_init();
   SSL_CTX *ctx = SSL_CTX_new( TLS_client_method() );
  
   if ( !ctx )
         {
         std::cerr << "CTX creation failed." << std::endl;
         return 1;  
         }
   
   SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
   SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);
   SSL_CTX_set_verify_depth(ctx, 4);

   if (SSL_CTX_set_default_verify_paths(ctx) != 1) 
      {
      std::cerr << "Error loading trust store";
      SSL_CTX_free( ctx );
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
   
   int status = getaddrinfo( url.Host.c_str(), "443", &hints, &address );
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
      std::cerr << "Error: " << errno << std::endl;
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
   if (SSL_write( ssl, getMessage.c_str(), getMessage.length() ) <= 0 )
      {
      std::cerr << "Failed to write over SSL" << std::endl;
      returnCode = 1;
      goto Cleanup;
      }
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


int main(int argc, char* argv[]) {
   if (argc != 2) {
      perror("Usage: ./LinuxGetSsl [url]");
      return 1;
   }
   std::string url = argv[1];
   crawl(ParsedUrl(url));
   return 0;
}