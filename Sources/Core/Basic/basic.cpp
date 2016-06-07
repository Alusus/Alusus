/**
 * @file Core/Basic/basic.cpp
 * Contains the global implementations of Basic namespace's declarations.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <locale>

namespace Core { namespace Basic
{

//============================================================================
// Variables and Types

std::ostream & outStream = std::cout;

typedef std::codecvt<WChar,Char,std::mbstate_t> FacetType;

static std::locale utf8Locale("en_US.UTF8");

static const FacetType& utf8Facet = std::use_facet<FacetType>(utf8Locale);


//============================================================================
// Global Functions

void convertStr(Char const *input, int inputLength, WChar *output, int outputSize, int &processedInputLength, int &resultedOutputLength)
{
  std::mbstate_t mystate = std::mbstate_t();
  Char const *fromNext;
  WChar *toNext;

  // translate characters:
  utf8Facet.in(mystate, input, input+inputLength, fromNext, output, output+outputSize, toNext);

  processedInputLength = fromNext-input;
  resultedOutputLength = toNext-output;
}


void convertStr(WChar const *input, int inputLength, Char *output, int outputSize, int &processedInputLength, int &resultedOutputLength)
{
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


void printIndents(OutStream &stream, int indents)
{
  for (Int i=0; i < indents; ++i) {
    stream << STR(" ");
  }
}

} } // namespace
