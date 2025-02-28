CXX = g++
CXXFLAGS = -std=c++11 -g

OPENSSL_DIR = /opt/homebrew/opt/openssl@3
INCLUDES = -I$(OPENSSL_DIR)/include
LDFLAGS = -L$(OPENSSL_DIR)/lib

all: crawler.exe

crawler.exe: main.cpp parser/HtmlParser.cpp parser/HtmlTags.cpp crawler/crawler.cpp include/string.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ $(LDFLAGS) -lssl -lcrypto -lz -o crawler.exe -g


.PHONY: clean

clean:
	rm -f crawler.exe 
