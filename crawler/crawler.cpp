// crawler.cpp

#include "crawler.h"

Crawler::Crawler() {
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;
   sd = 0;
   currentHost = "";
}

void Crawler::freeSSL() {
   if (ssl != nullptr) {
      SSL_shutdown( ssl );
      SSL_free( ssl );
      close(sd);
   }
   currentHost = "";
}

bool Crawler::verifySSL() {
   if (!ssl) {
      return false;
   }
   int error = 0;
   socklen_t len = sizeof(error);
   int ret = getsockopt(sd, SOL_SOCKET, SO_ERROR, &error, &len);
   if (ret == 0)
      return true;
   else
      return false;
}

int Crawler::setupConnection(string hostName) {
   if (getaddrinfo(hostName.c_str(), "443", &hints, &address) < 0) {
      std::cerr << "Address lookup failed." << std::endl;
      return 1;
   }

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
   SSL_CTX_set_options(ctx, SSL_OP_IGNORE_UNEXPECTED_EOF);

   if (ctx == NULL) {
      std::cerr << "Couldn't initialize SSL context." << std::endl;
      return 1;
   }

   ssl = SSL_new(ctx);
   if (!ssl) {
      std::cerr << "SSL initialization failed." << std::endl;
      return 1;
   }
   SSL_set_tlsext_host_name(ssl, hostName.c_str());
   
   
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
      freeSSL();
      return 1;
   }
   return 0;
}

int Crawler::crawl ( ParsedUrl url, char *buffer, size_t &pageSize)
   {
   int bytes;
   string concatStr = url.Service + string("://") + url.Host + string("\n");
   string path = url.Path;
   
   const char* route = url.Host.c_str();
   hostent *host = gethostbyname(route);
   //if (string(host->h_name) != currentHost || !verifySSL()) {
      if (setupConnection(url.Host) != 0)
         return 1;
      currentHost = host->h_name;
   //}

   //GET Message construction
   if (*path.at(0) != '/')
      path = string("/") + path;
   
   string getMessage =
      string("GET ") + path + " HTTP/1.1\r\n"
      "Host: " + route + "\r\n"
      "User-Agent: LinuxGetSsl/2.0 codefather@umich.edu (Linux)\r\n"
      "Accept: */*\r\n"
      "Accept-Encoding: identity\r\n"
      "Connection: close\r\n\r\n";

   if (SSL_write( ssl, getMessage.c_str(), getMessage.length() ) <= 0 ) {
      std::cerr << "Failed to write over SSL" << std::endl;
      freeSSL();
      return 1;
   }
   strcpy(buffer, concatStr.c_str());
   pageSize = concatStr.length();
   while ((bytes = SSL_read(ssl, buffer + pageSize, sizeof(buffer))) > 0 && pageSize < 2000000) {
      pageSize += bytes;
   }
   
   return 0;
   }
