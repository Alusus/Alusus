/**
 * @file Core/Data/Ast/OutfixOperator.h
 * Contains the header of class Core::Data::Ast::OutfixOperator.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_OUTFIXOPERATOR_H
#define CORE_DATA_AST_OUTFIXOPERATOR_H

namespace Core::Data::Ast
{

// TODO: DOC

class OutfixOperator : public Node,
                       public virtual Binding, public virtual ExMapContaining<TiObject>, public virtual MetaHaving
{
  //============================================================================
  // Type Info

  TYPE_INFO(OutfixOperator, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Node, Binding, ExMapContaining<TiObject>, MetaHaving);


  //============================================================================
  // Member Variables

  protected: TiStr type;
  protected: TioSharedPtr operand;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(OutfixOperator);

  IMPLEMENT_BINDING(Binding,
    (type, TiStr, VALUE, setType(value), &type),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(ExMapContaining<TiObject>,
    (operand, TiObject, SHARED_REF, setOperand(value), operand.get())
  );


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(OutfixOperator);

  IMPLEMENT_ATTR_CONSTRUCTOR(OutfixOperator);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(OutfixOperator);

  public: virtual ~OutfixOperator()
  {
    DISOWN_SHAREDPTR(this->operand);
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

  public: void setOperand(TioSharedPtr const &o)
  {
    UPDATE_OWNED_SHAREDPTR(this->operand, o);
  }
  private: void setOperand(TiObject *o)
  {
    this->setOperand(getSharedPtr(o));
  }

  public: TioSharedPtr const& getOperand() const
  {
    return this->operand;
  }

}; // class


//==============================================================================
// Macros

#define DEFINE_AST_OUTFIX_OPERATOR(X) \
  class X : public OutfixOperator, public virtual Clonable, public virtual Printable \
  { \
    TYPE_INFO(X, OutfixOperator, "Core.Data.Ast", "Core", "alusus.net"); \
    IMPLEMENT_INTERFACES_2(OutfixOperator, Clonable, Printable); \
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

} // namespace

#endif
