//stemmer.cpp
//stems words

#include "index.h"
#include <algorithm>
#include <cctype>

//MODIFIES: WORD
void toLower (string word) 
    {
    for ( size_t i = 0; word[i] != '\0'; i++ )
        {
        word[i] = std::tolower( word[i] );
        }
    }

string removePunctuation (string word)
    {
    string newWord;
    for ( size_t i = 0; word[i] != '\0'; i++ )
        if ( isalnum( word[i] ) )
            newWord.pushBack( word[i] );
    return newWord;
    }

string stem (string word) {

}