test:  HtmlParserDemo NYTimes.html
	./HtmlParserDemo t NYTimes.html > TitleWords.txt
	./HtmlParserDemo w NYTimes.html > Words.txt
	./HtmlParserDemo a NYTimes.html > Links.txt
	./HtmlParserDemo b NYTimes.html > Base.txt

HtmlParserDemo:  HtmlTags.h HtmlTags.cpp HtmlParser.h HtmlParser.cpp HtmlParserDemo.cpp
	g++ HtmlParserDemo.cpp HtmlTags.cpp HtmlParser.cpp -DLinux -o HtmlParserDemo -std=c++11

clean:
	rm -f HtmlParserDemo TitleWords.txt Words.txt Links.txt Base.txt