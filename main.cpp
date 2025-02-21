#include "crawler/LinuxGetSsl.h"
#include "parser/HtmlParser.h"
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <mutex>

void parse(char * buffer, size_t &pagesize) {
   std::lock_guard<std::mutex> lock(bufferLock);
   HtmlParser parser( buffer, pagesize );
   for (auto i : parser.links) 
      std::cout << i.URL << std::endl;
}

int main(int argc, char* argv[]) {
   if (argc != 2) {
      perror("Usage: ./LinuxGetSsl [url]");
      return 1;
   }

   std::mutex bufferLock;

   std::string url = argv[1];
   ParsedUrl purl(url);

   char buffer[2000000]; //how big should the buffer be?
   size_t pageSize;

   std::thread t1(crawl, purl, buffer, pageSize, bufferLock);
   std::thread t2(parse, buffer, pageSize);

   return 0;
}