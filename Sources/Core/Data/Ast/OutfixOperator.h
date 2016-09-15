/**
 * @file Core/Data/Ast/OutfixOperator.h
 * Contains the header of class Core::Data::Ast::OutfixOperator.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_OUTFIXOPERATOR_H
#define CORE_DATA_AST_OUTFIXOPERATOR_H

namespace Core { namespace Data { namespace Ast
{

// TODO: DOC

class OutfixOperator : public Node,
                       public virtual MapContainer, public virtual MetadataHolder
{
  //============================================================================
  // Type Info

  TYPE_INFO(OutfixOperator, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Node, MapContainer, MetadataHolder);


  //============================================================================
  // Member Variables

  protected: String type;
  protected: TioSharedPtr operand;

  IMPLEMENT_MAP_CONTAINER((TiObject, operand));


  //============================================================================
  // Constructors & Destructor

  public: OutfixOperator()
  {
  }

  public: OutfixOperator(Word pid, SourceLocation const &sl) :
    MetadataHolder(pid, sl)
  {
  }

  public: OutfixOperator(Word pid, Char const *t, TioSharedPtr const &o) :
    MetadataHolder(pid), type(t), operand(o)
  {
    OWN_SHAREDPTR(this->operand);
  }

  public: OutfixOperator(Word pid, SourceLocation const &sl, Char const *t, TioSharedPtr const &o) :
    MetadataHolder(pid, sl), type(t), operand(o)
  {
    OWN_SHAREDPTR(this->operand);
  }

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

  public: String const& getType() const
  {
    return this->type;
  }

  public: void setOperand(TioSharedPtr const &o)
  {
    UPDATE_OWNED_SHAREDPTR(this->operand, o);
  }

  public: TioSharedPtr const& getOperand() const
  {
    return this->operand;
  }


  //============================================================================
  // MetadataHolder Overrides

  public: virtual TiObject* getAttribute(Char const *name)
  {
    if (SBSTR(name) == STR("type")) {
      return &this->type;
    }
    return MetadataHolder::getAttribute(name);
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
    public: X() \
    { \
    } \
    public: X(Word pid, SourceLocation const &sl) : \
      OutfixOperator(pid, sl) \
    { \
    } \
    public: X(Word pid, Char const *t, TioSharedPtr const &o) : \
      OutfixOperator(pid, t, o) \
    { \
    } \
    public: X(Word pid, SourceLocation const &sl, Char const *t, TioSharedPtr const &o) : \
      OutfixOperator(pid, sl, t, o) \
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
    public: static SharedPtr<X> create(Word pid, Char const *t, TioSharedPtr const &o) \
    { \
      return std::make_shared<X>(pid, t, o); \
    } \
    public: static SharedPtr<X> create(Word pid, SourceLocation const &sl, \
                                       Char const *t, TioSharedPtr const &o) \
    { \
      return std::make_shared<X>(pid, sl, t, o); \
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
  }

} } } // namespace

#endif
