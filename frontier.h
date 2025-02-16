#pragma once
#include <queue>
#include <unordered_map>
#include <string>
#include <string.h>
#include <set>

class ParsedUrl {
public:
   std::string urlName;
   char *Service, *Host, *Port, *Path;

   ParsedUrl( const char *url) {
      // Assumes url points to static text but
      // does not check.

      urlName = url;

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

   ~ParsedUrl( ) {
      delete[ ] pathBuffer;
   }

private:
   char *pathBuffer;
};


class UrlFrontier {

private:
   std::queue<ParsedUrl> urlQueue;
   std::set<std::string> seen;

public:
   void addNewUrl(const std::string & urlName) {
      if (seen.find(urlName) == seen.end()) {
         // TODO: priority?
         ParsedUrl url(urlName.c_str());
         urlQueue.push(url);
      }
   }

   std::string getNextUrl() {
      if (urlQueue.empty()) {
         return "";
      }
      ParsedUrl nextUrl = urlQueue.front();
      urlQueue.pop();
      return nextUrl.Path;
   }

   void addSeenUrl(const std::string & urlName) {
      seen.insert(urlName);
   }

   size_t getQueueSize() {
      return urlQueue.size();
   }

   size_t getSeenSize() {
      return seen.size();
   }

};