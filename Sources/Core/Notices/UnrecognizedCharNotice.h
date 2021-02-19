/**
 * @file Core/Notices/UnrecognizedCharNotice.h
 * Contains the header of class Core::Notices::UnrecognizedCharNotice.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_NOTICES_UNRECOGNIZEDCHARNOTICE_H
#define CORE_NOTICES_UNRECOGNIZEDCHARNOTICE_H

namespace Core::Notices
{

/**
 * @brief A build message for the "Unrecognized Char" error.
 * @ingroup core_notices
 *
 * This message class is for error code L1001, which is raised when the
 * lexer faces characters that are not recognized by any token.
 */
class UnrecognizedCharNotice : public Notice
{
  //============================================================================
  // Type Info

  TYPE_INFO(UnrecognizedCharNotice, Notice, "Core.Notices", "Core", "alusus.org");


  //============================================================================
  // Member Variables

  /**
   * @brief The source text.
   *
   * This string contains all characters in the group of consecutive
   * characters that caused the error or the warning. However, this string
   * should contain a maximum of LEXER_ERROR_BUFFER_MAX_CHARACTERS characters.
   * If the error text is bigger than LEXER_ERROR_BUFFER_MAX_CHARACTERS, it should
   * be clipped.
   */
  private: Str text;


  //============================================================================
  // Constructor / Destructor

  public: UnrecognizedCharNotice(Char const *t, SharedPtr<Data::SourceLocation> const &sl)
    : Notice(sl), text(t)
  {
  }

  public: virtual ~UnrecognizedCharNotice()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Notice::getCode()
  public: virtual Str const& getCode() const
  {
    static Str code(S("CL1001"));
    return code;
  }

  /// @sa Notice::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 1;
  }

  /// @sa Notice::buildDescription()
  public: virtual void buildDescription(Str &str) const;

  /**
   * @brief Set the source text.
   *
   * This string contains all characters in the group of consecutive
   * characters that caused the error or the warning. However, this string
   * should contain a maximum of LEXER_ERROR_BUFFER_MAX_CHARACTERS characters.
   * If the error text is bigger than LEXER_ERROR_BUFFER_MAX_CHARACTERS, it should
   * be clipped.
   *
   * @param t A pointer to a string containing the error or warning text.
   */
  public: void setText(Char const *t)
  {
    this->text = t;
  }

  /**
   * @brief Append a character to the source text.
   *
   * The source text string contains all characters in a group of consecutive
   * characters that caused the error or the warning. However, this string
   * should contain a maximum of LEXER_ERROR_BUFFER_MAX_CHARACTERS characters.
   * If the error text is bigger than LEXER_ERROR_BUFFER_MAX_CHARACTERS, it should
   * be clipped.
   *
   * @param ch The character to append to the source text.
   * @param sl The source location at which the character appeared. This value
   *           will only be considered if this character is the first in the
   *           buffer.
   */
  public: void appendText(Char ch, Data::SourceLocationRecord const &sl)
  {
    if (this->getSourceLocation() == 0) {
      this->setSourceLocation(newSrdObj<Data::SourceLocationRecord>(sl));
    }
    this->text.append(&ch, 1);
  }

  /**
   * @brief Append a string to the source text.
   *
   * The source text string contains all characters in a group of consecutive
   * characters that caused the error or the warning. However, this string
   * should contain a maximum of LEXER_ERROR_BUFFER_MAX_CHARACTERS characters.
   * If the error text is bigger than LEXER_ERROR_BUFFER_MAX_CHARACTERS, it should
   * be clipped.
   *
   * @param str The string to append to the source text.
   * @param sl The source location at which the character appeared. This value
   *           will only be considered if this character is the first in the
   *           buffer.
   */
  public: void appendText(Char const *str, Data::SourceLocationRecord const &sl)
  {
    if (str == 0 || str[0] == C('\0')) return;
    if (this->getSourceLocation() == 0) {
      this->setSourceLocation(newSrdObj<Data::SourceLocationRecord>(sl));
    }
    this->text.append(str);
  }

  /**
   * @brief Get a pointer to the source text.
   *
   * This string contains all characters in the group of consecutive characters
   * that caused the error or the warning. However, this string should contain
   * a maximum of LEXER_ERROR_BUFFER_MAX_CHARACTERS characters. If the error text is
   * bigger than LEXER_ERROR_BUFFER_MAX_CHARACTERS, it should be clipped.
   *
   * @return A pointer to the source text string.
   */
  public: Str const& getText() const
  {
    return this->text;
  }

  /**
   * @brief Get the length of the source text.
   *
   * This string contains all characters in the group of consecutive characters
   * that caused the error or the warning. However, this string should contain
   * a maximum of LEXER_ERROR_BUFFER_MAX_CHARACTERS characters. If the error text is
   * bigger than LEXER_ERROR_BUFFER_MAX_CHARACTERS, it should be clipped.
   *
   * @return The number of characters in the error text.
   */
  public: Int getTextLength() const
  {
    return this->text.getLength();
  }

  /**
   * Clear the source text and reset the line and column to -1 and the code to
   * 0.
   */
  public: void clear()
  {
    this->text.clear();
    this->setSourceLocation(SharedPtr<Data::SourceLocation>::null);
  }

}; // class

} // namespace

#endif
