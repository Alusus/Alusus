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
                       public virtual Container, public virtual MetadataHolder
{
  //============================================================================
  // Type Info

  TYPE_INFO(OutfixOperator, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_2(Node, Container, MetadataHolder);


  //============================================================================
  // Member Variables

  protected: String type;
  protected: TioSharedPtr operand;


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
  // Container Implementation

  public: virtual void set(Int index, TiObject *val);

  public: virtual void remove(Int index);

  public: virtual Word getCount() const
  {
    return 1;
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

}; // class


//============================================================================
// Macros

#define DEFINE_AST_OUTFIX_OPERATOR(X) \
  class X : public OutfixOperator, public virtual Clonable, public virtual Printable \
  { \
    TYPE_INFO(X, OutfixOperator, "Core.Data.Ast", "Core", "alusus.net"); \
    IMPLEMENT_INTERFACES_2(OutfixOperator, Clonable, Printable); \
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
      stream << STR("Operand: "); \
      dumpData(stream, this->operand.get(), indents+1); \
    } \
  }

} } } // namespace

#endif
