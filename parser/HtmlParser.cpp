// HtmlParser.cpp
// Nicole Hamlton, nham@umich.edu

// If you don't define the HtmlParser class methods inline in
// HtmlParser.h, you may do it here.

#include "HtmlParser.h"
#include "HtmlTags.h"

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
                     Link curr_link( url );  
                     links.pushBack( curr_link );  
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
                     links.pushBack( url );  
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
                              links.pushBack( curr_link );  
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
                           titleWords.pushBack( word );  

                        if ( inAnchor ) 
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;  
                           else
                              curr_anchorText.pushBack( word );  
                           }
                        }
                     else if ( inHead1 )
                        {
                        if ( !isspace( *( start - 1 ) ) && !head1Words.empty() )
                           head1Words.back() += word;
                        else
                           head1Words.pushBack( word );
                        if ( !isspace( *( start - 1 ) ) && !words.empty() )
                           words.back() += word;
                        else
                           words.pushBack( word );
                        if ( inAnchor )
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;
                           else
                              curr_anchorText.pushBack( word );
                           }
                        }
                     else if ( inHead2 )
                        {
                        if ( !isspace( *( start - 1 ) ) && !head2Words.empty() )
                           head2Words.back() += word;
                        else
                           head2Words.pushBack( word );
                        if ( !isspace( *( start - 1 ) ) && !words.empty() )
                           words.back() += word;
                        else
                           words.pushBack( word );
                        if ( inAnchor )
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;
                           else
                              curr_anchorText.pushBack( word );
                           }
                        }
                     else if ( inHead3 )
                        {
                        if ( !isspace( *( start - 1 ) ) && !head3Words.empty() )
                           head3Words.back() += word;
                        else
                           head3Words.pushBack( word );
                        if ( !isspace( *( start - 1 ) ) && !words.empty() )
                           words.back() += word;
                        else
                           words.pushBack( word );
                        if ( inAnchor )
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;
                           else
                              curr_anchorText.pushBack( word );
                           }
                        }
                     else if ( inHead4 )
                        {
                        if ( !isspace( *( start - 1 ) ) && !head4Words.empty() )
                           head4Words.back() += word;
                        else
                           head4Words.pushBack( word );
                        if ( !isspace( *( start - 1 ) ) && !words.empty() )
                           words.back() += word;
                        else
                           words.pushBack( word );
                        if ( inAnchor )
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;
                           else
                              curr_anchorText.pushBack( word );
                           }
                        }
                     else if ( inHead5 )
                        {
                        if ( !isspace( *( start - 1 ) ) && !head5Words.empty() )
                           head5Words.back() += word;
                        else
                           head5Words.pushBack( word );
                        if ( !isspace( *( start - 1 ) ) && !words.empty() )
                           words.back() += word;
                        else
                           words.pushBack( word );
                        if ( inAnchor )
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;
                           else
                              curr_anchorText.pushBack( word );
                           }
                        }
                     else if ( inHead6 )
                        {
                        if ( !isspace( *( start - 1 ) ) && !head6Words.empty() )
                           head6Words.back() += word;
                        else
                           head6Words.pushBack( word );
                        if ( !isspace( *( start - 1 ) ) && !words.empty() )
                           words.back() += word;
                        else
                           words.pushBack( word );
                        if ( inAnchor )
                           {
                           if ( !isspace( *( start - 1 ) ) && !curr_anchorText.empty() )
                              curr_anchorText.back() += word;
                           else
                              curr_anchorText.pushBack( word );
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
                           curr_anchorText.pushBack( word );  
                           words.pushBack( word );  
                           }
                        }
                     else
                        {
                        if ( !isspace( *( start - 1) ) ) 
                           words.back() += word;  
                        else 
                           words.pushBack( word );  
                        }
                     if ( inBold )
                        {
                        if ( !isspace( *( start - 1 ) ) && !boldWords.empty() )
                           boldWords.back() += word;
                        else
                           boldWords.pushBack( word );
                        }
                     else if ( inItalic )
                        {
                        if ( !isspace( *( start - 1 ) ) && !italicWords.empty() )
                           italicWords.back() += word;
                        else
                           italicWords.pushBack( word );
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
            string newWord = string( start, p - start );
            if ( inTitle )
               {
               titleWords.pushBack( newWord );  
               if ( inAnchor )
                  curr_anchorText.pushBack( newWord );  
               }
            else if ( inHead1 )
               {
               head1Words.pushBack( newWord );
               words.pushBack( newWord );
               if ( inAnchor )
                  curr_anchorText.pushBack( newWord );
               }
            else if ( inHead2 )
               {
               head2Words.pushBack( newWord );
               words.pushBack( newWord );
               if ( inAnchor )
                  curr_anchorText.pushBack( newWord  );
               }
            else if ( inHead3 )
               {
               head3Words.pushBack( newWord );
               words.pushBack( newWord );
               if ( inAnchor )
                  curr_anchorText.pushBack( newWord );
               }
            else if ( inHead4 )
               {
               head4Words.pushBack( newWord );
               words.pushBack( newWord );
               if ( inAnchor )
                  curr_anchorText.pushBack( newWord );
               }
            else if ( inHead5 )
               {
               head5Words.pushBack( newWord );
               words.pushBack( newWord );
               if ( inAnchor )
                  curr_anchorText.pushBack( newWord );
               }
            else if ( inHead6 )
               {
               head6Words.pushBack( newWord );
               words.pushBack( newWord );
               if ( inAnchor )
                  curr_anchorText.pushBack( newWord );
               }
            else if ( inAnchor )
               {
               curr_anchorText.pushBack( newWord );  
               words.pushBack( newWord );  
               }
            else
               words.pushBack( newWord );  
            if ( inBold )
               {
               boldWords.pushBack( newWord );
               }
            else if ( inItalic )
               {
               italicWords.pushBack( newWord );
               }
            }
         if ( p < end && *p == '<' ) 
            continue;  
         else 
            p++;  
         }
      }
   }