//index.cpp

#include "index.h"

void Index::addDocument(HtmlParser &parser) {
   Tuple<string, PostingList> *seek;
   for (auto i : parser.bodyWords) {
      seek = dict.Find(i.first, PostingList(i.first, Token::Body));
      seek->value.appendBodyDelta(i.second + WordsInIndex, 0);
   }
   for (auto i : parser.headWords) {
      seek = dict.Find(i.first, PostingList(i.first, Token::Body));
      seek->value.appendBodyDelta(i.second + WordsInIndex, 3);
   }
   for (auto i : parser.boldWords) {
      seek = dict.Find(i.first, PostingList(i.first, Token::Body));
      seek->value.appendBodyDelta(i.second + WordsInIndex, 2);
   }
   for (auto i : parser.italicWords) {
      seek = dict.Find(i.first, PostingList(i.first, Token::Body));
      seek->value.appendBodyDelta(i.second + WordsInIndex, 1);
   }
   for (auto i : parser.titleWords) {
      seek = dict.Find(titleMarker + i.first, PostingList(i.first, Token::Title));
      seek->value.appendTitleDelta(i.second + WordsInIndex);
   }

   for (auto i : parser.links) {

   }

   seek = dict.Find(eodMarker, PostingList(eodMarker, Token::EoD));
   seek->value.appendEODDelta(parser.count + WordsInIndex, DocumentsInIndex);
   WordsInIndex += parser.count;
   DocumentsInIndex += 1;
   //LocationsInIndex += ?
   //MaximumLocation += ?
   documents.push_back(parser.base);
}

//for utillity
uint8_t bitsNeeded(const size_t n) {
    if (n == 0) {
        return 1; 
    }
    return std::max(1, static_cast<int>(std::ceil(std::log2(n + 1))));
}

vector<bool> formatUtf8(const size_t &delta) {
   const uint8_t boundary = bitsNeeded(delta);
   std::bitset<sizeof(delta) << 3> deltaBits(delta);

   size_t bytes = 0;
   if (boundary < 7)
      bytes = 0;
   else if (boundary < 12)
      bytes = 1;
   else if (boundary < 17)
      bytes = 2;
   else if (boundary < 22)
      bytes = 3;
   else if (boundary < 27)
      bytes = 4;
   else if (boundary < 32)
      bytes = 5;
   else if (boundary < 37)
      bytes = 6;

   const uint8_t bits = ((bytes + 1) << 3) - 1;
   vector<bool> bitset(bits + 1);
   uint8_t bitsetIndex = 0, initDelta = 0, deltaIndex = 0;
   
   while(deltaIndex < boundary) {
      initDelta = deltaIndex + 6;
      for (; deltaIndex < initDelta && deltaIndex < boundary; deltaIndex++) {
         bitset[bitsetIndex] = deltaBits[deltaIndex];
         bitsetIndex++;
      }
      if (deltaIndex < boundary) {
         bitset[bitsetIndex] = 0;
         bitset[bitsetIndex + 1] = 1;
         bitsetIndex += 2;
      }
   }
   for (int i = bits; i >= bits - bytes; i--) {
       bitset[i] = 1;
   }
   return bitset;
}

void PostingList::appendTitleDelta(const size_t delta) {
   list.push_back(Post(formatUtf8(delta)));
}

void PostingList::appendBodyDelta(size_t delta, uint8_t style) {
   delta = delta << 2;
   delta += style;
   list.push_back(Post(formatUtf8(delta)));   
}

void PostingList::appendEODDelta(size_t delta, size_t docIndex) {
   delta = delta << sizeof(docIndex);
   delta += docIndex;
   list.push_back(Post(formatUtf8(delta)));   
}