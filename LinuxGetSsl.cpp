#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>

#include <iostream>

class ParsedUrl
   {
   public:
      const char *CompleteUrl;
      char *Service, *Host, *Port, *Path;

      ParsedUrl( const char *url )
         {
         // Assumes url points to static text but
         // does not check.

         CompleteUrl = url;

         pathBuffer = new char[ strlen( url ) + 1 ];
         const char *f;
         char *t;
         for ( t = pathBuffer, f = url; *t++ = *f++; )
            ;

         Service = pathBuffer;

         const char Colon = ':', Slash = '/';
         char *p;
         for ( p = pathBuffer; *p && *p != Colon; p++ )
            ;

         if ( *p )
            {
            // Mark the end of the Service.
            *p++ = 0;

            if ( *p == Slash )
               p++;
            if ( *p == Slash )
               p++;

            Host = p;

            for ( ; *p && *p != Slash && *p != Colon; p++ )
               ;

            if ( *p == Colon )
               {
               // Port specified.  Skip over the colon and
               // the port number.
               *p++ = 0;
               Port = +p;
               for ( ; *p && *p != Slash; p++ )
                  ;
               }
            else
               Port = p;

            if ( *p )
               // Mark the end of the Host and Port.
               *p++ = 0;

            // Whatever remains is the Path.
            Path = p;
            }
         else
            Host = Path = p;
         }

      ~ParsedUrl( )
         {
         delete[ ] pathBuffer;
         }

   private:
      char *pathBuffer;
   };

int main( int argc, char **argv )
   {

   if ( argc != 2 )
      {
      std::cerr << "Usage: " << argv[ 0 ] << " url" << std::endl;
      return 1;
      }

   // Parse the URL
   ParsedUrl url( argv[ 1 ] );

   // Get the host address.
   struct addrinfo *address, hints;
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;
   
   int status = getaddrinfo( url.Host, "80", &hints, &address );
   if ( status != 0 ) {
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
      close( socketfd );
      freeaddrinfo( address );
      return 1;
      }
   
   // Build an SSL layer and set it to read/write
   // to the socket we've connected.
   SSL_library_init(); //always returns 1
   SSL_CTX *ctx = SSL_CTX_new( SSLv23_method( ) );
   if ( !ctx )
      {
      std::cerr << "CTX creation failed." << std::endl;
      close( socketfd );
      freeaddrinfo( address );
      return 1;  
      }
   SSL *ssl = SSL_new( ctx );
   if ( !ssl )
      {
      std::cerr << "SSL creation failed." << std::endl;
      close( socketfd );
      freeaddrinfo( address );
      return 1;  
      }
   SSL_set_fd( ssl, socketfd);
   if (SSL_connect( ssl ) != 1)
      {
      std::cerr << "SSL connection failed." << std::endl;
      close( socketfd );
      freeaddrinfo( address );
      return 1;  
      }

   // Send a GET message.
   std::string path = url.Path;
   if (path[0] != '/') {
      path = "/" + path;
   }
   
   std::string getMessage =
      "GET " + path + " HTTP/1.1\r\n"
      "Host: " + std::string(url.Host) + "\r\n"
      "User-Agent: LinuxGetSsl/2.0 darrinz@umich.edu (Linux)\r\n"
      "Accept: */*\r\n"
      "Accept-Encoding: identity\r\n"
      "Connection: close\r\n\r\n";

   // Read from the socket until there's no more data, copying it to
   // stdout.
   send(socketfd, getMessage.c_str(), getMessage.length(), 0);
   
   char buffer[10240];
   int bytes;
   bool headerEnded = false;
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
   close(socketfd);
   freeaddrinfo(address);
   SSL_shutdown( ssl );
   SSL_free( ssl );
   SSL_CTX_free( ctx );
   return 0;

   }
