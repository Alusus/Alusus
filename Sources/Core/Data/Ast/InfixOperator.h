/**
 * @file Core/Data/Ast/InfixOperator.h
 * Contains the header of class Core::Data::Ast::InfixOperator.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
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
                      public Binding, public MapContaining<TiObject>, public MetaHaving,
                      public Clonable, public Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(InfixOperator, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Node, Binding, MapContaining<TiObject>, MetaHaving, Clonable, Printable);


  //============================================================================
  // Member Variables

  protected: TiStr type;
  protected: TioSharedPtr first;
  protected: TioSharedPtr second;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(InfixOperator);

  IMPLEMENT_BINDING(Binding,
    (type, TiStr, VALUE, setType(value), &type),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (first, TiObject, SHARED_REF, setFirst(value), first.get()),
    (second, TiObject, SHARED_REF, setSecond(value), second.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(InfixOperator, << this->type.get());


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(InfixOperator);

  IMPLEMENT_ATTR_CONSTRUCTOR(InfixOperator);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(InfixOperator);

  public: virtual ~InfixOperator()
  {
    DISOWN_SHAREDPTR(this->first);
    DISOWN_SHAREDPTR(this->second);
  }


  //============================================================================
  // Member Functions

  public: void setType(Char const *t)
  {
    this->type = t;
  }
  public: void setType(TiStr const *t)
  {
    this->type = t == 0 ? "" : t->get();
  }

  public: TiStr const& getType() const
  {
    return this->type;
  }

  public: void setFirst(TioSharedPtr const &f)
  {
    UPDATE_OWNED_SHAREDPTR(this->first, f);
  }
  private: void setFirst(TiObject *f)
  {
    this->setFirst(getSharedPtr(f));
  }

  public: TioSharedPtr const& getFirst() const
  {
    return this->first;
  }

  public: void setSecond(TioSharedPtr const &s)
  {
    UPDATE_OWNED_SHAREDPTR(this->second, s);
  }
  private: void setSecond(TiObject *s)
  {
    this->setSecond(getSharedPtr(s));
  }

  public: TioSharedPtr const& getSecond() const
  {
    return this->second;
  }


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const;

}; // class


//==============================================================================
// Macros

#define DEFINE_AST_INFIX_OPERATOR(X) \
  class X : public InfixOperator \
  { \
    TYPE_INFO(X, InfixOperator, "Core.Data.Ast", "Core", "alusus.net"); \
    IMPLEMENT_INTERFACES_2(InfixOperator, Clonable, Printable); \
    IMPLEMENT_AST_MAP_PRINTABLE(X, << this->type.get()); \
    IMPLEMENT_EMPTY_CONSTRUCTOR(X); \
    IMPLEMENT_ATTR_CONSTRUCTOR(X); \
    IMPLEMENT_ATTR_MAP_CONSTRUCTOR(X); \
    public: virtual SharedPtr<TiObject> clone() const \
    { \
      SharedPtr<X> newObject = std::make_shared<X>(); \
      newObject->setProdId(this->getProdId()); \
      for (Word i = 0; i < this->getElementCount(); ++i) { \
        newObject->setElement(i, this->getElement(i)); \
      } \
      newObject->setType(this->type); \
      newObject->setSourceLocation(this->getSourceLocation()); \
      return newObject; \
    } \
  }

} } } // namespace

#endif
