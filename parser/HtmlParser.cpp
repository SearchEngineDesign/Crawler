// HtmlParser.cpp
// Nicole Hamlton, nham@umich.edu

// If you don't define the HtmlParser class methods inline in
// HtmlParser.h, you may do it here.

#include "HtmlParser.h"
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <stack>
using namespace std;
// std::vector< std::string > words, titleWords;
// std::vector< Link > links;
// std::string base;


// char * getEndOfTag(char * buffer, size_t length, size_t & i){
//     while ( i < length && buffer[i] != '>' ){
//         i++;
//     }
//     return buffer+ i;
// }


HtmlParser::HtmlParser(const char *buffer, size_t length)
{
    size_t i = 0;
    bool inTitle = false;
    bool indiscard = false;

    int AnchorDepth = 0;
    stack<string> urlStack;
    stack<vector<string>> anchorTextStack;

    while ( i < length){
        if ( buffer[i] == '<')
        { 
            i++;
            size_t j = i;
            bool isClosingTag = (buffer[j] == '/'); 
            if ( isClosingTag ) {
                j++;
                i = j;
            }

            while ( buffer[j] != '>' && buffer[j] != '/' && !isspace(buffer[j]) && j < length ) {
                j++;
            }
            // string tmp(buffer+i,buffer+j);
            
            DesiredAction action = LookupPossibleTag(buffer+i,buffer+j); 
            if ( isClosingTag ){

                switch (action)
                {
                case DesiredAction::Title:
                    inTitle = false;
                    break;
                case DesiredAction::Anchor:
                    if ( AnchorDepth > 0){
                        AnchorDepth--;
                        Link curr_link(urlStack.top());
                        curr_link.anchorText = anchorTextStack.top();
                        links.push_back(curr_link);
                        urlStack.pop();
                        anchorTextStack.pop();
                    }
                    else{
                        while ( i<length && buffer[i] != '>' ){
                            i++;
                        }
                        i ++;
                        continue;
                    }
                    break;
                case DesiredAction::DiscardSection:
                    indiscard = false;
                    break;    
                default:
                    break;
                }

            }else{

                bool toSkip = false;

                switch (action)
                {
                case DesiredAction::Title:
                    i = j;
                    if ( inTitle ){
                        while ( i<length && buffer[i] != '>' ){
                            i++;
                        }
                        i ++;
                        continue;
                    }
                    inTitle = true;
                    break;
                case DesiredAction::Base:
                    i = j;
                    while ( i < length && base.empty() ){
                        if ( memcmp(buffer + i, "href=", 5) == 0 && buffer[i+5] == '"'){
                            i += 5;
                            while (buffer[i] != '"'){
                                i ++;
                            }
                            i ++;
                            j =i;
                            while ( j<length && buffer[j] != '"'){
                                j++;
                            }


                            base = string(buffer+ i, buffer+ j);
                            break;
                        }
                        else if ( memcmp(buffer+i, "/>", 2) == 0 ){
                            break;
                        }
                        i ++;
                    }
                    i = j;
                    break;
                case DesiredAction::Embed: 
                    {
                    string embed_href="";
                    while ( i < length && buffer[i] != '>' ){
                        if( memcmp(buffer + i, "src=", 4) == 0 && buffer[i+4] == '"' ){
                            i += 5;
                            j = i;
                            while (buffer[j] != '"'){
                                j++;
                            }
                            embed_href = string(buffer + i, buffer + j);
                            break;
                        }
                        i ++;
                    }
                    i = j;
                    if ( !embed_href.empty() ){
                        // Link curr_link(url);
                        links.emplace_back(embed_href);
                    }
                    break;
                    }

                case DesiredAction::Anchor:

                    {
                    i = j;
                    string anchor_href = "";
                    while ( i < length && buffer[i] != '>' ){
                        if( strncmp(buffer + i, "href=", 5) == 0 && buffer[i+5] == '"' ){
                            if ( AnchorDepth == 1){
                                AnchorDepth--;
                                Link curr_link(urlStack.top());
                                curr_link.anchorText = anchorTextStack.top();
                                links.push_back(curr_link);
                                urlStack.pop();
                                anchorTextStack.pop();
                            }
                            AnchorDepth++;
                            i += 6;
                            j = i;
                            while ( j< length && buffer[j] != '"') j++;
                            anchor_href = string(buffer + i, buffer + j);
                            break;
                        }
                        i ++;
                    }
                    i = j;
                    if ( !anchor_href.empty() ){
                        urlStack.push(anchor_href);
                        anchorTextStack.push({});
                    }
                    break;
                    }
                case DesiredAction::Comment:
                    i = j;
                    while ( i < length-2 && !(buffer[i] == '-' && buffer[i+1] == '-' && buffer[i+2] == '>')){
                        i++;
                    }
                    break;
                case DesiredAction::DiscardSection:
                    i = j;
                    indiscard = true;
                    break;
                case DesiredAction::OrdinaryText:
                    {
                    size_t start = i-1;
                    while ( i < length-1 && buffer[i] != '<' && !isspace(buffer[i]) && !(buffer[i] == '/' && buffer[i+1] == '>')){
                        i++;
                    }
                    string word(buffer + start, buffer + i);
                    if (!word.empty() && !indiscard ) {
                        if ( inTitle ){
                            if (!isspace(buffer[start-1]) && !titleWords.empty() ){
                                titleWords.back() += word;
                            }else{
                                titleWords.push_back(word);
                            }
                            if ( AnchorDepth != 0 ) {
                                if ( !isspace(buffer[start-1]) && !anchorTextStack.top().empty() ){
                                    anchorTextStack.top().back() += word;
                                }else{
                                    anchorTextStack.top().push_back(word);
                                }
                            }
                        }
                        else if ( AnchorDepth != 0 ) {
                            if ( !isspace(buffer[start-1]) && !anchorTextStack.top().empty() ){
                                anchorTextStack.top().back() += word;
                                words.back() += word;
                            }else{
                                anchorTextStack.top().push_back(word);
                                words.push_back(word);
                            }
                        }else{
                            if (!isspace(buffer[start-1]) ){
                                words.back() += word;
                            }else{
                                words.push_back(word);
                            }
                            
                        }
                    }

                    // where does this jump to?
                    // continue;
                    toSkip = true;
                    break;
                    }
                default:
                    break;
                }

                if ( toSkip )
                    continue;


                // if ( action == DesiredAction::Title ){
                    
                // }
                // else if ( action == DesiredAction::Base ){
                    
                    
                // }
                // else if ( action == DesiredAction::Embed ){
                // }
                // else if ( action == DesiredAction::Anchor ){
                    
                // }
                // else if ( action == DesiredAction::Comment ){
                    
                // }
                // else if ( action == DesiredAction::DiscardSection ){
                 
                // }
                // else if ( action == DesiredAction::OrdinaryText ){
                    
                // }

            }
            while ( i<length && buffer[i] != '>' ){
                i++;
            }
            i ++;
        }
        else
        {
            size_t j = i;
            while ( buffer[j] != '<' && !isspace(buffer[j]) && !(buffer[j] == '/' && buffer[j+1] == '>' && j < length )){
                j++;
            }
            string word(buffer + i, buffer + j);
            if ( !word.empty() && !indiscard ) {
                if ( inTitle ){
                    titleWords.push_back(word);
                    if ( AnchorDepth != 0 ){
                        anchorTextStack.top().push_back(word);
                    }
                }
                else if ( AnchorDepth != 0 ) {
                    anchorTextStack.top().push_back(word);
                    words.push_back(word);
                }else{
                    words.push_back(word);
                }
            }
            if ( buffer[j] == '<' && j < length ) {
                i = j;  
            } else {
                i = j + 1;  
            }
        }
    }
}