#include "crawler/crawler.h"
#include "parser/HtmlParser.h"
#include "indexParser/index.h"
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <iostream>

char buffer[2000000];
size_t pageSize = 0;
Crawler crawler;
UrlFrontier frontier;

Index index;

void crawlLoop() {
   while(!frontier.empty()) {
      ParsedUrl cur = frontier.getNextUrl();
      std::cout << cur.urlName << std::endl;
      if (crawler.crawl(cur, buffer, pageSize) == 0) {
         HtmlParser parser( buffer, pageSize );
         index.addDocument(parser);
         for (auto i : parser.links)
            frontier.addNewUrl(i.URL);
      }
   }
}

void parseLoop() {
   
}

int main(int argc, char* argv[]) {
   if (argc != 2) {
      perror("Usage: ./LinuxGetSsl [url]");
      return 1;
   }
   frontier.addNewUrl(argv[1]);
   
   std::thread t1Crawler(crawlLoop);
   std::thread t2Parser(parseLoop);

   t1Crawler.join();
   t2Parser.join();

   return 0;
}