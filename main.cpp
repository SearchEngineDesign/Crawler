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
bool bufferInUse = false;
UrlFrontier frontier;

void crawlLoop() {
   while(!frontier.empty()) {
      ParsedUrl cur = frontier.getNextUrl();
      //std::cout << cur.urlName << std::endl;
      {
      std::unique_lock<std::mutex> lk(bufLock);
      cv.wait(lk, []{ return !bufferInUse; });
      crawl(cur, buffer, pageSize);
      bufferInUse = true;
      cv.notify_one();
      }
   }
}

void parseLoop() {
   //while(!frontier.empty()) {
      {
      std::unique_lock<std::mutex> lk(bufLock);
      cv.wait(lk, []{ return bufferInUse; });
      HtmlParser parser( buffer, pageSize );
      for (int i = 0; i < pageSize; i++) {
         std::cout << buffer[i];
      }
      bufferInUse = false;
      cv.notify_one();
      }
      
      //for (auto i : parser.links) 
         //frontier.addNewUrl(i.URL);
   //}
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