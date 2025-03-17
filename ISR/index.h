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
    EoD,    // end-of-document token
    Anchor, // token in anchor text
    URL,    // url token
    Title,  // title text token
    Body    // body text token
};

// WordAttributes
enum class Style
{
    Normal,  // represented by 00
    Heading, // represented by 11
    // Italic,  // represented by 01
    // Bold     // represented by 10
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

class PostingList
{
private:
    struct PostingListIndex
    {
        FileOffset Offset;
        Location PostLocation;
    };
    PostingListIndex *index;
    virtual char *GetPostingList();

public:
    virtual Post *Seek(Location);
    
};

class PostingList
{
public:
    // virtual Post *Seek( Location );
    void appendTitleDelta(size_t delta);                // title token
    void appendBodyDelta(size_t delta, uint8_t style);  // body token
    void appendEODDelta(size_t delta, size_t docIndex); // EOF token

    // Construct empty posting list for string str_in
    PostingList(string &str_in, Token type_in) : index(str_in), type(type_in), useCount(1) {}

    // Get size of post list (in bytes)
    size_t getListSize()
    {
        return sizeof(list);
    }

    string getIndex()
    {
        return index;
    }

private:
    // Common header
    size_t useCount;      // number of times token occurs
    size_t documentCount; // number of documents containing token
    Token type;           // variety of token

    // Type-specific data

    // Index
    string index;

    // Posts
    vector<Post> list;

    // Sentinel
    char sentinel = '\0';
};

class Index
{
public:
    // Constructor should map filename to memory
    Index(const char *filename);

    // addDocument should take in parsed HTML and add it to the index.
    void addDocument(HtmlParser &parser);
    vector<string> documents;
    size_t WordsInIndex = 0,
           DocumentsInIndex = 0;

    Index() {}

    HashTable<string, PostingList> *getDict()
    {
        return &dict;
    }

private:
    HashTable<string, PostingList> dict;

    string titleMarker = string("@");
    string anchorMarker = string("$");
    string urlMarker = string("#");
    string eodMarker = string("%");
};

struct IndexHandler
{
    int fd;
    void *map;
    Index *index;
    int fsize = 0;
    IndexHandler(const char *filename);
    ~IndexHandler()
    {
        if (msync(map, fsize, MS_SYNC) == -1)
        {
            perror("Error syncing memory to file");
            munmap(map, fsize);
        }
        if (munmap(map, fsize == -1))
        {
            perror("Error un-mmapping the file");
        }
        close(fd);
    }
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