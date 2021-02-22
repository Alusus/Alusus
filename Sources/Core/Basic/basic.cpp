/**
 * @file Core/Basic/basic.cpp
 * Contains the global implementations of Basic namespace's declarations.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
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

typedef std::codecvt<WChar,Char,std::mbstate_t> FacetType;

static std::locale utf8Locale("en_US.UTF-8");

static const FacetType& utf8Facet = std::use_facet<FacetType>(utf8Locale);


//============================================================================
// Global Functions

Int compareStr(Char const *str1, Char const *str2)
{
  if (str1 == 0 && str2 == 0) return 0;
  else if (str1 == 0) return -1;
  else if (str2 == 0) return 1;
  else return strcmp(str1, str2);
}


Int compareStr(WChar const *str1, WChar const *str2)
{
  if (str1 == 0 && str2 == 0) return 0;
  else if (str1 == 0) return -1;
  else if (str2 == 0) return 1;
  else return wcscmp(str1, str2);
}


Int compareStr(Char const *str1, Char const *str2, Int size)
{
  if (str1 == 0 && str2 == 0) return 0;
  else if (str1 == 0) return -1;
  else if (str2 == 0) return 1;
  else return strncmp(str1, str2, size);
}


Int compareStr(WChar const *str1, WChar const *str2, Int size)
{
  if (str1 == 0 && str2 == 0) return 0;
  else if (str1 == 0) return -1;
  else if (str2 == 0) return 1;
  else return wcsncmp(str1, str2, size);
}


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
  std::mbstate_t mystate = std::mbstate_t();
  Char const *fromNext;
  WChar *toNext;

  // translate characters:
  utf8Facet.in(mystate, input, input+inputLength, fromNext, output, output+outputSize, toNext);

  processedInputLength = fromNext-input;
  resultedOutputLength = toNext-output;
}


void convertStr(
  WChar const *input, int inputLength, Char *output, int outputSize,
  int &processedInputLength, int &resultedOutputLength
) {
  std::mbstate_t mystate = std::mbstate_t();
  WChar const *fromNext;
  Char *toNext;

  // translate characters:
  utf8Facet.out(mystate, input, input+inputLength, fromNext, output, output+outputSize, toNext);

  processedInputLength = fromNext-input;
  resultedOutputLength = toNext-output;
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
