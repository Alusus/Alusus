/**
 * @file Core/Basic/SbWStr.h
 * Contains the header of class Core::Basic::SbWStr.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_SBWSTR_H
#define CORE_BASIC_SBWSTR_H

namespace Core { namespace Basic
{

/**
 * @brief Static buffer string with comparison operations.
 * @ingroup basic_datatypes
 *
 * This class uses static string buffers instead of the dynamic std::string
 * class. This class is a wrappar around the WChar* buffers. It treats 'this' as
 * the pointer to the buffer. This allows the user to easily cast any buffer
 * into this class.
 */
class SbWStr
{
  //============================================================================
  // Constructors

  public: SbWStr()
  {
  }

  public: SbWStr(WChar const *str, Word n, Word bufferSize)
  {
    this->assign(str, n, bufferSize);
  }

  public: SbWStr(WChar const *str, Word bufferSize)
  {
    this->assign(str, bufferSize);
  }

  public: SbWStr(Char const *str, Word n, Word bufferSize)
  {
    this->assign(str, n, bufferSize);
  }

  public: SbWStr(Char const *str, Word bufferSize)
  {
    this->assign(str, bufferSize);
  }


  //============================================================================
  // Operators

  public: Bool operator==(WChar const *s) const
  {
    return compareStr(this->c_str(), s) == 0;
  }

  public: Bool operator==(const std::wstring &s) const
  {
    return compareStr(this->c_str(), s.c_str()) == 0;
  }

  public: Bool operator==(const SbWStr &s) const
  {
    return compareStr(this->c_str(), s.c_str()) == 0;
  }

  public: Bool operator>(WChar const *s) const
  {
    return compareStr(this->c_str(), s) > 0;
  }

  public: Bool operator>(const std::wstring &s) const
  {
    return compareStr(this->c_str(), s.c_str()) > 0;
  }

  public: Bool operator>(const SbWStr &s) const
  {
    return compareStr(this->c_str(), s.c_str()) > 0;
  }

  public: Bool operator<(WChar const *s) const
  {
    return compareStr(this->c_str(), s) < 0;
  }

  public: Bool operator<(const std::wstring &s) const
  {
    return compareStr(this->c_str(), s.c_str()) < 0;
  }

  public: Bool operator<(const SbWStr &s) const
  {
    return compareStr(this->c_str(), s.c_str()) < 0;
  }


  //============================================================================
  // Functions

  /// @name Wide Character Assigning Functions
  /// @{

  public: void assign(WChar const *str, Word n, Word bufferSize);

  public: void assign(WChar const *str, Word bufferSize)
  {
    this->assign(str, getStrLen(str), bufferSize);
  }

  public: void append(WChar const *str, Word src_size, Word bufferSize);

  public: void append(WChar const *str, Word bufferSize)
  {
    this->append(str, getStrLen(str), bufferSize);
  }

  /// @}

  /// @name Byte Character Assigning Functions
  /// @{

  public: void assign(Char const *str, Word n, Word bufferSize);

  public: void assign(Char const *str, Word bufferSize)
  {
    this->assign(str, 0, bufferSize);
  }

  public: void append(Char const *str, Word src_size, Word bufferSize);

  public: void append(Char const *str, Word bufferSize)
  {
    this->append(str, 0, bufferSize);
  }

  /// @}

  /// @name Other Functions
  /// @{

  public: Word size() const
  {
    return getStrLen(this->c_str());
  }

  public: WChar const* c_str() const
  {
    return reinterpret_cast<WChar const*>(this);
  }

  /// @}

}; // class


/**
 * @brief Cast any const buffer into a const SbWStr object.
 * @ingroup basic_datatypes
 */
template <class T> SbWStr const& sbwstr_cast(T const *b)
{
  return *reinterpret_cast<SbWStr const*>(b);
}


/**
 * @brief Cast any buffer into a SbWStr object.
 * @ingroup basic_datatypes
 */
template <class T> SbWStr& sbwstr_cast(T *b)
{
  return *reinterpret_cast<SbWStr*>(b);
}


/**
 * @brief Wrapper for static buffer string literals (SbWStr).
 * @ingroup basic_macros
 *
 * This wrapper is needed for future purposes. The main purpose of this is to
 * allow easy conversion of the program from ascii to unicode. This macro can
 * be thought as a combination of the S macro and sbwstr_cast.
 * Usage: SBWSTR("hello") == S("hello")
 */
#define SBWSTR(x)    Core::Basic::sbwstr_cast(x)

} } // namespace

#endif
