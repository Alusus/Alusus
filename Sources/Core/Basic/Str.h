/**
 * @file Core/Basic/Str.h
 * Contains the header of class Core::Basic::Str.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef BASIC_STR_H
#define BASIC_STR_H

namespace Core { namespace Basic
{

/**
 * @brief Basic string functionality with comparison operators.
 * @ingroup basic_datatypes
 *
 * This class overrides std's string class to provide comparison operators.
 */
class Str : public std::string
{
  //============================================================================
  // Constructors

  /// @name Main and Byte Character Constructors
  /// @{

  public: Str() {}
  public: Str(std::string const &str) : std::string(str) {}
  public: Str(std::string const &str, Word pos, Word n) : std::string(str, pos, n) {}
  public: Str(SbStr const &str) : std::string(str.c_str()) {}
  public: Str(SbStr const &str, Word pos, Word n) : std::string(str.c_str() + pos, n) {}
  public: Str(Char const *s, Word n) : std::string(s, n) {}
  public: Str(Char const *s) : std::string(s) {}
  public: Str(Word n, Char c) : std::string(n, c) {}

  /// @}

  /// @name Wide Character Constructors
  /// @{

  public: Str(std::wstring const &str, Word pos=0, Word n=0)
  {
    this->assign(str, pos, n);
  }

  public: Str(SbWStr const &str, Word pos=0, Word n=0)
  {
    this->assign(str, pos, n);
  }

  public: Str(WChar const *s, Word n=0)
  {
    this->assign(s, n);
  }

  /// @}


  //============================================================================
  // Operators

  public: Bool operator==(Char const *s) const
  {
    return this->compare(s) == 0;
  }

  public: Bool operator==(const std::string &s) const
  {
    return this->compare(s) == 0;
  }

  public: Bool operator==(const SbStr &s) const
  {
    return this->compare(s.c_str()) == 0;
  }

  public: Bool operator>(Char const *s) const
  {
    return this->compare(s) > 0;
  }

  public: Bool operator>(const std::string &s) const
  {
    return this->compare(s) > 0;
  }

  public: Bool operator>(const SbStr &s) const
  {
    return this->compare(s.c_str()) > 0;
  }

  public: Bool operator<(Char const *s) const
  {
    return this->compare(s) < 0;
  }

  public: Bool operator<(const std::string &s) const
  {
    return this->compare(s) < 0;
  }

  public: Bool operator<(const SbStr &s) const
  {
    return this->compare(s.c_str()) < 0;
  }


  //============================================================================
  // Functions

  using std::string::assign;

  public: void assign(SbStr const &str, Word pos=0, Word n=0)
  {
    if (n == 0) n = str.size()-pos;
    this->assign(str.c_str()+pos, n);
  }

  public: void assign(std::wstring const &str, Word pos=0, Word n=0)
  {
    this->assign(str.c_str()+pos, n);
  }

  public: void assign(SbWStr const &str, Word pos = 0, Word n=0)
  {
    this->assign(str.c_str()+pos, n);
  }

  public: void assign(WChar const *s, Word n=0)
  {
    if (n == 0) n = getStrLen(s);
    Char *buffer = reinterpret_cast<Char*>(SALLOC(n*4)); // A UTF8 char may take up to 4 bytes.
    Int inLength, outLength;
    convertStr(s, n, buffer, n*4, inLength, outLength);
    this->assign(buffer, outLength);
    SFREE(buffer);
  }

  public: SbStr const& sbstr() const
  {
    return sbstr_cast(this->c_str());
  }

}; // class

} } // namespace

#endif
