/**
 * @file Core/Data/Token.h
 * Contains the header of class Core::Data::Token.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_TOKEN_H
#define CORE_DATA_TOKEN_H

namespace Core { namespace Data
{

// TODO: Do we need to add new variables to the Token class to hold values needed by custom_literal and the likes?

/**
 * @brief Contains information about a single token.
 * @ingroup core_data
 *
 * Contains the information that defines a single token. This information is
 * composed of the token definition and the token text.
 */
class Token : public Node
{
  //============================================================================
  // Type Info

  TYPE_INFO(Token, Node, "Core.Data", "Core", "alusus.net");


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

  /// The location of the token in the source code.
  private: SourceLocationRecord sourceLocation;


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
  public: void setText(Char const *t)
  {
    this->text = t;
  }

  /**
   * Set the token text from a wide character string.
   *
   * @param t A pointer to a string containing the token text.
   */
  public: void setText(WChar const *t)
  {
    this->text.assign(t);
  }

  /**
   * Set the token text.
   *
   * @param t A pointer to a string containing the token text.
   * @param s The size of the string.
   */
  public: void setText(Char const *t, Int s)
  {
    this->text.assign(t, s);
  }

  /**
   * Set the token text from a wide character string.
   *
   * @param t A pointer to a string containing the token text.
   * @param s The size of the string.
   */
  public: void setText(WChar const *t, Int s)
  {
    this->text.assign(t, s);
  }

  /**
   * Get the token text.
   *
   * @return Returns a pointer to the token text string.
   */
  public: Str const& getText() const
  {
    return this->text;
  }

  /// Set the location of the token within the source code.
  public: void setSourceLocation(SourceLocationRecord const &loc)
  {
    this->sourceLocation = loc;
  }

  /// Get the location of the token within the source code.
  public: SourceLocationRecord const& getSourceLocation() const
  {
    return this->sourceLocation;
  }

}; // class

} } // namespace

#endif
