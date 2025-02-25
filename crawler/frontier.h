#pragma once
#include <queue>
#include <unordered_map>
#include <string>
#include <string.h>
#include <set>

class ParsedUrl {
public:
    std::string urlName;
    std::string Service, Host, Port, Path;

    ParsedUrl(const std::string& url) {
        urlName = url;

        size_t pos = 0;
        size_t colonPos = url.find(':');
        size_t slashPos = url.find('/', colonPos + 3);

        if (colonPos != std::string::npos) {
            // Extract Service
            Service = url.substr(0, colonPos);
            pos = colonPos + 1;

            // Skip "://" if present
            if (url[pos] == '/' && url[pos + 1] == '/') {
                pos += 2;
            }

            // Extract Host
            size_t hostEnd = url.find_first_of(":/", pos);
            
                Host = url.substr(pos, hostEnd - pos);
                pos = hostEnd;

                // Extract Port if present
                if (url[pos] == ':') {
                    pos++;
                    size_t portEnd = url.find('/', pos);
                    Port = url.substr(pos, portEnd - pos);
                    pos = portEnd;
                }

            // Extract Path
            if (pos != std::string::npos) {
                Path = url.substr(pos);
            }
        } else {
            Host = url;
            Path = "";
        }
    }

    ~ParsedUrl() {
        // No manual memory management required with std::string
    }
};


class UrlFrontier {

private:
   std::queue<ParsedUrl> urlQueue;
   std::set<std::string> seen;

public:
   void addNewUrl(const std::string & urlName) {
      if (seen.find(urlName) == seen.end()) {
        // TODO: priority?
        ParsedUrl url(urlName);
        urlQueue.push(url);
        addSeenUrl(urlName);
      }
   }

   ParsedUrl getNextUrl() {
      if (urlQueue.empty()) {
        ParsedUrl empty("");
        return empty;
      }
      ParsedUrl nextUrl = urlQueue.front();
      urlQueue.pop();
      return nextUrl;
   }

   void addSeenUrl(const std::string & urlName) {
      seen.insert(urlName);
   }

    bool empty() {
        return urlQueue.empty();
    }

   size_t getQueueSize() {
      return urlQueue.size();
   }

   size_t getSeenSize() {
      return seen.size();
   }

};