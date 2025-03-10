#include "Utf8.h"
#include <iostream>

size_t SizeOfUtf8( Unicode c )
   {
   if ( c <= 0x7F )
      return 1;
   else if ( c <= 0x7FF )
      return 2;
   else if ( c <= 0xFFFF ) 
      return 3;
   else
      return 0;
   }

size_t IndicatedLength( const Utf8 *p )
   {
   if (*p <= 0x7F)
      return 1;
   else if ((*p & 0xE0) == 0xC0)
      return 2;
   else if ((*p & 0xF0) == 0xE0)
      return 3;
   else if ((*p & 0xF8) == 0xF0)
      return 4;
   return 1;
   }

Unicode GetUtf8( const Utf8 *p ) {
   // pxx: add for autograder test
   std::cout << ("First byte is 0x%X\n", *p);
   size_t indicatedLength = IndicatedLength(p);
   if ( indicatedLength == 1 )
      {
      if ( *p & 0x80 == 0x00 )
         return *p; // ASCII, 1 byte
      else
        return ReplacementCharacter;
      }

   if ( indicatedLength == 2 ) 
      {
      if ( *p == 0xc0 || *p == 0xc1 )
         return ReplacementCharacter; // Overlong sequence
      if (( *( p + 1 ) & 0xC0 ) == 0x80 )
         return (( *p & 0x1F ) << 6 ) | ( *( p + 1 ) & 0x3F );
      }

   if ( indicatedLength == 3 ) 
      {
      if (*p == 0xe0 && *(p + 1) <= 0x9f) {
         return ReplacementCharacter; // Overlong sequence
      }
      if (( *(p + 1) & 0xC0 ) == 0x80 && ( *(p + 2) & 0xC0 ) == 0x80 )
         return ((*p & 0x0F) << 12) | ((*(p+1) & 0x3F) << 6) | (*(p+2) & 0x3F);
      }

   return ReplacementCharacter;
   }

Unicode GetUtf8( const Utf8 *p, const Utf8 *bound )
   {
   size_t indicatedLength = IndicatedLength(p);
    // Single byte (ASCII)
   if (bound == nullptr || *bound && p + indicatedLength > bound )
      return ReplacementCharacter;
   return GetUtf8(p);
   }

const Utf8 *NextUtf8( const Utf8 *p, const Utf8 *bound )
   {
   Utf8 firstByte = *p;  // first byte
   int length;

   if ( firstByte <= 0x7F )
      length = 1;  
   else if ( firstByte >= 0xC0 && firstByte <= 0xDF )
      length = 2;  
   else if ( firstByte >= 0xE0 && firstByte <= 0xEF )
      length = 3;  
   else if ( firstByte >= 0xF0 && firstByte <= 0xF7 )
      length = 4;
   else if ( firstByte >= 0xF8 && firstByte <= 0xFB )
      length = 5;
   else if ( firstByte >= 0xFC && firstByte <= 0xFD )
      length = 6;  
   else
      return p;  

   for ( int i = 1; i < length; i ++ )
      {
      // invalid continuation byte
      if ( p[i] < 0x80 || p[i] > 0xBF )
         return p;  
      // stop at the bound
      if ( bound && ( p + i >= bound ) )
         return p;  
      }

   return p + length;  
   }

const Utf8 *PreviousUtf8( const Utf8 *p )
   {
   p --;  
   while ( *p > 0x7F && *p < 0xC0 || *p > 0xDF && *p < 0xE0 || *p > 0xEF && *p < 0xF0 || *p > 0xFD )
      p --;  

   return p;
   }

Utf8 *WriteUtf8( Utf8 *p, Unicode c )
   {
   Utf8 firstByte = c >> 12 | 0xE0;
   Utf8 secondByte = c >> 6 & 0x03F;
   secondByte = secondByte | 0x80;
   Utf8 thirdByte = c & 0x003F;
   *p = firstByte;
   *( p + 1 ) = secondByte;
   *( p + 2 ) = thirdByte;
   
//    return p + 3;
   }

// Not part of hw

int StringCompare( const Utf8 *a, const Utf8 *b ) {
    // const size_t len1 = IndicatedLength(a);
    // const size_t len2 = IndicatedLength(b);

    // if (len1 > len2) {
    //     return 1;
    // } else if (len1 < len2) {
    //     return -1;
    // } else {
    //     for (size_t i = 0; i < len1; i++) {
    //         if (a[i] > b[i]) {
    //             return 1;
    //         } else if (a[i] < b[i]) {
    //             return -1;
    //         }
    //     }
    //     return 0;
    // }
}

// Unicode string compare up to 'N' UTF-8 characters (not bytes)
// from two UTF-8 strings.

int StringCompare( const Utf8 *a, const Utf8 *b, size_t N ) {}

// Case-independent compares.

int StringCompareI( const Utf8 *a, const Utf8 *b ) {}
int StringCompareI( const Utf8 *a, const Utf8 *b, size_t N ) {}

Unicode ToLower( Unicode c ) {}

bool IsPunctuation( Unicode c ) {}
bool IsSpace( Unicode c ) {}
bool IsControl( Unicode c ) {}


