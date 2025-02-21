// HtmlParser.cpp
// Nicole Hamlton, nham@umich.edu

// If you don't define the HtmlParser class methods inline in
// HtmlParser.h, you may do it here.

#include "HtmlParser.h"
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
using namespace std;

HtmlParser::HtmlParser( const char *buffer, size_t length )
   {
   size_t i = 0;  
   bool inTitle = false;  
   bool indiscard = false;  
   bool inAnchor;  
   string url;  
   vector< string > curr_anchorText;  
   const char *p = buffer;  
   const char *end = buffer + length;  

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
            else if ( action == DesiredAction::Anchor )
               {
               if ( inAnchor )
                  {
                  inAnchor = false;  
                  if ( !url.empty() )
                     {
                     Link curr_link( url );  
                     links.push_back( curr_link );  
                     links.back().anchorText = curr_anchorText;  
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
                     links.emplace_back( url );  
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
                           titleWords.back() += word;  
                        else 
                           titleWords.emplace_back( word );  

                        if ( inAnchor ) 
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;  
                           else
                              curr_anchorText.emplace_back( word );  
                           }
                        }
                     else if ( inAnchor )
                        {
                        if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                           {
                           curr_anchorText.back() += word;  
                           words.back() += word;  
                           } 
                        else
                           {
                           curr_anchorText.emplace_back( word );  
                           words.emplace_back( word );  
                           }
                        }
                     else
                        {
                        if ( !isspace( *( start - 1) ) ) 
                           words.back() += word;  
                        else 
                           words.emplace_back( word );  
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
            if ( inTitle )
               {
               titleWords.emplace_back( start, p - start );  
               if ( inAnchor )
                  curr_anchorText.emplace_back( start, p - start );  
               }
            else if ( inAnchor  )
               {
               curr_anchorText.emplace_back( start, p - start );  
               words.emplace_back( start, p - start );  
               }
            else
               words.emplace_back( start, p - start );  
            }
         if ( p < end && *p == '<' ) 
            continue;  
         else 
            p++;  
         }
      }
   }