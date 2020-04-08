/**
 * @file Core/Processing/StdCharInStream.h
 * Contains the header of class Core::Processing::StdCharInStream.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
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

#if defined(_WIN32)
  private: FILE *fd;
#else
  private: InStream *stream;
#endif


  //============================================================================
  // Constructors & Destructor

#if defined(_WIN32)
  public: StdCharInStream(FILE* fd);
#else
  public: StdCharInStream(InStream *s);
#endif

  public: virtual ~StdCharInStream();


  //============================================================================
  // Member Functions

#if defined(_WIN32)
  public: virtual WChar get();
#else
  public: virtual Char get();
#endif

  public: virtual Bool isEof();

  public: virtual CharInStreaming::CharInStreamingType getType();

}; // class

} // namespace

#endif
