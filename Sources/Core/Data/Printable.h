/**
 * @file Core/Data/Printable.h
 * Contains the header of interface Data::Printable.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_PRINTABLE_H
#define CORE_DATA_PRINTABLE_H

namespace Core { namespace Data
{

// TODO: DOC

class Printable : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Printable, TiInterface, "Core.Data", "Core", "alusus.org");


  //============================================================================
  // Abstract Functions

  public: virtual void print(OutStream &stream, Int indents=0) const = 0;

  public: virtual Str toString(Int indents=0) const
  {
    StrStream stream;
    this->print(stream, indents);
    return stream.str();
  }

}; // class

} } // namespace

#endif
