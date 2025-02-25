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
UrlFrontier frontier;

void crawlLoop() {
   while(!frontier.empty()) {
      ParsedUrl cur = frontier.getNextUrl();
      //std::cout << cur.urlName << std::endl;
      {
      std::unique_lock<std::mutex> lk(bufLock);
      crawl(cur, buffer, pageSize);
      finishedParse = true;
      cv.notify_one();
      }
   }
}

void parseLoop() {
   //while(!frontier.empty()) {
      {
      std::unique_lock<std::mutex> lk(bufLock);
      cv.wait(lk, []{ return finishedParse; });
      HtmlParser parser( buffer, pageSize );
      for (auto i : parser.links) 
         std::cout << i.URL << std::endl;
         //frontier.addNewUrl(i.URL);
      finishedParse = false;
      }
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