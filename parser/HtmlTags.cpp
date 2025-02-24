#include <ctype.h>
#include <cassert>
#include <iostream>
#include <cstring> 
#include "HtmlTags.h"

// name points to beginning of the possible HTML tag name.
// nameEnd points to one past last character.
// Comparison is case-insensitive.
// Use a binary search.
// If the name is found in the TagsRecognized table, return
// the corresponding action.
// If the name is not found, return OrdinaryText.

DesiredAction LookupPossibleTag( const char *name, const char *nameEnd )
   {
      // Your code here.
      nameEnd = nameEnd ? nameEnd : name + strlen( name );  
      size_t length = nameEnd - name;  
      if ( length > LongestTagLength )
         return DesiredAction::OrdinaryText;  
      char lowerName[ length + 1 ];  
      for ( size_t i = 0; i < length; i++ )
         lowerName[ i ] = tolower( static_cast<unsigned char>( name[ i ] ) );  
      
      lowerName[length] = '\0';  
      int left = 0;  
      int right = NumberOfTags - 1;  
      while( left <= right )
         {
         int mid = left + ( right - left ) / 2;  
         const char *tag = TagsRecognized[ mid ].Tag;  
         
         int cmp = strcmp( tag, lowerName );  
         if ( cmp == 0 )
            return TagsRecognized[ mid ].Action;  
         else if ( cmp < 0 )
            left = mid + 1;  
         else
            right = mid - 1;  
         }
      return DesiredAction::OrdinaryText;  
   }
