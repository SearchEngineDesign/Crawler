#pragma once
#include <queue>
#include <unordered_map>
#include "../include/string.h"
#include <set>

class ParsedUrl {
public:
    string urlName;
    string Service, Host, Port, Path;

    ParsedUrl(const string& url) {
        urlName = url;

        size_t pos = 0;
        const char *colon = ":";
        const char *slash = "/";
        int colonPos = url.find(colon);
        int slashPos = url.find(slash, colonPos + 2);

        if (colonPos != -1) {
            // Extract Service
            Service = url.substr(0, colonPos);
            pos = colonPos + 1;

            // Skip "://" if present
            if (url.at(pos) == '/' && url.at(pos+1) == '/') {
                pos += 2;
            }

            // Extract Host
            int hostEnd = url.find(slash, pos);
            hostEnd == -1 ? hostEnd = url.length() : hostEnd = hostEnd;
            Host = url.substr(pos, hostEnd - pos);
            pos += Host.length();

            // Extract Port if present
            if (url.at(pos) == ':') {
                pos++;
                int portEnd = url.find(slash, pos);
                Port = url.substr(pos, portEnd - pos);
                pos += portEnd;
            }

            // Extract Path
            if (pos < url.length()) {
                Path = url.substr(pos, url.length() - pos);
                if (Path == "/")
                    Path = "";
            }
        } else {
            Host = url;
            Path = "";
        }
    }

    ~ParsedUrl() {
        // No manual memory management required with string
    }
};


class UrlFrontier {

private:
   std::queue<ParsedUrl> urlQueue;
   std::set<string> seen;

public:
   void addNewUrl(const string & urlName) {
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

   void addSeenUrl(const string & urlName) {
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