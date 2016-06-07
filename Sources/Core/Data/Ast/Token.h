/**
 * @file Core/Data/Ast/Token.h
 * Contains the header of class Core::Data::Ast::Token.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_TOKEN_H
#define CORE_DATA_AST_TOKEN_H

namespace Core { namespace Data { namespace Ast
{

/**
 * @brief Contains information about a single token in a PRT.
 * @ingroup data
 *
 * Contains the information that defines a single token in a Parsing
 * Representation Tree (PRT). This information is composed of the token
 * definition and the token text. This object is created by the
 * GenericParsingHandler to compose the parsed tree.
 */
class Token : public Node,
              public virtual MetadataHolder, public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Token, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_3(Node, MetadataHolder, Clonable, Printable);


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

  public: Token(Word pid=UNKNOWN_ID, Word i=UNKNOWN_ID, Char const *txt=STR("")) :
    MetadataHolder(pid), id(i), text(txt)
  {
  }

  public: Token(Word pid, Word i, Char const *txt, SourceLocation const &sl) :
    MetadataHolder(pid, sl), id(i), text(txt)
  {
  }

  public: virtual ~Token()
  {
  }

  public: static SharedPtr<Token> create(Word pid=UNKNOWN_ID, Word i=UNKNOWN_ID, Char const *txt=STR(""))
  {
    return std::make_shared<Token>(pid, i, txt);
  }

  public: static SharedPtr<Token> create(Word pid, Word i, Char const *txt, SourceLocation const &sl)
  {
    return std::make_shared<Token>(pid, i, txt, sl);
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


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const
  {
    SharedPtr<Token> newToken = std::make_shared<Token>();
    newToken->setProdId(this->getProdId());
    newToken->setId(this->getId());
    newToken->setText(this->getText().c_str());
    newToken->setSourceLocation(this->getSourceLocation());
    return newToken;
  }


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const
  {
    stream << STR("Token");
    Word id = this->getProdId();
    if (id != UNKNOWN_ID) {
      stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("] ");
    }
    stream << STR(": ");

    stream << IdGenerator::getSingleton()->getDesc(this->getId());
    stream << STR(" (\"") << this->getText() << STR("\")");
  }

}; // class

} } } // namespace

#endif
