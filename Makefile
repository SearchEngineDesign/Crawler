CXX = g++
CXXFLAGS = -std=c++11 -g

OPENSSL_DIR = /opt/homebrew/opt/openssl@3
INCLUDES = -I$(OPENSSL_DIR)/include
LDFLAGS = -L$(OPENSSL_DIR)/lib

all: LinuxGetSsl #LinuxGetUrl

LinuxGetSsl: main.cpp parser/HtmlParser.cpp parser/HtmlTags.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ $(LDFLAGS) -lssl -lcrypto -lz -o LinuxGetSsl -g

LinuxGetUrl: LinuxGetUrl.cpp
	$(CXX) $(CXXFLAGS) $^ -o LinuxGetUrl

.PHONY: clean

clean:
	rm -f LinuxGetSsl LinuxGetUrl
