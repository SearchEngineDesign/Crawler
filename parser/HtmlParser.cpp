// HtmlParser.cpp
// Nicole Hamlton, nham@umich.edu

// If you don't define the HtmlParser class methods inline in
// HtmlParser.h, you may do it here.

#include "HtmlParser.h"
#include "../include/vector.h"
#include "../include/string.h"
#include <iostream>
#include <cstring>

void HtmlParser::appendWord(const string &word,
                           vector< std::pair<string, size_t> > &vec, bool append) {
   if (append && !vec.empty()) {
      vec.back().first += word;
   } else {
      vec.push_back(std::make_pair(word, count));
   }
}

void HtmlParser::appendWord(const char * ptr, long len,
                           vector< std::pair<string, size_t> > &vec) {
   vec.emplace_back(string(ptr, len), count);
}

string HtmlParser::complete_link(string link, string base_url)
{
   if (link.find("http://") == 0 || link.find("https://") == 0)
   {
      return link;
   }
   else
   {
      size_t slashCount = 0;
      while (*link.at(0) == '/') {
         link = link.substr(1, link.length() - 1); //remove first character (/)
         slashCount++;
      }
      while (*base_url.at(base_url.length() - 1) == '/')
         base_url = base_url.substr(0, base_url.length() - 1); //remove last character (/)
      switch (slashCount) {
         case 0:
            return base_url + "/" + link;
         case 1:
            return base_url + "/" + link;
         case 2:
            return string("https://") + link;
         default:
            return "";
      }  
   }
}

HtmlParser::HtmlParser( const char *buffer, size_t length )
   {
   size_t i = 0;  
   bool inTitle = false;  
   bool indiscard = false;  
   bool inAnchor = false;
   bool inHead = false;
   bool inItalic = false;
   bool inBold = false;
   string url;  
   vector< string > curr_anchorText;  
   const char *p = buffer;  
   const char *end = buffer + length; 

   if ( *p == 'h' && *(p+1) == 't' && *(p+2) == 't' && *(p+3) == 'p' )
   {
      while ( *p != '\n' )
         p++;
      base = string( buffer, p - buffer);
      p++;
   }
   while (*p != '<')
      p++;

   while ( p < end )
      {
      if ( *p == '<' )
         {
         p++;  
         bool endflag = ( *p == '/' );  
         if ( endflag )
            p++;  

         const char *tag_start = p;  
         while ( p < end && *p != '>' && *p != '/' && !isspace( *p ) )
            p++;  

         DesiredAction action = LookupPossibleTag( tag_start, p );  
         if ( endflag )
            {
            if ( action == DesiredAction::Title )
               inTitle = false;  
            else if ( action == DesiredAction::Head )
               inHead = false;
            else if ( action == DesiredAction::Anchor )
               {
               if ( inAnchor )
                  {
                  inAnchor = false;  
                  if ( !url.empty() )
                     {
                     url = complete_link(url, base);
                     if (!url.empty()) {
                        Link curr_link( url );  
                        links.push_back( curr_link );  
                        links.back().anchorText = curr_anchorText;  
                     }
                     }
                  curr_anchorText.clear();  
                  }
               else
                  {
                  while ( p < end && *p!= '>' )
                     p++;  
                  p ++;  
                  continue;  
                  }
               }
            else if ( action == DesiredAction::DiscardSection )
               indiscard = false;  
            else if ( action == DesiredAction::Bold )
               inBold = false;
            else if ( action == DesiredAction::Italic )
               inItalic = false;
            }
         else
            {
            switch ( action )
               {
               case DesiredAction::Title:
                  if ( inTitle )
                     {
                     while ( p < end && *p != '>' )
                        p++;  
                     p++;  
                     continue;  
                     }
                  inTitle = true;  
                  break;  

               case DesiredAction::Base:
                  while ( p < end && base.empty() )
                     {
                     if ( strncmp( p, "href=", 5 ) == 0 && p[ 5 ] == '"' )
                        {
                        p += 6;  
                        const char *start = p;  
                        while ( *p != '"' && p < end ) 
                           p++;  
                        base.assign( start, p - start );  
                        break;  
                        }
                     else if ( memcmp( p, "/>", 2 ) == 0 )
                        break;  
                     p++;  
                     }
                  break;  

               case DesiredAction::Embed:
                  {
                  string embed_url = "";  
                  while ( p < end && *p != '>' )
                     {
                     if ( memcmp( p, "src=", 4 ) == 0 && p[ 4 ] == '"' )
                        {
                        p += 5;  
                        const char *start = p;  
                        while ( *p != '"' && p < end ) 
                           p++;  
                        embed_url.assign( start, p - start );  
                        break;  
                        }
                     p++;  
                     }
                  if ( !embed_url.empty() )
                     {
                        url = complete_link(embed_url, base);
                        if (url.find("http") != -1)
                           links.push_back( url );
                     }
                  break;  
                  }
               case DesiredAction::Anchor:
                  while ( p < end && *p != '>' )
                     {
                     if ( strncmp( p, "href=", 5 ) == 0 && p[ 5 ] == '"' ) 
                        {
                        if ( inAnchor )
                           {
                           if ( !url.empty() )
                              {
                              url = complete_link(url, base);
                              Link curr_link( url );  
                              links.push_back( curr_link );  
                              links.back().anchorText = curr_anchorText;  
                              curr_anchorText.clear();  
                              }
                           curr_anchorText.clear();  
                           }
                        inAnchor = true;  
                        p += 6;  
                        const char *start = p;  
                        while ( *p != '"' && p < end )
                           p++;  
                        url.assign( start, p - start );  
                        break;  
                        }
                     p++;  
                     }
                  break;  

               case DesiredAction::Comment:
                  while ( p < end - 2 && !( p[ 0 ] == '-' && p[ 1 ] == '-' && p[ 2 ] == '>' ) ) 
                     p++;  
                  break;  

               case DesiredAction::DiscardSection:
                  indiscard = true;  
                  break;  

               case DesiredAction::Head:
                  inHead = true;
                  break;
               case DesiredAction::Bold:
                  inBold = true;
                  break;
               case DesiredAction::Italic:
                  inItalic = true;
                  break;
               case DesiredAction::OrdinaryText:
                  {
                  const char *start = p - 2;  
                  while ( p < end - 1 && *p != '<' && !isspace( *p ) && !( p[ 0 ] == '/' && p[ 1 ] == '>' ) )
                     p++;  
                  const string word( start, p - start );  
                  const bool emptyWord = ( p - start ) == 0;  

                  if ( !emptyWord && !indiscard )
                     {
                     if ( inTitle )
                        {
                        if ( !isspace( *( start - 1 ) ) && !titleWords.empty() )
                           appendWord(word, titleWords, true);
                        else 
                           appendWord(word, titleWords, false);

                        if ( inAnchor ) 
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;  
                           else
                              curr_anchorText.push_back( word );  
                           }
                        }
                     else if ( inHead )
                        {
                        if ( !isspace( *( start - 1 ) ) && !headWords.empty() )
                           appendWord(word, headWords, true);
                        else
                           appendWord(word, headWords, false);
                        if ( !isspace( *( start - 1 ) ) && !bodyWords.empty() )
                           appendWord(word, bodyWords, true);
                        else
                           appendWord(word, bodyWords, false);
                        if ( inAnchor )
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;
                           else
                              curr_anchorText.push_back( word );
                           }
                        }
                     else if ( inAnchor )
                        {
                        if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                           {
                           curr_anchorText.back() += word;  
                           appendWord(word, bodyWords, true); 
                           } 
                        else
                           {
                           curr_anchorText.push_back( word );  
                           appendWord(word, bodyWords, false);
                           }
                        }
                     else
                        {
                        if ( !isspace( *( start - 1) ) ) 
                           appendWord(word, bodyWords, true);
                        else 
                           appendWord(word, bodyWords, false);
                        }
                     if ( inBold )
                        {
                        if ( !isspace( *( start - 1 ) ) && !boldWords.empty() )
                           appendWord(word, boldWords, true);
                        else
                           appendWord(word, boldWords, false);
                        }
                     else if ( inItalic )
                        {
                        if ( !isspace( *( start - 1 ) ) && !italicWords.empty() )
                           appendWord(word, italicWords, true);
                        else
                           appendWord(word, italicWords, false);
                        }
                     continue;  
                     }
                  }
               default:
                  break;  
               }
            }
            
         while ( p < end && *p != '>' ) 
            p++;  
         p++;  
         }
      else
         {
         const char *start = p;  
         while ( p < end && *p != '<' && !isspace( *p ) && !( p[ 0 ] == '/' && p[ 1 ] == '>' ) )
            p++;  

         const bool isEmptyWord = ( p - start ) == 0;  

         if ( !isEmptyWord && !indiscard )
            {
            count++;
            if ( inTitle )
               {
               appendWord(start, p - start, titleWords);
               if ( inAnchor )
                  curr_anchorText.emplace_back( start, p - start );  
               }
            else 
               {
               appendWord(start, p - start, bodyWords);
               if ( inHead )
                  {
                  appendWord(start, p - start, headWords);
                  if ( inAnchor )
                     curr_anchorText.emplace_back( start, p - start );
                  }
               else if ( inAnchor )
                  {
                  curr_anchorText.emplace_back( start, p - start );  
                  }
               }

               
            if ( inBold )
               {
               appendWord(start, p - start, boldWords);
               }
            else if ( inItalic )
               {
               appendWord(start, p - start, boldWords);
               }
            }
         if ( p < end && *p == '<' ) 
            continue;  
         else 
            p++;  
         }
      }
   }