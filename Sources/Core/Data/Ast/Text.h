/**
 * @file Core/Data/Ast/Text.h
 * Contains the header of class Core::Data::Ast::Text.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_TEXT_H
#define CORE_DATA_AST_TEXT_H

namespace Core { namespace Data { namespace Ast
{

// TODO: DOC

class Text : public Node,
             public virtual RtMembers, public virtual Metadata, public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Text, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Node, RtMembers, Metadata, Clonable, Printable);


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

  IMPLEMENT_METADATA(Text);

  IMPLEMENT_RTMEMBERS((value, TiStr, VALUE, setValue(value), &value),
                      (prodId, TiWord, VALUE, setProdId(value), &prodId),
                      (sourceLocation, SourceLocation, VALUE, setSourceLocation(value), &sourceLocation));


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
    this->value = v == 0 ? "" : v->get();
  }

  public: TiStr const& getValue() const
  {
    return this->value;
  }


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const
  {
    SharedPtr<Text> newText = std::make_shared<Text>();
    newText->setProdId(this->getProdId());
    newText->setValue(this->getValue());
    newText->setSourceLocation(this->getSourceLocation());
    return newText;
  }


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const
  {
    stream << STR("Text");
    Word id = this->getProdId();
    if (id != UNKNOWN_ID) {
      stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("] ");
    }
    stream << STR(": ") << this->getValue();
  }

}; // class


//==============================================================================
// Macros

#define DEFINE_AST_TEXT_ELEMENT(X) \
  class X : public Text \
  { \
    TYPE_INFO(X, Text, "Core.Data.Ast", "Core", "alusus.net"); \
    IMPLEMENT_EMPTY_CONSTRUCTOR(X); \
    IMPLEMENT_ATTR_CONSTRUCTOR(X); \
    public: virtual SharedPtr<TiObject> clone() const \
    { \
      SharedPtr<X> newObject = std::make_shared<X>(); \
      newObject->setProdId(this->getProdId()); \
      newObject->setValue(this->getValue()); \
      newObject->setSourceLocation(this->getSourceLocation()); \
      return newObject; \
    } \
    public: virtual void print(OutStream &stream, Int indents=0) const \
    { \
      stream << STR(#X); \
      stream << STR(": ") << this->getValue(); \
      Word id = this->getProdId(); \
      if (id != UNKNOWN_ID) { \
        stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("] "); \
      } \
    } \
  }

} } } // namespace

#endif
