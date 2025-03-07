#pragma once
#include "frontier.h"


// Crawl the given URL and store the result in the buffer.
int crawl (const ParsedUrl &url, char *buffer, size_t &pageSize);
