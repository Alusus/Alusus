/**
 * @file Core/Basic/WStr.h
 * Contains the header of class Core::Basic::WStr.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_WSTR_H
#define CORE_BASIC_WSTR_H

namespace Core::Basic
{

/**
 * @brief Basic wide string functionality with comparison operators.
 * @ingroup basic_datatypes
 *
 * This class overrides std's wstring class to provide comparison operators.
 */
class WStr : public Brl::WString
{
  //============================================================================
  // Constructors

  public: using Brl::WString::WString;

  public: WStr(Brl::WString const &str) : Brl::WString(str)
  {
  }

  public: WStr(WChar const *str, LongInt pos, LongInt n)
  {
    this->assign(str, pos, n);
  }

  public: WStr(Char const *str, LongInt pos=0, LongInt n=0)
  {
    this->assign(str, pos, n);
  }


  //============================================================================
  // Functions

  using Brl::WString::assign;

  public: void assign(WChar const *buf, LongInt pos, LongInt n);

  public: void assign(Char const *buf, LongInt pos, LongInt n);

  public: void assign(Char const *s, Word n=0)
  {
    if (n == 0) n = getStrLen(s);
    WChar *buffer = reinterpret_cast<WChar*>(SALLOC(n*sizeof(WChar)));
    Int inLength, outLength;
    convertStr(s, n, buffer, n, inLength, outLength);
    this->assign(buffer, outLength);
    SFREE(buffer);
  }

  public: SbWStr const sbwstr() const
  {
    return sbwstr_cast(this->getBuf());
  }

}; // class

} // namespace

#endif
