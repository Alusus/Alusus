/**
 * @file Core/Processing/StdCharInStream.h
 * Contains the header of class Core::Processing::StdCharInStream.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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

  TYPE_INFO(StdCharInStream, TiObject, "Core.Data.Ast", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(CharInStreaming)
  ));


  //============================================================================
  // Member Variables

  private: InStream *stream;


  //============================================================================
  // Constructors & Destructor

  public: StdCharInStream(InStream *s) : stream(s)
  {
    VALIDATE_NOT_NULL(s);
  }

  public: virtual ~StdCharInStream()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual Char get()
  {
    Char c;
    this->stream->get(c);
    return c;
  }

  public: virtual Bool isEof()
  {
    return this->stream->eof();
  }

}; // class

} // namespace

#endif
