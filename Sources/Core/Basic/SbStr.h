/**
 * @file Core/Basic/SbStr.h
 * Contains the header of class Core::Basic::SbStr.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef BASIC_SBSTR_H
#define BASIC_SBSTR_H

namespace Core { namespace Basic
{

/**
 * @brief Static buffer string with comparison operations.
 * @ingroup basic_datatypes
 *
 * This class uses static string buffers instead of the dynamic std::string
 * class. This class is a wrappar around the Char* buffers. It treats 'this' as
 * the pointer to the buffer. This allows the user to easily cast any buffer
 * into this class.
 */
class SbStr
{
  //============================================================================
  // Constructors

  public: SbStr()
  {
  }

  public: SbStr(Char const *str, Word n, Word bufferSize)
  {
    this->assign(str, n, bufferSize);
  }

  public: SbStr(Char const *str, Word bufferSize)
  {
    this->assign(str, bufferSize);
  }


  //============================================================================
  // Operators

  public: Bool operator==(Char const *s) const
  {
    return compareStr(this->c_str(), s) == 0;
  }

  public: Bool operator==(const std::string &s) const
  {
    return compareStr(this->c_str(), s.c_str()) == 0;
  }

  public: Bool operator==(const SbStr &s) const
  {
    return compareStr(this->c_str(), s.c_str()) == 0;
  }

  public: Bool operator>(Char const *s) const
  {
    return compareStr(this->c_str(), s) > 0;
  }

  public: Bool operator>(const std::string &s) const
  {
    return compareStr(this->c_str(), s.c_str()) > 0;
  }

  public: Bool operator>(const SbStr &s) const
  {
    return compareStr(this->c_str(), s.c_str()) > 0;
  }

  public: Bool operator<(Char const *s) const
  {
    return compareStr(this->c_str(), s) < 0;
  }

  public: Bool operator<(const std::string &s) const
  {
    return compareStr(this->c_str(), s.c_str()) < 0;
  }

  public: Bool operator<(const SbStr &s) const
  {
    return compareStr(this->c_str(), s.c_str()) < 0;
  }


  //============================================================================
  // Functions

  public: void assign(Char const *str, Word n, Word bufferSize);

  public: void assign(Char const *str, Word bufferSize);

  public: void assign(const SbStr &str, Word bufferSize)
  {
    this->assign(str.c_str(), bufferSize);
  }

  public: void append(Char const *str, Word src_size, Word bufferSize);

  public: void append(Char const *str, Word bufferSize);

  public: void append(const SbStr &str, Word bufferSize)
  {
    this->append(str.c_str(), bufferSize);
  }

  public: Word size() const
  {
    return getStrLen(this->c_str());
  }

  public: Char const* c_str() const
  {
    return reinterpret_cast<Char const*>(this);
  }

}; // class


/**
 * @brief Cast any const buffer into a const SbStr object.
 * @ingroup basic_datatypes
 */
template <class T> SbStr const& sbstr_cast(T const *b)
{
  return *reinterpret_cast<SbStr const*>(b);
}


/**
 * @brief Cast any buffer into a SbStr object.
 * @ingroup basic_datatypes
 */
template <class T> SbStr& sbstr_cast(T *b)
{
  return *reinterpret_cast<SbStr*>(b);
}


/**
 * @brief Wrapper for static buffer string literals (SbStr).
 * @ingroup basic_macros
 *
 * This wrapper is needed for future purposes. The main purpose of this is to
 * allow easy conversion of the program from ascii to unicode. This macro can
 * be thought as a combination of the STR macro and sbstr_cast.
 * Usage: SBSTR("hello") == STR("hello")
 */
#define SBSTR(x)    Core::Basic::sbstr_cast(x)

} } // namespace

#endif
