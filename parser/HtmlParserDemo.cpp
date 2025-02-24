// HtmlParserDemo.cpp
// Nicole Hamilton, nham@umich.edu

// This is a simple demo of the HTML parser.  It reads a file
// containing presumed HTML into memory, creates an HtmlParser object
// to parse it, then prints out what it found.  By default, the output
// is a simplified HTML page representing the content.  Options allow
// for extracting only a portion of the contents.

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "HtmlParser.h"

enum class Options
   {
   PrintSimplePage,
   PrintWords,
   PrintTitleWords,
   PrintLinks,
   PrintBase,
   PrintHead,
   PrintBold,
   PrintItalic
   };

Options option = Options::PrintSimplePage;

void PrintParse( HtmlParser &parser )
   {
   switch ( option )
      {
      case Options::PrintSimplePage:
         // Print a simplified page

         std::cout << "<!DOCTYPE html>" << std::endl << "<html>" << std::endl;
         
         std::cout << "<title>" << std::endl;
         for ( auto w : parser.titleWords )
            std::cout << w << " ";
         std::cout << std::endl << "</title>" << std::endl << std::endl;

         std::cout << "<h>" << std::endl << "Headings" << std::endl << "</h>" << std::endl;
         std::cout << "<h1>" << std::endl;
         for ( auto w : parser.head1Words )
            std::cout << w << " ";
         std::cout << std::endl << "</h1>" << std::endl << std::endl;

         std::cout << "<h2>" << std::endl;
         for ( auto w : parser.head2Words )
            std::cout << w << " ";
         std::cout << std::endl << "</h2>" << std::endl << std::endl;

         std::cout << "<h3>" << std::endl;
         for ( auto w : parser.head3Words )
            std::cout << w << " ";
         std::cout << std::endl << "</h3>" << std::endl << std::endl;

         std::cout << "<h4>" << std::endl;
         for ( auto w : parser.head4Words )
            std::cout << w << " ";
         std::cout << std::endl << "</h4>" << std::endl << std::endl;     

         std::cout << "<h5>" << std::endl;
         for ( auto w : parser.head5Words )
            std::cout << w << " ";
         std::cout << std::endl << "</h5>" << std::endl << std::endl;

         std::cout << "<h6>" << std::endl;
         for ( auto w : parser.head6Words )
            std::cout << w << " ";
         std::cout << std::endl << "</h6>" << std::endl << std::endl;

         std::cout << "<body style=\"font-family:Verdana,Arial,Helvetica,sans-serif;font-size:0.9em\">"
               << std::endl << "<h>" << std::endl << "Body text" << std::endl << "</h>" << std::endl;
         
         std::cout << "<p>" << std::endl;
         for ( auto w : parser.words )
            std::cout << w << " ";
         std::cout << std::endl << "<p>" << std::endl << std::endl;

         std::cout << "<b>" << std::endl;
         for ( auto w : parser.boldWords )
            std::cout << w << " ";
         std::cout << std::endl << "</b>" << std::endl << std::endl;

         std::cout << "<i>" << std::endl;
         for ( auto w : parser.italicWords )
            std::cout << w << " ";
         std::cout << std::endl << "</i>" << std::endl << std::endl;

         std::cout << "<base>" << std::endl << "Base = " << "<a href=\"" << parser.base <<
            "\">" << parser.base << "</a>" << std::endl << "</base>" << std::endl << std::endl;

         std::cout << "<h>" << std::endl << "Links" << std::endl << "</h>" << std::endl;
         for ( auto link : parser.links )
            {
            std::cout << "<p>" << std::endl << "<a href=\"" << link.URL << "\">" << link.URL << "</a> ( " ;
            for ( auto w : link.anchorText )
               std::cout << w << " ";
            std::cout << ")" << std::endl << "</p>" <<  std::endl;
            }

         std::cout << "</body>" << std::endl << "</html>" << std::endl;
         break;

      case Options::PrintWords:
         for ( auto w : parser.words )
            std::cout << w << std::endl;
         break;

      case Options::PrintTitleWords:
         for ( auto w : parser.titleWords )
            std::cout << w << std::endl;
         break;

      case Options::PrintLinks:
         for ( auto link : parser.links )
            {
            std::cout << link.URL << " ( ";
            for ( auto w : link.anchorText )
               std::cout << w << " ";
            std::cout << ")" << std::endl;
            }
         break;

      case Options::PrintBase:
         std::cout << parser.base << std::endl;
      
      case Options::PrintHead:
         for ( auto w : parser.head1Words )
            std::cout << w << std::endl;
         for ( auto w : parser.head2Words )
            std::cout << w << std::endl;
         for ( auto w : parser.head3Words )
            std::cout << w << std::endl;
         for ( auto w : parser.head4Words )
            std::cout << w << std::endl;
         for ( auto w : parser.head5Words )
            std::cout << w << std::endl;
         for ( auto w : parser.head6Words )
            std::cout << w << std::endl;
         break;

      case Options::PrintBold:
         for ( auto w : parser.boldWords )
            std::cout << w << std::endl;
         break;
      
      case Options::PrintItalic:
         for ( auto w : parser.italicWords )
            std::cout << w << std::endl;
         break;
      }
   }


char *ReadFile( char *filename, size_t &fileSize )
   {
   // Read the file into memory.
   // You'll soon learn a much more efficient way to do this.

   // Attempt to Create an istream and seek to the end
   // to get the size.
   std::ifstream ifs( filename, std::ios::ate | std::ios::binary );
   if ( !ifs.is_open( ) )
      return nullptr;
   fileSize = ifs.tellg( );

   // Allocate a block of memory big enough to hold it.
   char *buffer = new char[ fileSize ];

   // Seek back to the beginning of the file, read it into
   // the buffer, then return the buffer.
   ifs.seekg( 0 );
   ifs.read( buffer, fileSize );
   return buffer;
   }


bool ValidOption( const char *o )
   {
   // Must be exactly one character.
   if ( !o[ 0 ] || o[ 1 ] )
      return false;

   switch ( o[ 0 ] )
      {
      case 'w':
         option = Options::PrintWords;
         break;
      case 't':
         option = Options::PrintTitleWords;
         break;
      case 'a':
         option = Options::PrintLinks;
         break;
      case 'b':
         option = Options::PrintBase;
         break;
      case 'h':
         option = Options::PrintHead;
         break;
      case 'd':
         option = Options::PrintBold;
         break;
      case 'i':
         option = Options::PrintItalic;
         break;
      default:
         return false;
      }
   return true;
   }


int main( int argc, char **argv )
   {
   if ( !( argc == 2 || argc == 3 && ValidOption( argv[ 1 ] ) ) )
      {
      std::cout << "Usage:  HtmlParser [wtab] filename" << std::endl <<
              std::endl <<
              "By default, prints a simplified HTML page.  Options allow" << std::endl <<
              "for only a section of the content to be printed.  Only one" << std::endl <<
              "wtab option allowed at a time." << std::endl <<
              std::endl <<
              "   w  List the words, one per line." << std::endl <<
              "   t  List the title words, one per line." << std::endl << 
              "   a  List the links as URL( anchor text ), one per line." << std::endl <<
              "   b  Print any base that was found." << std::endl <<
              "   h  List the heading words, one per line." << std::endl <<
              "   d  List the bold (strong) words, one per line." << std::endl <<
              "   i  List the italic (emphasis) words, one per line." << std::endl;
      exit( 1 );
      }

   size_t fileSize = 0;
   char *buffer = ReadFile( argv[ 1 + ( argc == 3 ) ], fileSize );
   if ( !buffer )
      {
      std::cerr << "Could not open the file." << std::endl;
      exit( 1 );
      }

   HtmlParser parser( buffer, fileSize );
   PrintParse( parser );
   delete [ ] buffer;
   }