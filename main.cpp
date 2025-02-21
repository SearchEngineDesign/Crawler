#include "crawler/LinuxGetSsl.h"
#include "parser/HtmlParser.h"
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <mutex>

std::mutex bufferLock;

void parse(char * buffer, size_t &pagesize) {
   std::lock_guard<std::mutex> lock(bufferLock);
   HtmlParser parser( buffer, pagesize );
   for (auto i : parser.links) 
      std::cout << i.URL << std::endl;
   std::lock_guard<std::mutex> unlock(bufferLock);
}

void crawl(ParsedUrl &url, char *buffer, size_t &pageSize) {
   std::lock_guard<std::mutex> lock(bufferLock);
   crawl(url, buffer, pageSize);
   std::lock_guard<std::mutex> unlock(bufferLock);
}

int main(int argc, char* argv[]) {
   if (argc != 2) {
      perror("Usage: ./LinuxGetSsl [url]");
      return 1;
   }


   std::string url = argv[1];
   ParsedUrl purl(url);

   char buffer[2000000]; //how big should the buffer be?
   size_t pageSize;

   std::thread t1(crawl, purl, pageSize);
   std::thread t2(parse, pageSize);
   t1.join();
   t2.join();

   return 0;
}