// index.h

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cmath>
#include <bitset>

#include "../include/string.h"
#include "../include/vector.h"
#include "../include/Utf8.h"
#include "hash/HashTable.h"
#include "../parser/HtmlParser.h"

// DocumentAttributes
enum class Token
{
    EoD,    // end-of-document token 0000
    Anchor, // token in anchor text 0001
    URL,    // url token 0010
    Title,  // title text token 0011
    Body    // body text token 0100
};

// WordAttributes (Type-specific information)
enum class Style
{
    EOD,     // represented by 00
    Normal,  // represented by 01
    Heading, // represented by 10
    // Italic,  
    // Bold     
};

typedef union Attributes
{
    Style Word;
    Token Document;
};

typedef size_t Location; // Location 0 is the null location.
typedef size_t FileOffset;

class Post
{
private:
    FileOffset delta;

public:
    virtual Location GetStartLocation();
    virtual Location GetEndLocation();
    virtual Attributes GetAttributes();
};

struct CommonHeader {
    size_t occurrenceCount;
    size_t documentCount;
    Token token_type;
};

class PostingList
{
private:

    CommonHeader header;
    struct PostingListIndex
    {
        FileOffset Offset;
        Location PostLocation;
    };
    PostingListIndex *index;
    virtual char *GetPostingList(); // ?

public:
    virtual Post *Seek(Location);
};

class Index
{
public:
    Location WordsInIndex = 0,
             DocumentsInIndex = 0,
             LocationsInIndex,
             MaximumLocation;

    ISRWord *OpenISRWord(char *word);
    ISRWord *OpenISREndDoc();

    HashTable<string, PostingList> *getDict()
    {
        return &dict;
    }

private:
    HashTable<string, PostingList> dict;

    // string titleMarker = string("@");
    // string anchorMarker = string("$");
    // string urlMarker = string("#");
    // string eodMarker = string("%");
};

class Dictionary
{
public:
    ISR *OpenIsr(char *token);
    // The first line of index.txt, in order
    Location GetNumberOfWords();
    Location GetNumberOfUniqueWords(); // number of posting list
    Location GetNumberOfDocuments();
};

class ISR
{
public:
    virtual Post *Next();
    virtual Post *NextDocument();
    virtual Post *Seek(Location target);
    virtual Location GetStartLocation();
    virtual Location GetEndLocation();
};

class ISRWord : public ISR
{
public:
    unsigned GetDocumentCount();
    unsigned GetNumberOfOccurrences();
    virtual Post *GetCurrentPost();
};

class ISREndDoc : public ISRWord
{
public:
    unsigned GetDocumentLength();
    unsigned GetTitleLength();
    unsigned GetUrlLength();
};

class ISROr : public ISR
{
public:
    ISR **Terms;
    unsigned NumberOfTerms;
    Location GetStartLocation();
    Location GetEndLocation();
    Post *Seek(Location target);
    Post *Next();
    Post *NextDocument();

private:
    unsigned nearestTerm;
    Location nearestStartLocation, nearestEndLocation;
};

class ISRAnd : public ISR
{
public:
    ISR **Terms;
    unsigned NumberOfTerms;
    Post *Seek(Location target);
    Post *Next()
    {
        return Seek(nearestStartLocation + 1);
    }

private:
    unsigned nearestTerm, farthestTerm;
    Location nearestStartLocation, nearestEndLocation;
};

class ISRPhrase : public ISR
{
public:
    ISR **Terms;
    unsigned NumberOfTerms;
    Post *Seek(Location target);
    Post *Next()
    {
        // Finds overlapping phrase matches.
        return Seek(nearestStartLocation + 1);
    }

private:
    unsigned nearestTerm, farthestTerm;
    Location nearestStartLocation, nearestEndLocation;
};


// class ISRContainer : public ISR
// {
// public:
//     ISR **Contained,*Excluded;
//     ISREndDoc *EndDoc;
//     unsigned CountContained, CountExcluded;
//     Post *Seek( Location target );
//     Post *Next( )
//     {
//     Seek( Contained[ nearestContained ]->GetStartlocation( ) + 1 );
//     }

// private:
//     unsigned nearestTerm, farthestTerm;
//     Location nearestStartLocation, nearestEndLocation;
// };
