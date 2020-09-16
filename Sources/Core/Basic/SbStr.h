/**
 * @file Core/Basic/SbStr.h
 * Contains the header of class Core::Basic::SbStr.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_SBSTR_H
#define CORE_BASIC_SBSTR_H

namespace Core::Basic
{

/**
 * @brief Static buffer string with comparison operations.
 * @ingroup basic_datatypes
 *
 * This class uses static string buffers instead of the dynamic Brl::String
 * class. This class is a wrappar around the Char* buffers. It treats 'this' as
 * the pointer to the buffer. This allows the user to easily cast any buffer
 * into this class.
 */
class SbStr
{
  //============================================================================
  // Member Variables

  private: Char *buf;


  //============================================================================
  // Constructors

  public: SbStr(Char *b) : buf(b)
  {
  }

  public: SbStr(Byte *b) : buf(reinterpret_cast<Char*>(b))
  {
  }


  //============================================================================
  // Operators

  public: Bool operator==(Char const *s) const
  {
    return compareStr(this->getBuf(), s) == 0;
  }

  public: Bool operator!=(Char const *s) const
  {
    return compareStr(this->getBuf(), s) != 0;
  }

  public: Bool operator>(Char const *s) const
  {
    return compareStr(this->getBuf(), s) > 0;
  }

  public: Bool operator<(Char const *s) const
  {
    return compareStr(this->getBuf(), s) < 0;
  }

  public: operator Char const*() const {
    return this->buf;
  }


  //============================================================================
  // Functions

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

  /// @name Other Functions
  /// @{

  public: Char const* find(Char c) const
  {
    if (this->getBuf() == 0) return 0;
    return strchr(this->getBuf(), c);
  }

  public: Char const* find(Char const *str) const
  {
    if (this->getBuf() == 0 || str == 0) return 0;
    return strstr(this->getBuf(), str);
  }

  public: Int findPos(Char c) const
  {
    if (this->getBuf() == 0) return -1;
    auto found = strchr(this->getBuf(), c);
    return found == 0 ? -1 : found - this->getBuf();
  }

  public: Int findPos(Char const *str) const
  {
    if (this->getBuf() == 0 || str == 0) return -1;
    auto found = strstr(this->getBuf(), str);
    return found == 0 ? -1 : found - this->getBuf();
  }

  public: Word getLength() const
  {
    if (this->getBuf() == 0) return 0;
    return getStrLen(this->getBuf());
  }

  public: Char const* getBuf() const
  {
    return this->buf;
  }

  public: Char* getBuf()
  {
    return this->buf;
  }

  /// @}

}; // class


/**
 * @brief Cast any const buffer into a const SbStr object.
 * @ingroup basic_datatypes
 */
template <class T> SbStr const sbstr_cast(T const *b)
{
  return SbStr(const_cast<T*>(b));
}


/**
 * @brief Cast any buffer into a SbStr object.
 * @ingroup basic_datatypes
 */
template <class T> SbStr sbstr_cast(T *b)
{
  return SbStr(b);
}


/**
 * @brief Wrapper for static buffer string literals (SbStr).
 * @ingroup basic_macros
 *
 * This wrapper is needed for future purposes. The main purpose of this is to
 * allow easy conversion of the program from ascii to unicode. This macro can
 * be thought as a combination of the S macro and sbstr_cast.
 * Usage: SBSTR("hello") == S("hello")
 */
#define SBSTR(x)    Core::Basic::sbstr_cast(x)

} // namespace

#endif
