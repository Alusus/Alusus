#include "InteractiveCharInStream.h"
#include "CharInStreaming.h"

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
  // Print a new line only on the first line feed.
  if (c == L'\n' && this->isPreviousLF) {
    this->isPreviousLF = false;
    outStream << ++this->lineNumber << S("> ");
  }
  else if (c == L'\n') {
    this->isPreviousLF = true;
  }
#else
Char InteractiveCharInStream::get()
{
  Char c;
  inStream.get(c);
  if (c == C('\n')) {
    outStream << ++this->lineNumber << S("> ");
  }
#endif
  return c;
}

Bool InteractiveCharInStream::isEof()
{
#if defined(__MINGW32__) || defined(__MINGW64__)
  return feof(stdin);
#else
  return inStream.eof();
#endif
}

CharInStreaming::CharInStreamingType InteractiveCharInStream::getType()
{
  return CharInStreaming::CharInStreamingType::INTERACTIVE_CHAR_IN_STREAM;
}

} // namespace
