/**
 * @file Core/Data/Stringifyable.h
 * Contains the header of interface Data::Stringifyable.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_STRINGIFYABLE_H
#define CORE_DATA_STRINGIFYABLE_H

namespace Core { namespace Data
{

// TODO: DOC

class Stringifyable : public IdentifiableInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Stringifyable, IdentifiableInterface, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual void toString(StrStream &stream, Int indentation=0) const = 0;

  public: virtual Str toString(Int indentation=0) const
  {
    StrStream stream;
    this->toString(stream);
    return stream.str();
  }

}; // class

} } // namespace

#endif
