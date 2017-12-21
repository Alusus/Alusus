/**
 * @file Spp/Ast/Function.h
 * Contains the header of class Spp::Ast::Function.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_FUNCTION_H
#define SPP_AST_FUNCTION_H

namespace Spp { namespace Ast
{

class Type;

class Function : public Core::Data::Node,
                 public virtual Core::Basic::Bindings, public virtual Core::Data::MapContainer,
                 public virtual Core::Data::Ast::Metadata,
                 public virtual Core::Data::Clonable, public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Function, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Core::Basic::Bindings, Core::Data::MapContainer,
                                         Core::Data::Ast::Metadata,
                                         Core::Data::Clonable, Core::Data::Printable);


  //============================================================================
  // Types

  public: struct ArgMatchContext
  {
    Int index;
    Int subIndex;
    Type *type;
    ArgMatchContext() : index(-1), subIndex(-1), type(0) {}
  };


  //============================================================================
  // Member Variables

  private: TiStr name;
  private: TiBool inlined;
  private: SharedPtr<Core::Data::SharedMap> argTypes;
  private: TioSharedPtr retType;
  private: SharedPtr<Block> body;


  //============================================================================
  // Implementations

  IMPLEMENT_METADATA(Function);

  IMPLEMENT_BINDINGS(Bindings,
    (name, TiStr, VALUE, setName(value), &name),
    (inlined, TiBool, VALUE, setInlined(value), &inlined),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINER(MapContainer,
                          (Core::Data::SharedMap, argTypes),
                          (TiObject, retType),
                          (Block, body));

  IMPLEMENT_AST_MAP_PRINTABLE(Function, << this->name.get());

  IMPLEMENT_AST_MAP_CLONABLE(Function);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Function);

  IMPLEMENT_ATTR_CONSTRUCTOR(Function);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Function);

  public: virtual ~Function()
  {
    DISOWN_SHAREDPTR(this->argTypes);
    DISOWN_SHAREDPTR(this->retType);
    DISOWN_SHAREDPTR(this->body);
  }


  //============================================================================
  // Member Functions

  public: void setName(Char const *n)
  {
    this->name.set(n);
  }
  public: void setName(TiStr const *n)
  {
    this->setName(n == 0 ? STR("") : n->get());
  }

  public: TiStr const& getName() const
  {
    return this->name;
  }

  public: void setInlined(Bool i)
  {
    this->inlined = i;
  }

  public: Bool getInlined() const
  {
    return this->inlined;
  }

  public: void setArgTypes(SharedPtr<Core::Data::SharedMap> const &args)
  {
    UPDATE_OWNED_SHAREDPTR(this->argTypes, args);
  }

  public: SharedPtr<Core::Data::SharedMap> const& getArgTypes() const
  {
    return this->argTypes;
  }

  public: Word getArgCount() const
  {
    return this->argTypes == 0 ? 0 : this->argTypes->getCount();
  }

  public: Type* traceArgType(Int index, Helper *helper) const;

  public: Bool isVariadic() const;

  public: void setRetType(TioSharedPtr const &ret)
  {
    UPDATE_OWNED_SHAREDPTR(this->retType, ret);
  }

  public: TioSharedPtr const& getRetType() const
  {
    return this->retType;
  }

  public: Type* traceRetType(Helper *helper) const;

  public: void setBody(SharedPtr<Block> const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }

  public: SharedPtr<Block> const& getBody() const
  {
    return this->body;
  }

  public: CallMatchStatus matchCall(Core::Basic::Container<Core::Basic::TiObject> *types, Helper *helper);

  public: CallMatchStatus matchNextArg(Core::Basic::TiObject *nextType, ArgMatchContext &matchContext, Helper *helper);

}; // class

} } // namespace

#endif
