//index.cpp

#include "index.h"

void Index::addDocument(HtmlParser &parser) {

   // for every word, append it to the end of its posting list

   for (const auto& wordCount : parser.bodyWords)
      dict.Find(wordCount.first)->value.appendDelta(wordCount.second + WordsInIndex, 0);
   for (const auto& wordCount : parser.headWords)
      dict.Find(wordCount.first)->value.appendDelta(wordCount.second + WordsInIndex, 3);
   for (const auto &wordCount : parser.boldWords)
      dict.Find(wordCount.first)->value.appendDelta(wordCount.second + WordsInIndex, 2);
   for (const auto& wordCount : parser.italicWords)
      dict.Find(wordCount.first)->value.appendDelta(wordCount.second + WordsInIndex, 1);
   for (const auto& wordCount : parser.titleWords)
      dict.Find(titleMarker + wordCount.first)->value.appendDelta(wordCount.second + WordsInIndex);

   //for (auto i : parser.links)
      //do something w anchortext

   dict.Find(eodMarker)->value.appendEOD(parser.count + WordsInIndex, DocumentsInIndex);
   WordsInIndex += parser.count;
   DocumentsInIndex += 1;
   //LocationsInIndex += ?
   //MaximumLocation += ?
   documents.push_back(parser.base);
}

void PostingList::appendDelta(size_t delta) {
   
}

void PostingList::appendDelta(size_t delta, uint8_t style) {

 
}

void PostingList::appendEOD(size_t delta, size_t docIndex) {

}