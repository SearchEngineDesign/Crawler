#include "crawler/LinuxGetSsl.h"
#include "parser/HtmlParser.h"
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <condition_variable>

char buffer[2000000];
size_t pageSize = 0;
std::mutex bufLock;
std::condition_variable cv;
bool finishedParse = false;

void crawlLoop(ParsedUrl purl) {
   {
   std::lock_guard<std::mutex> lk(bufLock);
   crawl(purl, buffer, pageSize);
   finishedParse = true;
   }
}

void parseLoop() {
   {
   std::unique_lock<std::mutex> lk(bufLock);
   cv.wait(lk, []{ return finishedParse; });
   HtmlParser parser( buffer, pageSize );
   for (auto i : parser.links) 
      std::cout << i.URL << std::endl;
   finishedParse = false;
   }
}

int main(int argc, char* argv[]) {
   if (argc != 2) {
      perror("Usage: ./LinuxGetSsl [url]");
      return 1;
   }
   
   std::thread t1Crawler(crawlLoop, ParsedUrl(argv[1]));
   std::thread t2Parser(parseLoop);

   t1Crawler.join();
   t2Parser.join();

   return 0;
}