#pragma once

#define STRINGIFY(str) #str
#define XSTRINGIFY(str) STRINGIFY(str)
#define LOC_STAMP "File " __FILE__ ", line " XSTRINGIFY(__LINE__) ": "
