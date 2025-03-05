//index.cpp

#include "index.h"
#include "../parser/HtmlParser.h"

void Index::addDocument(HtmlParser &parser) {
   for (auto i : parser.bodyWords)
      dict.Find(i.first)->value.update(i.second + wordCount, 0);
   for (auto i : parser.headWords)
      dict.Find(i.first)->value.update(i.second + wordCount, 3);
   for (auto i : parser.boldWords)
      dict.Find(i.first)->value.update(i.second + wordCount, 2);
   for (auto i : parser.italicWords)
      dict.Find(i.first)->value.update(i.second + wordCount, 1);
   for (auto i : parser.titleWords)
      dict.Find(titleMarker + i.first)->value.update(i.second + wordCount);

   //for (auto i : parser.links)
      //do something w anchortext

   dict.Find(eodMarker)->value.updateEOD(parser.count + wordCount, documents.size());
   wordCount += parser.count;
   documents.push_back(parser.base);
}