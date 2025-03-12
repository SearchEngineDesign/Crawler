//stemmer.cpp
//stems words

#include "index.h"
#include <cctype>

//MODIFIES: WORD
string standardize (string word) 
    {
    //sets to lowercase
    for ( size_t i = 0; word[i] != '\0'; i++ )
        word[i] = std::tolower( word[i] );
    //removes punctuation
    string newWord;
    for ( size_t i = 0; word[i] != '\0'; i++ )
        if ( isalnum( word[i] ) )
            newWord.pushBack( word[i] );
    return newWord;
    //TODO: remove accents
    }

static inline void removeS (string word)
    {
    if( word.substr(-1) != (string)"s" )
        return;
    if ( word.substr(-4) == (string)"sses" )
        word.popBack(2);
    if ( word.substr(-3) == (string)"ies" )
        word.popBack(2);
    if ( word.substr(-2) != (string)"ss" ) //word end guaranteed to be s from above
        word.popBack(1);
    }

//algorithm from Algorithm for Suffix Stripping by M.F. Porter
string stem (string word) 
    {
    standardize(word);

    }