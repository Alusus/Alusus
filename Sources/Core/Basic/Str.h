/**
 * @file Core/Basic/Str.h
 * Contains the header of class Core::Basic::Str.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
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

  public: Str() {}
  public: Str(const std::string &str) : std::string(str) {}
  public: Str(const std::string &str, Word pos, Word n) : std::string(str, pos, n) {}
  public: Str(const SbStr &str) : std::string(str.c_str()) {}
  public: Str(const SbStr &str, Word pos, Word n) : std::string(str.c_str() + pos, n) {}
  public: Str(const Char *s, Word n) : std::string(s, n) {}
  public: Str(const Char *s) : std::string(s) {}
  public: Str(Word n, Char c) : std::string(n, c) {}


  //============================================================================
  // Operators

  public: Bool operator==(const Char *s) const
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

  public: Bool operator>(const Char *s) const
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

  public: Bool operator<(const Char *s) const
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

}; // class

} } // namespace

#endif
