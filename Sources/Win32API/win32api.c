#include <Windows.h>
#include <stdlib.h>

#include "win32api.h"

/*
    Using a modified solution for "utf8Encode" and "utf8Decode" made by "tfinniga" from 
    "https://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte"
    [Accessed: May 30th, 2019].
*/

// Convert a wide Unicode string to a UTF8 string.
std::string utf8Encode(const std::wstring &wstr) {
  if (wstr.empty()) {
    return std::string();
  }
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
  std::string strTo(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
  return strTo;
}

// Convert a UTF8 string to a wide Unicode String.
std::wstring utf8Decode(const std::string &str) {
  if (str.empty()) {
    return std::wstring();
  }
  int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
  std::wstring wstrTo(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
  return wstrTo;
}
