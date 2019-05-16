/**
 * @file Core/Data/Ast/Token.h
 * Contains the header of class Core::Data::Ast::Token.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_TOKEN_H
#define CORE_DATA_AST_TOKEN_H

namespace Core { namespace Data { namespace Ast
{

/**
 * @brief Contains information about a single token in a PRT.
 * @ingroup core_data
 *
 * Contains the information that defines a single token in a Parsing
 * Representation Tree (PRT). This information is composed of the token
 * definition and the token text. This object is created by the
 * GenericParsingHandler to compose the parsed tree.
 */
class Token : public Node,
              public Binding, public MetaHaving, public Clonable, public Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Token, Node, "Core.Data.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(Node, Binding, MetaHaving, Clonable, Printable);


  //============================================================================
  // Member Variables

  /**
   * @brief Specifies the type of this token.
   *
   * This value is the id of the token definition that matched this token.
   */
  private: TiWord id;

  /**
   * @brief The text of the token, if needed.
   *
   * If the token is not a constant string, this value will contain the token
   * text, otherwise it will be empty.
   */
  private: TiStr text;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(Token);

  IMPLEMENT_BINDING(Binding,
    (id, TiWord, VALUE, setId(value), &id),
    (text, TiStr, VALUE, setText(value), &text),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Token);

  IMPLEMENT_ATTR_CONSTRUCTOR(Token);


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
  public: void setId(TiWord const *i)
  {
    this->id = i == 0 ? UNKNOWN_ID : i->get();
  }

  /**
   * @brief Get the token id.
   *
   * @return Returns the token id. This value is the id of the token
   *         definition that matched this token.
   */
  public: TiWord const& getId() const
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
  public: void setText(Char const *t, Int s)
  {
    this->text.set(t, s);
  }
  public: void setText(TiStr const *t)
  {
    this->text = t == 0 ? "" : t->get();
  }

  /**
   * Get the token text.
   *
   * @return Returns a pointer to the token text string.
   */
  public: TiStr const& getText() const
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
    newToken->setText(this->getText());
    newToken->setSourceLocation(this->getSourceLocation());
    return newToken;
  }


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const
  {
    stream << S("Token: ");
    stream << IdGenerator::getSingleton()->getDesc(this->getId());
    stream << S(" (\"") << this->getText() << S("\")");
    Word id = this->getProdId();
    if (id != UNKNOWN_ID) {
      stream << S(" [") << IdGenerator::getSingleton()->getDesc(id) << S("] ");
    }
  }

}; // class

} } } // namespace

#endif
