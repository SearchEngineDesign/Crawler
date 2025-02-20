#include "crawler/LinuxGetSsl.h"
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
   if (argc != 2) {
      perror("Usage: ./LinuxGetSsl [url]");
      return 1;
   }

   int sockets[2];

   if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
   perror("socketpair");
   return 1;
   }

   pid_t pid = fork();
   if (pid == -1) {
   perror("fork");
   return 1;
   }

   if (pid == 0) {
      close(sockets[0]); // Close the reading end

      std::string url = argv[1];
      char buffer[2000000]; //don't use a buffer! write to a mapped file or other data structure
      size_t pageSize;
      ParsedUrl purl(url);
      crawl(purl, buffer, pageSize);
      std::cout << "Sending: " << pageSize << std::endl;
      send(sockets[1], &pageSize, sizeof(size_t), 0);
      send(sockets[1], buffer, strlen(buffer), 0);

      close(sockets[1]);
   } else {
      // Parent process
      close(sockets[1]); // Close the writing end
      size_t pageSize;
      ssize_t bytes_received = recv(sockets[0], &pageSize, sizeof(size_t), 0);
      char buffer[pageSize];
      bytes_received = recv(sockets[0], buffer, sizeof(buffer), 0);
      if (bytes_received > 0) {
         buffer[bytes_received] = '\0';
         std::cout << "Parent received: " << buffer << std::endl;
      }

      close(sockets[0]);
   }

   return 0;
}