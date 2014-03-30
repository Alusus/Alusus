/**
 * @file Core/Basic/SbStr.h
 * Contains the header of class Basic::SbStr.
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

  public: SbStr(const Char *str, Word n, Word bufferSize)
  {
    this->assign(str, n, bufferSize);
  }

  public: SbStr(const Char *str, Word bufferSize)
  {
    this->assign(str, bufferSize);
  }


  //============================================================================
  // Operators

  public: Bool operator==(const Char *s) const
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

  public: Bool operator>(const Char *s) const
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

  public: Bool operator<(const Char *s) const
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

  public: void assign(const Char *str, Word n, Word bufferSize);

  public: void assign(const Char *str, Word bufferSize);

  public: void assign(const SbStr &str, Word bufferSize)
  {
    this->assign(str.c_str(), bufferSize);
  }

  public: void append(const Char *str, Word src_size, Word bufferSize);

  public: void append(const Char *str, Word bufferSize);

  public: void append(const SbStr &str, Word bufferSize)
  {
    this->append(str.c_str(), bufferSize);
  }

  public: Word size() const
  {
    return getStrLen(this->c_str());
  }

  public: const Char* c_str() const
  {
    return reinterpret_cast<const Char*>(this);
  }

}; // class


/**
 * @brief Cast any const buffer into a const SbStr object.
 * @ingroup basic_datatypes
 */
template <class T> const SbStr& sbstr_cast(const T *b)
{
  return *reinterpret_cast<const SbStr*>(b);
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
