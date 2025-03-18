#include <sys/socket.h>
#include <thread>
#include <condition_variable>
#include <iostream>
#include "crawler/crawler.h"
#include "index/index.h"

char buffer[BUFFER_SIZE];
size_t pageSize = 0;
Crawler crawler;
UrlFrontier frontier;

Index *in;

void printBuffer() {
   for (int i = 0; i < pageSize; i++) {
      std::cout << buffer[i];
   }
}

void crawlLoop() {
   int i = 0;
   while(!frontier.empty()) {
      ParsedUrl cur = frontier.getNextUrl();
      if (crawler.robots( frontier, buffer, pageSize, cur )) {
         if (crawler.crawl(cur, buffer, pageSize) == 0) {
            HtmlParser parser( buffer, pageSize );
            in->addDocument(parser, cur.urlName);
            for (auto i : parser.links)
               frontier.addNewUrl(i.URL);
         }
      }
   }
}

void parseLoop() {
   
}

int main(int argc, char* argv[]) {
   if (argc != 3) {
      perror("Usage: ./LinuxGetSsl [url] [index file name]");
      return 1;
   }
   frontier.addNewUrl(argv[1]);
   IndexHandler handler(argv[2]);
   in = handler.index;
   
   std::thread t1Crawler(crawlLoop);
   std::thread t2Parser(parseLoop);

   t1Crawler.join();
   t2Parser.join();


   return 0;
}