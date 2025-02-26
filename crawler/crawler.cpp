// crawler.cpp

#include "crawler.h"

int crawl ( ParsedUrl url, char *buffer, size_t &pageSize)
   {
   int returnCode = 0;
   int bytes;
   std::string concatStr = (url.Service + "://" + url.Host + url.Path + '\n');
   
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

   SSL_library_init();
   OpenSSL_add_all_algorithms();
   SSL_load_error_strings();
   SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());

   if (ctx == NULL) {
      std::cerr << "Couldn't initialize SSL context." << std::endl;
      return 1;
   }

   SSL *ssl = SSL_new(ctx);
   if (!ssl) {
      std::cerr << "SSL initialization failed." << std::endl;
      return 1;
   }
   SSL_set_tlsext_host_name(ssl, url.Host.c_str());
   
   SSL_set_fd(ssl, sd);

   int connect = SSL_connect(ssl);
   if (connect != 1) {
      std::cerr << "SSL connection failed." << std::endl;
      int err = SSL_get_error(ssl, connect);
      if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
      } else {
         char error_buffer[256];
         ERR_error_string_n(ERR_get_error(), error_buffer, sizeof(error_buffer));
         std::cerr << "SSL_connect failed with error code: " << err << std::endl;
         std::cerr << "Error string: " << error_buffer << std::endl;
      }
      returnCode = 1;
      goto Cleanup; 
   }
   if (SSL_write( ssl, getMessage.c_str(), getMessage.length() ) <= 0 )
      {
      std::cerr << "Failed to write over SSL" << std::endl;
      returnCode = 1;
      goto Cleanup;
      }
   strcpy(buffer, concatStr.c_str());
   pageSize = concatStr.length();
   while ((bytes = SSL_read(ssl, buffer + pageSize, sizeof(buffer))) > 0) {
      pageSize += bytes;
   }

   // Close the socket and free the address info structure.
   Cleanup:
   SSL_shutdown( ssl );
   SSL_free( ssl );
   close(sd);
   return returnCode;
   }
