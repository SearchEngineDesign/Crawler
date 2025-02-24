CXX = g++
CXXFLAGS = -std=c++11 -g -DLinux

all: HtmlParserDemo

test1: HtmlParserDemo NYTimes.html
	./HtmlParserDemo NYTimes.html > HYT.txt

head: HtmlParserDemo NYTimes.html
	./HtmlParserDemo h NYTimes.html > Head.txt

title: HtmlParserDemo NYTimes.html
	./HtmlParserDemo t NYTimes.html > TitleWords.txt

words: HtmlParserDemo NYTimes.html
	./HtmlParserDemo w NYTimes.html > Words.txt

base: HtmlParserDemo NYTimes.html
	./HtmlParserDemo b NYTimes.html > Base.txt

links: HtmlParserDemo NYTimes.html
	./HtmlParserDemo a NYTimes.html > Links.txt

bold: HtmlParserDemo NYTimes.html
	./HtmlParserDemo d NYTimes.html > Bold.txt

italic: HtmlParserDemo NYTimes.html
	./HtmlParserDemo i NYTimes.html > Italic.txt

test: HtmlParserDemo NYTimes.html
	./HtmlParserDemo t NYTimes.html > TitleWords.txt
	diff -B TitleWords.correct.txt TitleWords.txt
	./HtmlParserDemo w NYTimes.html > Words.txt
	diff -B Words.correct.txt Words.txt 
	./HtmlParserDemo a NYTimes.html > Links.txt
	diff -B Links.correct.txt Links.txt 
	./HtmlParserDemo b NYTimes.html > Base.txt
	diff -B Base.correct.txt Base.txt 

HtmlParserDemo: HtmlTags.h HtmlTags.cpp HtmlParser.h HtmlParser.cpp HtmlParserDemo.cpp string.cpp
	$(CXX) $(CXXFLAGS) -o HtmlParserDemo HtmlParserDemo.cpp HtmlTags.cpp HtmlParser.cpp string.cpp

clean:
	rm -f HtmlParserDemo TitleWords.txt Words.txt Links.txt Base.txt HYT.txt Bold.txt Italic.txt