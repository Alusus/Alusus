/**
 * @file Core/Processing/StdCharInStream.h
 * Contains the header of class Core::Processing::StdCharInStream.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_STDCHARINSTREAM_H
#define CORE_PROCESSING_STDCHARINSTREAM_H

namespace Core::Processing
{

class StdCharInStream : public TiObject, public CharInStreaming
{
  //============================================================================
  // Type Info

  TYPE_INFO(StdCharInStream, TiObject, "Core.Data.Ast", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(CharInStreaming)
  ));


  //============================================================================
  // Member Variables

#if defined(__MINGW32__) || defined(__MINGW64__)
  private: FILE *fd;
#else
  private: InStream *stream;
#endif


  //============================================================================
  // Constructors & Destructor

#if defined(__MINGW32__) || defined(__MINGW64__)
  public: StdCharInStream(FILE* fd) : fd(fd)
  {
    VALIDATE_NOT_NULL(fd);
#else
  public: StdCharInStream(InStream *s) : stream(s)
  {
    VALIDATE_NOT_NULL(s);
#endif
  }

  public: virtual ~StdCharInStream()
  {
  }


  //============================================================================
  // Member Functions

#if defined(__MINGW32__) || defined(__MINGW64__)
  public: virtual WChar get()
  {
    WChar c;
    fread(&c, sizeof(c), 1, this->fd);
    return c;
#else
  public: virtual Char get()
  {
    Char c;
    this->stream->get(c);
    return c;
#endif
  }

  public: virtual Bool isEof()
  {
#if defined(__MINGW32__) || defined(__MINGW64__)
    return feof(this->fd);
#else
    return this->stream->eof();
#endif
  }

}; // class

} // namespace

#endif
