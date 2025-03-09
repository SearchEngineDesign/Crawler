//index.h

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "../include/string.h"
#include "../include/vector.h"
#include "hash/HashTable.h"
#include "../parser/HtmlParser.h"

enum class Token {
    EoD,            //end-of-document token
    Anchor,         //token in anchor text
    URL,            //url token
    Title,          //title text token
    Body            //body text token
};

enum class Style {
    Normal,     //represented by 00
    Heading,    //represented by 11
    Italic,     //represented by 01
    Bold        //represented by 10
};

typedef size_t Location; // Location 0 is the null location.
typedef size_t FileOffset;

typedef union Attributes {
    string Word;
    string Document;
} Attributes;

class Post {
public:
    virtual Location getLocation();
    virtual Attributes getAttributes(); 

private:
    //Variable byte size char array, to be encoded in utf-8.
    //Structure: n bits to encode the offset + 2 bits to encode style (for word tokens)
    //n bits to encode the offset + log(count) bits to number of times the anchor text 
        //the word belongs to has occurred with the URL it links to (for anchor text)
    //n bits to encode the EOF + n bits to encode an index to the corresponding URL for EOF tokens
    char *data;
};

class PostingList {
public:
    //virtual Post *Seek( Location );
    void appendDelta(size_t delta); //title token
    void appendDelta(size_t delta, uint8_t style); //body token
    void appendEOD(size_t delta, size_t docIndex); //EOF token
private:
    //Common header
    size_t useCount;        //number of times token occurs
    size_t documentCount;   //number of documents containing token
    Token type;             //variety of token
    size_t listSize;        //size of list (in bytes)

    //Type-specific data

    //Index
    string index;

    //Posts
    vector<Post> list;

    //Sentinel
    char sentinel = '\0';
};

class Index {
public:
    //Constructor should take in parsed HTML and add it to the index.
    void addDocument(HtmlParser &parser);
    vector<string> documents;
    Location WordsInIndex = 0, 
    DocumentsInIndex = 0, 
    LocationsInIndex = 0,
    MaximumLocation = 0;

    Index() {}
private:
    //HashBlob dictBlob;
    HashTable<string, PostingList> dict;

    string titleMarker = string("@");
    string anchorMarker = string("$");
    string urlMarker = string("#");
    string eodMarker = string("%");
};

class ISR {
public:
    virtual Post *Next( );
    virtual Post *NextDocument( );
    virtual Post *Seek( Location target );
    virtual Location GetStartLocation( );
    virtual Location GetEndLocation( );
};

class ISRWord : public ISR {
public:
    unsigned GetDocumentCount( );
    unsigned GetNumberOfOccurrences( );
    virtual Post *GetCurrentPost( );
};