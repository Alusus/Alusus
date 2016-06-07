/**
 * @file Core/Data/Ast/InfixOperator.h
 * Contains the header of class Core::Data::Ast::InfixOperator.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_INFIXOPERATOR_H
#define CORE_DATA_AST_INFIXOPERATOR_H

namespace Core { namespace Data { namespace Ast
{

// TODO: DOC

class InfixOperator : public Node,
                      public virtual Container, public virtual MetadataHolder,
                      public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(InfixOperator, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_4(Node, Container, MetadataHolder, Clonable, Printable);


  //============================================================================
  // Member Variables

  protected: String type;
  protected: TioSharedPtr first;
  protected: TioSharedPtr second;


  //============================================================================
  // Constructors & Destructor

  public: InfixOperator()
  {
  }

  public: InfixOperator(Word pid, SourceLocation const &sl) :
    MetadataHolder(pid, sl)
  {
  }

  public: InfixOperator(Word pid, Char const *t, TioSharedPtr const &f, TioSharedPtr const &s) :
    MetadataHolder(pid), type(t), first(f), second(s)
  {
    OWN_SHAREDPTR(this->first);
    OWN_SHAREDPTR(this->second);
  }

  public: InfixOperator(Word pid, SourceLocation const &sl, Char const *t, TioSharedPtr const &f, TioSharedPtr const &s) :
    MetadataHolder(pid, sl), type(t), first(f), second(s)
  {
    OWN_SHAREDPTR(this->first);
    OWN_SHAREDPTR(this->second);
  }

  public: virtual ~InfixOperator()
  {
    DISOWN_SHAREDPTR(this->first);
    DISOWN_SHAREDPTR(this->second);
  }

  public: static SharedPtr<InfixOperator> create()
  {
    return std::make_shared<InfixOperator>();
  }

  public: static SharedPtr<InfixOperator> create(Word pid, SourceLocation const &sl)
  {
    return std::make_shared<InfixOperator>(pid, sl);
  }

  public: static SharedPtr<InfixOperator> create(Word pid, Char const *t, TioSharedPtr const &f, TioSharedPtr const &s)
  {
    return std::make_shared<InfixOperator>(pid, t, f, s);
  }

  public: static SharedPtr<InfixOperator> create(Word pid, SourceLocation const &sl,
                                                 Char const *t, TioSharedPtr const &f, TioSharedPtr const &s)
  {
    return std::make_shared<InfixOperator>(pid, sl, t, f, s);
  }


  //============================================================================
  // Member Functions

  public: void setType(Char const *t)
  {
    this->type = t;
  }

  public: String const& getType() const
  {
    return this->type;
  }

  public: void setFirst(TioSharedPtr const &f)
  {
    UPDATE_OWNED_SHAREDPTR(this->first, f);
  }

  public: TioSharedPtr const& getFirst() const
  {
    return this->first;
  }

  public: void setSecond(TioSharedPtr const &s)
  {
    UPDATE_OWNED_SHAREDPTR(this->second, s);
  }

  public: TioSharedPtr const& getSecond() const
  {
    return this->second;
  }


  //============================================================================
  // Container Implementation

  public: virtual void set(Int index, TiObject *val);

  public: virtual void remove(Int index);

  public: virtual Word getCount() const
  {
    return 2;
  }

  public: virtual TiObject* get(Int index) const;


  //============================================================================
  // MetadataHolder Overrides

  /**
   * @brief Override the original implementation to search the tree if needed.
   * If the value is not yet set for this object and it has children, it will
   * call getSourceLocation on the children.
   */
  public: virtual SourceLocation const& getSourceLocation() const;

  public: virtual TiObject* getAttribute(Char const *name);


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const;


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const;

}; // class


//============================================================================
// Macros

#define DEFINE_AST_INFIX_OPERATOR(X) \
  class X : public InfixOperator, public virtual Clonable, public virtual Printable \
  { \
    TYPE_INFO(X, InfixOperator, "Core.Data.Ast", "Core", "alusus.net"); \
    IMPLEMENT_INTERFACES_2(InfixOperator, Clonable, Printable); \
    public: X() \
    { \
    } \
    public: X(Word pid, SourceLocation const &sl) : \
      InfixOperator(pid, sl) \
    { \
    } \
    public: X(Word pid, Char const *t, TioSharedPtr const &f, TioSharedPtr const &s) : \
      InfixOperator(pid, t, f, s) \
    { \
    } \
    public: X(Word pid, SourceLocation const &sl, Char const *t, TioSharedPtr const &f, TioSharedPtr const &s) : \
      InfixOperator(pid, sl, t, f, s) \
    { \
    } \
    public: static SharedPtr<X> create() \
    { \
      return std::make_shared<X>(); \
    } \
    public: static SharedPtr<X> create(Word pid, SourceLocation const &sl) \
    { \
      return std::make_shared<X>(pid, sl); \
    } \
    public: static SharedPtr<X> create(Word pid, Char const *t, TioSharedPtr const &f, TioSharedPtr const &s) \
    { \
      return std::make_shared<X>(pid, t, f, s); \
    } \
    public: static SharedPtr<X> create(Word pid, SourceLocation const &sl, \
                                       Char const *t, TioSharedPtr const &f, TioSharedPtr const &s) \
    { \
      return std::make_shared<X>(pid, sl, t, f, s); \
    } \
    public: virtual SharedPtr<TiObject> clone() const \
    { \
      SharedPtr<X> newObject = std::make_shared<X>(); \
      newObject->setProdId(this->getProdId()); \
      for (Word i = 0; i < this->getCount(); ++i) { \
        newObject->set(i, this->get(i)); \
      } \
      newObject->setType(this->type); \
      newObject->setSourceLocation(this->getSourceLocation()); \
      return newObject; \
    } \
    public: virtual void print(OutStream &stream, Int indents=0) const \
    { \
      stream << STR(#X) ; \
      Word id = this->getProdId(); \
      if (id != UNKNOWN_ID) { \
        stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("]"); \
      } \
      stream << STR("\n"); \
      printIndents(stream, indents+1); \
      stream << STR("Type: ") << this->type; \
      stream << STR("\n"); \
      printIndents(stream, indents+1); \
      stream << STR("First: "); \
      dumpData(stream, this->first.get(), indents+1); \
      stream << STR("\n"); \
      printIndents(stream, indents+1); \
      stream << STR("Second: "); \
      dumpData(stream, this->second.get(), indents+1); \
    } \
  }

} } } // namespace

#endif
