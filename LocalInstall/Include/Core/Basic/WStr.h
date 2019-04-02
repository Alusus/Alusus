/**
 * @file Core/Basic/WStr.h
 * Contains the header of class Core::Basic::WStr.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_WSTR_H
#define CORE_BASIC_WSTR_H

namespace Core { namespace Basic
{

/**
 * @brief Basic wide string functionality with comparison operators.
 * @ingroup basic_datatypes
 *
 * This class overrides std's wstring class to provide comparison operators.
 */
class WStr : public std::wstring
{
  //============================================================================
  // Constructors

  /// @name Main and Wide Character Constructors
  /// @{

  public: WStr() {}
  public: WStr(std::wstring const &str) : std::wstring(str) {}
  public: WStr(std::wstring const &str, Word pos, Word n) : std::wstring(str, pos, n) {}
  public: WStr(SbWStr const &str) : std::wstring(str.c_str()) {}
  public: WStr(SbWStr const &str, Word pos, Word n) : std::wstring(str.c_str() + pos, n) {}
  public: WStr(WChar const *s, Word n) : std::wstring(s, n) {}
  public: WStr(WChar const *s) : std::wstring(s) {}
  public: WStr(Word n, Char c) : std::wstring(n, c) {}

  /// @}

  /// @name Byte Character Constructors
  /// @{

  public: WStr(std::string const &str, Word pos=0, Word n=0)
  {
    this->assign(str, pos, n);
  }

  public: WStr(SbStr const &str, Word pos=0, Word n=0)
  {
    this->assign(str, pos, n);
  }

  public: WStr(Char const *s, Word n=0)
  {
    this->assign(s, n);
  }

  /// @}


  //============================================================================
  // Operators

  public: Bool operator==(WChar const *s) const
  {
    return this->compare(s) == 0;
  }

  public: Bool operator==(const std::wstring &s) const
  {
    return this->compare(s) == 0;
  }

  public: Bool operator==(const SbWStr &s) const
  {
    return this->compare(s.c_str()) == 0;
  }

  public: Bool operator>(WChar const *s) const
  {
    return this->compare(s) > 0;
  }

  public: Bool operator>(const std::wstring &s) const
  {
    return this->compare(s) > 0;
  }

  public: Bool operator>(const SbWStr &s) const
  {
    return this->compare(s.c_str()) > 0;
  }

  public: Bool operator<(WChar const *s) const
  {
    return this->compare(s) < 0;
  }

  public: Bool operator<(const std::wstring &s) const
  {
    return this->compare(s) < 0;
  }

  public: Bool operator<(const SbWStr &s) const
  {
    return this->compare(s.c_str()) < 0;
  }


  //============================================================================
  // Functions

  using std::wstring::assign;

  public: void assign(SbWStr const &str, Word pos=0, Word n=0)
  {
    if (n == 0) n = str.size()-pos;
    this->assign(str.c_str()+pos, n);
  }

  public: void assign(std::string const &str, Word pos=0, Word n=0)
  {
    this->assign(str.c_str()+pos, n);
  }

  public: void assign(SbStr const &str, Word pos = 0, Word n=0)
  {
    this->assign(str.c_str()+pos, n);
  }

  public: void assign(Char const *s, Word n=0)
  {
    if (n == 0) n = getStrLen(s);
    WChar *buffer = reinterpret_cast<WChar*>(SALLOC(n*sizeof(WChar)));
    Int inLength, outLength;
    convertStr(s, n, buffer, n, inLength, outLength);
    this->assign(buffer, outLength);
    SFREE(buffer);
  }

  public: SbWStr const& sbwstr() const
  {
    return sbwstr_cast(this->c_str());
  }

}; // class

} } // namespace

#endif
