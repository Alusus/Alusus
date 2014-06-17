/**
 * @file Core/Standard/ParsedToken.h
 * Contains the header of class Core::Standard::ParsedToken.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef STANDARD_PARSED_TOKEN_H
#define STANDARD_PARSED_TOKEN_H

namespace Core { namespace Standard
{

/**
 * @brief Contains information about a single token.
 * @ingroup standard
 *
 * Contains the information that defines a single token. This information is
 * composed of the token definition and the token text. This object is created
 * by the GenericParsingHandler to compose the parsed tree.
 */
class ParsedToken : public ParsedItem
{
  //============================================================================
  // Type Info

  TYPE_INFO(ParsedToken, ParsedItem, "Core.Standard", "Core", "alusus.net");


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


  //============================================================================
  // Constructor / Destructor

  public: ParsedToken(Word pid=UNKNOWN_ID, Word i=UNKNOWN_ID, Char const *txt=STR(""), Int l=-1, Int c=-1) :
    ParsedItem(pid, l, c), id(i), text(txt)
  {
  }

  public: virtual ~ParsedToken()
  {
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Set the token id.
   *
   * @param i The token id. This value is the id of the token definition
   *          that matched this token.
   */
  public: void setId(Word i)
  {
    this->id = i;
  }

  /**
   * @brief Get the token id.
   *
   * @return Returns the token id. This value is the id of the token
   *         definition that matched this token.
   */
  public: Word getId() const
  {
    return this->id;
  }

  /**
   * @brief Set the token text.
   *
   * @param t A pointer to a string containing the token text.
   */
  public: void setText(Char const *t)
  {
    this->text = t;
  }

  /**
   * @brief Set the token text.
   *
   * @param t A pointer to a string containing the token text.
   * @param s The size of the string.
   */
  public: void setText(Char const *t, Int s)
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

}; // class

} } // namespace

#endif
