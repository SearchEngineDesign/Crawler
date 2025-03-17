#include <fstream>
#include "isr.h"

int main() {
   Index index;
   Dictionary dict;
   size_t numOfWords, numOfUniqueWords, numOfDocuments;

   std::ifstream inputFile("numbers.txt"); // Open the file for reading
   if (!inputFile) {
      std::cerr << "Error: Could not open the file.\n";
      return 1; // Exit with an error code
   }

   inputFile >> numOfWords >> numOfUniqueWords >> numOfDocuments;



   inputFile.close(); // Close the file


}