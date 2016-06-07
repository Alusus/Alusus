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
             public virtual MetadataHolder, public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Text, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_3(Node, MetadataHolder, Clonable, Printable);


  //============================================================================
  // Member Variables

  /**
   * @brief The text of the token, if needed.
   *
   * If the token is not a constant string, this value will contain the token
   * text, otherwise it will be empty.
   */
  private: Str value;


  //============================================================================
  // Constructor / Destructor

  public: Text(Word pid=UNKNOWN_ID, Char const *v=STR("")) :
    MetadataHolder(pid), value(v)
  {
  }

  public: Text(Word pid, Char const *v, SourceLocation const &sl) :
    MetadataHolder(pid, sl), value(v)
  {
  }

  public: virtual ~Text()
  {
  }

  public: static SharedPtr<Text> create(Word pid=UNKNOWN_ID, Char const *v=STR(""))
  {
    return std::make_shared<Text>(pid, v);
  }

  public: static SharedPtr<Text> create(Word pid, Char const *v, SourceLocation const &sl)
  {
    return std::make_shared<Text>(pid, v, sl);
  }


  //============================================================================
  // Member Functions

  public: void setValue(Char const *v)
  {
    this->value = v;
  }

  public: void setValue(Char const *v, Int s)
  {
    this->value.assign(v, s);
  }

  public: const Str& getValue() const
  {
    return this->value;
  }


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const
  {
    SharedPtr<Text> newText = std::make_shared<Text>();
    newText->setProdId(this->getProdId());
    newText->setValue(this->getValue().c_str());
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


//============================================================================
// Macros

#define DEFINE_AST_TEXT_ELEMENT(X) \
  class X : public Text \
  { \
    TYPE_INFO(X, Text, "Core.Data.Ast", "Core", "alusus.net"); \
    public: X(Word pid=UNKNOWN_ID, Char const *v=STR("")) : Text(pid, v) \
    { \
    } \
    public: X(Word pid, Char const *v, SourceLocation const &sl) : Text(pid, v, sl) \
    { \
    } \
    public: static SharedPtr<X> create(Word pid=UNKNOWN_ID, Char const *v=STR("")) \
    { \
      return std::make_shared<X>(pid, v); \
    } \
    public: static SharedPtr<X> create(Word pid, Char const *v, SourceLocation const &sl) \
    { \
      return std::make_shared<X>(pid, v, sl); \
    } \
    public: virtual SharedPtr<TiObject> clone() const \
    { \
      SharedPtr<X> newObject = std::make_shared<X>(); \
      newObject->setProdId(this->getProdId()); \
      newObject->setValue(this->getValue().c_str()); \
      newObject->setSourceLocation(this->getSourceLocation()); \
      return newObject; \
    } \
    public: virtual void print(OutStream &stream, Int indents=0) \
    { \
      stream << STR(#X); \
      Word id = this->getProdId(); \
      if (id != UNKNOWN_ID) { \
        stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("] "); \
      } \
      stream << STR(": ") << this->getValue(); \
    } \
  }

} } } // namespace

#endif
