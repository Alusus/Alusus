/**
 * @file Core/Ast/InfixOperator.h
 * Contains the header of class Core::Ast::InfixOperator.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_INFIXOPERATOR_H
#define CORE_AST_INFIXOPERATOR_H

namespace Core::Ast
{

// TODO: DOC

class InfixOperator : public Node,
                      public Binding, public MapContaining<Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(InfixOperator, Node, "Core.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(Node, Binding, MapContaining<Node>);
  OBJECT_FACTORY(InfixOperator);


  //============================================================================
  // Member Variables

  protected: TiStr type;
  protected: SharedPtr<Node> first;
  protected: SharedPtr<Node> second;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (type, TiStr, VALUE, setType(value), &type),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Node>,
    (first, Node, SHARED_REF, setFirst(value), first.get()),
    (second, Node, SHARED_REF, setSecond(value), second.get())
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

  public: void setFirst(SharedPtr<Node> const &f)
  {
    UPDATE_OWNED_SHAREDPTR(this->first, f);
  }
  private: void setFirst(Node *f)
  {
    this->setFirst(getSharedPtr(f));
  }

  public: SharedPtr<Node> const& getFirst() const
  {
    return this->first;
  }

  public: void setSecond(SharedPtr<Node> const &s)
  {
    UPDATE_OWNED_SHAREDPTR(this->second, s);
  }
  private: void setSecond(Node *s)
  {
    this->setSecond(getSharedPtr(s));
  }

  public: SharedPtr<Node> const& getSecond() const
  {
    return this->second;
  }

}; // class


//==============================================================================
// Macros

#define DEFINE_AST_INFIX_OPERATOR(X) \
  class X : public InfixOperator \
  { \
    TYPE_INFO(X, InfixOperator, "Core.Ast", "Core", "alusus.org"); \
    OBJECT_FACTORY(X); \
    IMPLEMENT_AST_MAP_PRINTABLE(X, << this->type.get()); \
    IMPLEMENT_EMPTY_CONSTRUCTOR(X); \
    IMPLEMENT_ATTR_CONSTRUCTOR(X); \
    IMPLEMENT_ATTR_MAP_CONSTRUCTOR(X); \
  }

} // namespace

#endif
