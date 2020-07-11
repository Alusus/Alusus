/**
 * @file Core/Basic/basic.cpp
 * Contains the global implementations of Basic namespace's declarations.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"
#include <locale>

namespace Core { namespace Basic
{

//============================================================================
// Variables and Types

std::ostream & outStream = std::cout;

std::istream & inStream = std::cin;


#ifdef _WIN32
#include <windows.h>
#else
typedef std::codecvt<WChar,Char,std::mbstate_t> FacetType;
static std::locale utf8Locale("en_US.UTF-8");
static const FacetType& utf8Facet = std::use_facet<FacetType>(utf8Locale);
#endif


//============================================================================
// Global Functions

Bool compareStrSuffix(Char const *str, Char const *suffix)
{
  Word strLen = getStrLen(str);
  Word suffixLen = getStrLen(suffix);
  if (suffixLen >= strLen) return false;
  return compareStr(str + strLen - suffixLen, suffix) == 0;
}


void convertStr(
  Char const *input, int inputLength, WChar *output, int outputSize,
  int &processedInputLength, int &resultedOutputLength
) {
#ifdef _WIN32
  int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, input, inputLength, NULL, 0);
  resultedOutputLength = sizeNeeded < outputSize ? sizeNeeded : outputSize;
  processedInputLength = MultiByteToWideChar(CP_UTF8, 0, input, inputLength, output, resultedOutputLength);
#else
  std::mbstate_t mystate = std::mbstate_t();
  Char const *fromNext;
  WChar *toNext;

  // translate characters:
  utf8Facet.in(mystate, input, input+inputLength, fromNext, output, output+outputSize, toNext);

  processedInputLength = fromNext-input;
  resultedOutputLength = toNext-output;
#endif
}


void convertStr(
  WChar const *input, int inputLength, Char *output, int outputSize,
  int &processedInputLength, int &resultedOutputLength
) {
#ifdef _WIN32
  int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, input, inputLength, NULL, 0, NULL, NULL);
  resultedOutputLength = sizeNeeded < outputSize ? sizeNeeded : outputSize;
  processedInputLength = WideCharToMultiByte(CP_UTF8, 0, input, inputLength, output, resultedOutputLength, NULL, NULL);
#else
  std::mbstate_t mystate = std::mbstate_t();
  WChar const *fromNext;
  Char *toNext;

  // translate characters:
  utf8Facet.out(mystate, input, input+inputLength, fromNext, output, output+outputSize, toNext);

  processedInputLength = fromNext-input;
  resultedOutputLength = toNext-output;
#endif
}


WChar getWideCharFromUtf8(Char const *s)
{
  WChar buf[2];
  SBWSTR(buf).assign(s, 2);
  return *buf;
}


WChar getWideCharFromUtf8(Char c)
{
  Char s[2];
  WChar buf[2];
  s[0] = c;
  s[1] = '\0';
  SBWSTR(buf).assign(s, 2);
  return *buf;
}


Int parseHexDigit(WChar wc)
{
  static WChar zero = getWideCharFromUtf8('0');
  static WChar nine = getWideCharFromUtf8('9');
  static WChar a = getWideCharFromUtf8('a');
  static WChar f = getWideCharFromUtf8('f');
  static WChar bigA = getWideCharFromUtf8('A');
  static WChar bigF = getWideCharFromUtf8('F');
  if (wc >= a && wc <= f) {
    return (wc - a) + 10;
  } else if (wc >= bigA && wc <= bigF) {
    return (wc - bigA) + 10;
  } else if (wc >= zero && wc <= nine) {
    return wc - zero;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("wc"), S("Invalid hex digit"), wc);
  }
}


Int parseHexDigits(WChar const *wc, Word count)
{
  Int val = 0;
  while (count > 0) {
    val *= 16;
    val += parseHexDigit(*wc);
    ++wc;
    --count;
  }
  return val;
}


void printIndents(OutStream &stream, int indents)
{
  for (Int i=0; i < indents; ++i) {
    stream << S(" ");
  }
}

} } // namespace
