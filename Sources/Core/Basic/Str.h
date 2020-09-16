/**
 * @file Core/Basic/Str.h
 * Contains the header of class Core::Basic::Str.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_STR_H
#define CORE_BASIC_STR_H

namespace Core::Basic
{

/**
 * @brief Basic string functionality with comparison operators.
 * @ingroup basic_datatypes
 *
 * This class overrides std's string class to provide comparison operators.
 */
class Str : public Brl::String
{
  //============================================================================
  // Constructors

  public: using Brl::String::String;

  public: Str(Brl::String const &str) : Brl::String(str)
  {
  }

  public: Str(Char const *str, LongInt pos, LongInt n)
  {
    this->assign(str, pos, n);
  }

  public: Str(WChar const *str, LongInt pos=0, LongInt n=0)
  {
    this->assign(str, pos, n);
  }


  //============================================================================
  // Functions

  using Brl::String::assign;

  public: void assign(Char const *buf, LongInt pos, LongInt n);

  public: void assign(WChar const *buf, LongInt pos, LongInt n);

  public: void assign(WChar const *s, Word n=0)
  {
    if (n == 0) n = getStrLen(s);
    Char *buffer = reinterpret_cast<Char*>(SALLOC(n*4)); // A UTF8 char may take up to 4 bytes.
    Int inLength, outLength;
    convertStr(s, n, buffer, n*4, inLength, outLength);
    this->assign(buffer, outLength);
    SFREE(buffer);
  }

  public: SbStr const sbstr() const
  {
    return sbstr_cast(this->getBuf());
  }

}; // class

} // namespace


//==============================================================================
// std::hash specialization for Str.

namespace std
{
  template<> struct hash<Core::Basic::Str>
  {
    std::size_t operator()(Core::Basic::Str const &s) const noexcept
    {
      return std::hash<std::string_view>{}(std::string_view(s.getBuf()));
    }
  };
}

#endif
