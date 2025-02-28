// HtmlParser.cpp
// Nicole Hamlton, nham@umich.edu

// If you don't define the HtmlParser class methods inline in
// HtmlParser.h, you may do it here.

#include "HtmlParser.h"
#include "../include/vector.h"
#include "../include/string.h"
#include <iostream>
#include <cstring>

string HtmlParser::complete_link(string link, string base_url)
{
   if (link.find("http") != -1)
   {
      return link;
   }
   else
   {
      size_t count = 0;
      for (int i = 0; i < link.size(); i++)
      {
         if (link[i] == '/')
            count++;
         else
            break;
      }

      string slash = "/";
      if (base_url[base_url.size() - 1] != '/')
         base_url += slash;  

      size_t last_slash_pos = base_url.size();

      while ( count > 0 && last_slash_pos > 0 )
      {
         while ( last_slash_pos > 0 && base_url[last_slash_pos - 1] != '/' )
            last_slash_pos--;
         
         if (last_slash_pos > 0)
            base_url = base_url.substr(0, last_slash_pos - 1);
         
         count--;
      }

      return base_url + link;
   }
}

HtmlParser::HtmlParser( const char *buffer, size_t length )
   {
   size_t i = 0;  
   bool inTitle = false;  
   bool indiscard = false;  
   bool inAnchor = false;
   bool inHead1 = false, inHead2 = false, inHead3 = false;
   bool inHead4 = false, inHead5 = false, inHead6 = false;
   bool inBold = false, inItalic = false;
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
            else if ( action == DesiredAction::Head1 )
               inHead1 = false;
            else if ( action == DesiredAction::Head2 )
               inHead2 = false;
            else if ( action == DesiredAction::Head3 )
               inHead3 = false;
            else if ( action == DesiredAction::Head4 )
               inHead4 = false;
            else if ( action == DesiredAction::Head5 )
               inHead5 = false;
            else if ( action == DesiredAction::Head6 )
               inHead6 = false;
            else if ( action == DesiredAction::Anchor )
               {
               if ( inAnchor )
                  {
                  inAnchor = false;  
                  if ( !url.empty() )
                     {
                     url = complete_link(url, base);
                     Link curr_link( url );  
                     if (curr_link.URL.find("https") != -1)
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

               case DesiredAction::Head1:
                  inHead1 = true;
                  break;
               case DesiredAction::Head2:
                  inHead2 = true;
                  break;
               case DesiredAction::Head3:
                  inHead3 = true;
                  break;
               case DesiredAction::Head4:
                  inHead4 = true;
                  break;
               case DesiredAction::Head5:
                  inHead5 = true;
                  break;
               case DesiredAction::Head6:
                  inHead6 = true;
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
                           titleWords.back() += word;  
                        else 
                           titleWords.push_back( word );  

                        if ( inAnchor ) 
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;  
                           else
                              curr_anchorText.push_back( word );  
                           }
                        }
                     else if ( inHead1 )
                        {
                        if ( !isspace( *( start - 1 ) ) && !head1Words.empty() )
                           head1Words.back() += word;
                        else
                           head1Words.push_back( word );
                        if ( !isspace( *( start - 1 ) ) && !words.empty() )
                           words.back() += word;
                        else
                           words.push_back( word );
                        if ( inAnchor )
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;
                           else
                              curr_anchorText.push_back( word );
                           }
                        }
                     else if ( inHead2 )
                        {
                        if ( !isspace( *( start - 1 ) ) && !head2Words.empty() )
                           head2Words.back() += word;
                        else
                           head2Words.push_back( word );
                        if ( !isspace( *( start - 1 ) ) && !words.empty() )
                           words.back() += word;
                        else
                           words.push_back( word );
                        if ( inAnchor )
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;
                           else
                              curr_anchorText.push_back( word );
                           }
                        }
                     else if ( inHead3 )
                        {
                        if ( !isspace( *( start - 1 ) ) && !head3Words.empty() )
                           head3Words.back() += word;
                        else
                           head3Words.push_back( word );
                        if ( !isspace( *( start - 1 ) ) && !words.empty() )
                           words.back() += word;
                        else
                           words.push_back( word );
                        if ( inAnchor )
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;
                           else
                              curr_anchorText.push_back( word );
                           }
                        }
                     else if ( inHead4 )
                        {
                        if ( !isspace( *( start - 1 ) ) && !head4Words.empty() )
                           head4Words.back() += word;
                        else
                           head4Words.push_back( word );
                        if ( !isspace( *( start - 1 ) ) && !words.empty() )
                           words.back() += word;
                        else
                           words.push_back( word );
                        if ( inAnchor )
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;
                           else
                              curr_anchorText.push_back( word );
                           }
                        }
                     else if ( inHead5 )
                        {
                        if ( !isspace( *( start - 1 ) ) && !head5Words.empty() )
                           head5Words.back() += word;
                        else
                           head5Words.push_back( word );
                        if ( !isspace( *( start - 1 ) ) && !words.empty() )
                           words.back() += word;
                        else
                           words.push_back( word );
                        if ( inAnchor )
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;
                           else
                              curr_anchorText.push_back( word );
                           }
                        }
                     else if ( inHead6 )
                        {
                        if ( !isspace( *( start - 1 ) ) && !head6Words.empty() )
                           head6Words.back() += word;
                        else
                           head6Words.push_back( word );
                        if ( !isspace( *( start - 1 ) ) && !words.empty() )
                           words.back() += word;
                        else
                           words.push_back( word );
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
                           words.back() += word;  
                           } 
                        else
                           {
                           curr_anchorText.push_back( word );  
                           words.push_back( word );  
                           }
                        }
                     else
                        {
                        if ( !isspace( *( start - 1) ) ) 
                           words.back() += word;  
                        else 
                           words.push_back( word );  
                        }
                     if ( inBold )
                        {
                        if ( !isspace( *( start - 1 ) ) && !boldWords.empty() )
                           boldWords.back() += word;
                        else
                           boldWords.push_back( word );
                        }
                     else if ( inItalic )
                        {
                        if ( !isspace( *( start - 1 ) ) && !italicWords.empty() )
                           italicWords.back() += word;
                        else
                           italicWords.push_back( word );
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
            else if ( inHead1 )
               {
               head1Words.emplace_back( start, p - start );
               words.emplace_back( start, p - start );
               if ( inAnchor )
                  curr_anchorText.emplace_back( start, p - start );
               }
            else if ( inHead2 )
               {
               head2Words.emplace_back( start, p - start );
               words.emplace_back( start, p - start );
               if ( inAnchor )
                  curr_anchorText.push_back( string( start, p - start )  );
               }
            else if ( inHead3 )
               {
               head3Words.emplace_back( start, p - start );
               words.emplace_back( start, p - start );
               if ( inAnchor )
                  curr_anchorText.emplace_back( start, p - start );
               }
            else if ( inHead4 )
               {
               head4Words.emplace_back( start, p - start );
               words.emplace_back( start, p - start );
               if ( inAnchor )
                  curr_anchorText.emplace_back( start, p - start );
               }
            else if ( inHead5 )
               {
               head5Words.emplace_back( start, p - start );
               words.emplace_back( start, p - start );
               if ( inAnchor )
                  curr_anchorText.emplace_back( start, p - start );
               }
            else if ( inHead6 )
               {
               head6Words.emplace_back( start, p - start );
               words.emplace_back( start, p - start );
               if ( inAnchor )
                  curr_anchorText.emplace_back( start, p - start );
               }
            else if ( inAnchor )
               {
               curr_anchorText.emplace_back( start, p - start );  
               words.emplace_back( start, p - start );  
               }
            else
               words.emplace_back( start, p - start );  
            if ( inBold )
               {
               boldWords.emplace_back( start, p - start );
               }
            else if ( inItalic )
               {
               italicWords.emplace_back( start, p - start );
               }
            }
         if ( p < end && *p == '<' ) 
            continue;  
         else 
            p++;  
         }
      }
   }