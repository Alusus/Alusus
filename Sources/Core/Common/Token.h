/**
 * @file Core/Common/Token.h
 * Contains the header of class Common::Token.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef COMMON_TOKEN_H
#define COMMON_TOKEN_H

namespace Core { namespace Common
{

// TODO: Do we need to add new variables to the Token class to hold values needed by custom_literal and the likes?

/**
 * @brief Contains information about a single token.
 * @ingroup common
 *
 * Contains the information that defines a single token. This information is
 * composed of the token definition and the token text.
 */
class Token
{
  //============================================================================
  // Member Variables

  /**
   * @brief Specifies the type of this token.
   *
   * This value is the id of the token definition that matched this token.
   */
  private: Word id;

  /**
   * @brief The text of the token, if needed.
   *
   * If the token is not a constant string, this value will contain the token
   * text, otherwise it will be empty.
   */
  private: Str text;

  /// The number of the line at which the token appeared in the source code.
  private: Int line;

  /// The number of the column at which the token appeared in the source code.
  private: Int column;


  //============================================================================
  // Constructor / Destructor

  public: Token() : id(UNKNOWN_ID)
  {
  }

  public: ~Token()
  {
  }


  //============================================================================
  // Member Functions

  /**
   * Set the token id.
   *
   * @param i The token id. This value is the id of the token definition
   *          that matched this token.
   */
  public: void setId(Word i)
  {
    this->id = i;
  }

  /**
   * Get the token id.
   *
   * @return Returns the token id. This value is the id of the token
   *         definition that matched this token.
   */
  public: Word getId() const
  {
    return this->id;
  }

  /**
   * Set the token text.
   *
   * @param t A pointer to a string containing the token text.
   */
  public: void setText(const Char *t)
  {
    this->text = t;
  }

  /**
   * Set the token text.
   *
   * @param t A pointer to a string containing the token text.
   * @param s The size of the string.
   */
  public: void setText(const Char *t, Int s)
  {
    this->text.assign(t, s);
  }

  /**
   * Get the token text.
   *
   * @return Returns a pointer to the token text string.
   */
  public: const Str& getText() const
  {
    return this->text;
  }

  /**
   * @brief Set the token's line number.
   *
   * Set the line number at which the token appeared in the source code. This
   * value refers to the line number of the first character in the token.
   *
   * @param l The value to set as the token's line number.
   */
  public: void setLine(Int l)
  {
    this->line = l;
  }

  /**
   * @brief Get the token's line number.
   *
   * Get the line number at which the token appeared in the source code. This
   * value refers to the line number of the first character in the token.
   *
   * @return The line number of the first character in the token.
   */
  public: Int getLine() const
  {
    return this->line;
  }

  /**
   * @brief Set the token's column number.
   *
   * Set the column number at which the token appeared in the source code.
   * This value refers to the column number of the first character in the token.
   *
   * @param c The value to set as the token's column number.
   */
  public: void setColumn(Int c)
  {
    this->column = c;
  }

  /**
   * @brief Get the token's column number.
   *
   * Get the column number at which the token appeared in the source code.
   * This value refers to the column number of the first character in the token.
   *
   * @return The column number of the first character in the token.
   */
  public: Int getColumn() const
  {
    return this->column;
  }

}; // class

} } // namespace

#endif
