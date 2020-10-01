/**
 * @file Core/Basic/SbWStr.h
 * Contains the header of class Core::Basic::SbWStr.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_SBWSTR_H
#define CORE_BASIC_SBWSTR_H

namespace Core::Basic
{

/**
 * @brief Static buffer string with comparison operations.
 * @ingroup basic_datatypes
 *
 * This class uses static string buffers instead of the dynamic Srl::WString
 * class. This class is a wrappar around the WChar* buffers. It treats 'this' as
 * the pointer to the buffer. This allows the user to easily cast any buffer
 * into this class.
 */
class SbWStr
{
  //============================================================================
  // Member Variables

  private: WChar *buf;


  //============================================================================
  // Constructors

  public: SbWStr(WChar *b) : buf(b)
  {
  }

  public: SbWStr(Word *b) : buf(reinterpret_cast<WChar*>(b))
  {
  }


  //============================================================================
  // Operators

  public: Bool operator==(WChar const *s) const
  {
    return compareStr(this->getBuf(), s) == 0;
  }

  public: Bool operator!=(WChar const *s) const
  {
    return compareStr(this->getBuf(), s) != 0;
  }

  public: Bool operator>(WChar const *s) const
  {
    return compareStr(this->getBuf(), s) > 0;
  }

  public: Bool operator<(WChar const *s) const
  {
    return compareStr(this->getBuf(), s) < 0;
  }

  public: operator WChar const*() const {
    return this->buf;
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

  public: Word getLength() const
  {
    if (this->getBuf() == 0) return 0;
    return getStrLen(this->getBuf());
  }

  public: WChar const* getBuf() const
  {
    return this->buf;
  }

  public: WChar* getBuf()
  {
    return this->buf;
  }

  /// @}

}; // class


/**
 * @brief Cast any const buffer into a const SbWStr object.
 * @ingroup basic_datatypes
 */
template <class T> SbWStr const sbwstr_cast(T const *b)
{
  return SbWStr(const_cast<T*>(b));
}


/**
 * @brief Cast any buffer into a SbWStr object.
 * @ingroup basic_datatypes
 */
template <class T> SbWStr sbwstr_cast(T *b)
{
  return SbWStr(b);
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

} // namespace

#endif
