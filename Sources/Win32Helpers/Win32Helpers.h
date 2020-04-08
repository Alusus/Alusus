#if defined(_WIN32)

#ifndef WIN32HELPERS_H
#define WIN32HELPERS_H

#include <string>

/*
    Using a modified solution for "utf8Encode" and "utf8Decode" made by "tfinniga" from 
    "https://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte"
    [Accessed: May 30th, 2019].
*/

// Convert a wide Unicode string to a UTF8 string.
std::string utf8Encode(const std::wstring &wstr);

// Convert a UTF8 string to a wide Unicode String.
std::wstring utf8Decode(const std::string &str);

#endif
#endif
