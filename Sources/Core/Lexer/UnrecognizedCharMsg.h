/**
 * @file Core/Lexer/UnrecognizedCharMsg.h
 * Contains the header of class Core::Lexer::UnrecognizedCharMsg.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_UNRECOGNIZED_CHAR_MSG_H
#define LEXER_UNRECOGNIZED_CHAR_MSG_H

namespace Core { namespace Lexer
{

/**
 * @brief A build message for the "Unrecognized Char" error.
 * @ingroup lexer
 *
 * This message class is for error code L1001, which is raised when the
 * lexer faces characters that are not recognized by any token.
 */
class UnrecognizedCharMsg : public Common::BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(UnrecognizedCharMsg, Common::BuildMsg, "Core.Lexer", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  /**
   * @brief The source text.
   *
   * This string contains all characters in the group of consecutive
   * characters that caused the error or the warning. However, this string
   * should contain a maximum of ERROR_BUFFER_MAX_CHARACTERS characters.
   * If the error text is bigger than ERROR_BUFFER_MAX_CHARACTERS, it should
   * be clipped.
   */
  private: Str text;


  //============================================================================
  // Constructor / Destructor

  public: UnrecognizedCharMsg(Char const *t, Int l, Int c) : Common::BuildMsg(l, c), text(t)
  {
  }

  public: virtual ~UnrecognizedCharMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Common::BuildMsg::getCode()
  public: virtual const Str& getCode() const
  {
    static Str code("L1001");
    return code;
  }

  /// @sa Common::BuildMsg::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 1;
  }

  /// @sa Common::BuildMsg::buildDescription()
  public: virtual void buildDescription(Str &str) const;

  /**
   * @brief Set the source text.
   *
   * This string contains all characters in the group of consecutive
   * characters that caused the error or the warning. However, this string
   * should contain a maximum of ERROR_BUFFER_MAX_CHARACTERS characters.
   * If the error text is bigger than ERROR_BUFFER_MAX_CHARACTERS, it should
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
   * should contain a maximum of ERROR_BUFFER_MAX_CHARACTERS characters.
   * If the error text is bigger than ERROR_BUFFER_MAX_CHARACTERS, it should
   * be clipped.
   *
   * @param ch The character to append to the source text.
   * @param l The line within the source code at which the character
   *          appeared. If this character is the first character in the
   *          buffer, the line number will be considered the line at which
   *          the source text appeared, otherwise this value will be
   *          ignored.
   * @param c The column within the source code at which the character
   *          appeared. If this character is the first character in the
   *          buffer, the column number will be considered the column at
   *          which the source text appeared, otherwise this value will be
   *          ignored.
   */
  public: void appendText(Char ch, Int l, Int c)
  {
    if (this->getLine() == -1) {
      this->setLine(l);
      this->setColumn(c);
    }
    this->text.append(1, ch);
  }

  /**
   * @brief Append a string to the source text.
   *
   * The source text string contains all characters in a group of consecutive
   * characters that caused the error or the warning. However, this string
   * should contain a maximum of ERROR_BUFFER_MAX_CHARACTERS characters.
   * If the error text is bigger than ERROR_BUFFER_MAX_CHARACTERS, it should
   * be clipped.
   *
   * @param str The string to append to the source text.
   * @param l The line within the source code at which the string appeared. If
   *          the position hasn't been set yet, this line number will be
   *          considered the line at which the source text appeared, otherwise
   *          this value will be ignored.
   * @param c The column within the source code at which the string appeared.
   *          If the position hasn't been set yet, this column number
   *          will be consindered the column at which the source text appeared
   *          otherwise this value will be ignored.
   */
  public: void appendText(Char const *str, Int l, Int c)
  {
    if (str == 0 || str[0] == CHR('\0')) return;
    if (this->getLine() == -1) {
      this->setLine(l);
      this->setColumn(c);
    }
    this->text.append(str);
  }

  /**
   * @brief Get a pointer to the source text.
   *
   * This string contains all characters in the group of consecutive characters
   * that caused the error or the warning. However, this string should contain
   * a maximum of ERROR_BUFFER_MAX_CHARACTERS characters. If the error text is
   * bigger than ERROR_BUFFER_MAX_CHARACTERS, it should be clipped.
   *
   * @return A pointer to the source text string.
   */
  public: const Str& getText() const
  {
    return this->text;
  }

  /**
   * @brief Get the length of the source text.
   *
   * This string contains all characters in the group of consecutive characters
   * that caused the error or the warning. However, this string should contain
   * a maximum of ERROR_BUFFER_MAX_CHARACTERS characters. If the error text is
   * bigger than ERROR_BUFFER_MAX_CHARACTERS, it should be clipped.
   *
   * @return The number of characters in the error text.
   */
  public: Int getTextLength() const
  {
    return this->text.size();
  }

  /**
   * Clear the source text and reset the line and column to -1 and the code to
   * 0.
   */
  public: void clear()
  {
    this->text.clear();
    this->setLine(0);
    this->setColumn(0);
  }

}; // class

} } // namespace

#endif
