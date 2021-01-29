/**
 * @file Core/Data/Ast/Text.h
 * Contains the header of class Core::Data::Ast::Text.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_TEXT_H
#define CORE_DATA_AST_TEXT_H

namespace Core::Data::Ast
{

// TODO: DOC

class Text : public Node, public Binding, public MetaHaving, public Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Text, Node, "Core.Data.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(Node, Binding, MetaHaving, Printable);
  OBJECT_FACTORY(Text);


  //============================================================================
  // Member Variables

  /**
   * @brief The text of the token, if needed.
   *
   * If the token is not a constant string, this value will contain the token
   * text, otherwise it will be empty.
   */
  private: TiStr value;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(Text);

  IMPLEMENT_BINDING(Binding,
    (value, TiStr, VALUE, setValue(value), &value),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Text);

  IMPLEMENT_ATTR_CONSTRUCTOR(Text);


  //============================================================================
  // Member Functions

  public: void setValue(Char const *v)
  {
    this->value = v;
  }
  public: void setValue(Char const *v, Int s)
  {
    this->value.set(v, s);
  }
  public: void setValue(TiStr const *v)
  {
    if (v == 0) this->value = "";
    else this->value = *v;
  }

  public: TiStr const& getValue() const
  {
    return this->value;
  }


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const
  {
    stream << S("Text");
    Word id = this->getProdId();
    stream << S(": ") << this->getValue();
    if (id != UNKNOWN_ID) {
      stream << S(" [") << IdGenerator::getSingleton()->getDesc(id) << S("]");
    }
  }

}; // class


//==============================================================================
// Macros

#define DEFINE_AST_TEXT_ELEMENT(X) \
  class X : public Text \
  { \
    TYPE_INFO(X, Text, "Core.Data.Ast", "Core", "alusus.org"); \
    OBJECT_FACTORY(X); \
    IMPLEMENT_EMPTY_CONSTRUCTOR(X); \
    IMPLEMENT_ATTR_CONSTRUCTOR(X); \
    public: virtual void print(OutStream &stream, Int indents=0) const \
    { \
      stream << S(#X); \
      stream << S(": ") << this->getValue(); \
      Word id = this->getProdId(); \
      if (id != UNKNOWN_ID) { \
        stream << S(" [") << IdGenerator::getSingleton()->getDesc(id) << S("]"); \
      } \
    } \
  }

} // namespace

#endif
