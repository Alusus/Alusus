#include "InteractiveCharInStream.h"

namespace Core::Processing
{


#if defined(__MINGW32__) || defined(__MINGW64__)
InteractiveCharInStream::InteractiveCharInStream() : lineNumber(1), isPreviousLF(false)
#else
InteractiveCharInStream::InteractiveCharInStream() : lineNumber(1)
#endif
{
  outStream << this->lineNumber << S("> ");
}

InteractiveCharInStream::~InteractiveCharInStream() {}


  //============================================================================
  // Member Functions

#if defined(__MINGW32__) || defined(__MINGW64__)
WChar InteractiveCharInStream::get()
{
  WChar c = getwchar();
  if (c == L'\n' && this->isPreviousLF) {
    this->isPreviousLF = false;
#else
Char InteractiveCharInStream::get()
{
  Char c;
  inStream.get(c);
  if (c == C('\n')) {
#endif
    outStream << ++this->lineNumber << S("> ");
  }
#if defined(__MINGW32__) || defined(__MINGW64__)
  else if (c == L'\n')
  {
    this->isPreviousLF = true;
  }
#endif
  return c;
}

Bool InteractiveCharInStream::isEof()
{
  return inStream.eof();
}

} // namespace
