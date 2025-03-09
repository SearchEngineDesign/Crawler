//index.cpp

#include "index.h"

void Index::addDocument(HtmlParser &parser) {
   for (auto i : parser.bodyWords)
      dict.Find(i.first)->value.appendDelta(i.second + WordsInIndex, 0);
   for (auto i : parser.headWords)
      dict.Find(i.first)->value.appendDelta(i.second + WordsInIndex, 3);
   for (auto i : parser.boldWords)
      dict.Find(i.first)->value.appendDelta(i.second + WordsInIndex, 2);
   for (auto i : parser.italicWords)
      dict.Find(i.first)->value.appendDelta(i.second + WordsInIndex, 1);
   for (auto i : parser.titleWords)
      dict.Find(titleMarker + i.first)->value.appendDelta(i.second + WordsInIndex);

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