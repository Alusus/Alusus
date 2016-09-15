/**
 * @file Spp/Ast/Function.h
 * Contains the header of class Spp::Ast::Function.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
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

using namespace Core;

class Function : public Core::Data::Node,
                 public virtual Core::Data::MapContainer, public virtual Core::Data::IdHolder,
                 public virtual Core::Data::Ast::MetadataHolder, public virtual Core::Data::Clonable,
                 public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Function, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Core::Data::MapContainer, Core::Data::IdHolder,
                                         Core::Data::Ast::MetadataHolder,
                                         Core::Data::Clonable, Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: Core::Data::String name;
  private: SharedPtr<Core::Data::SharedMap> argTypes;
  private: Core::Data::Integer variadic;
  private: TioSharedPtr retType;
  private: SharedPtr<Block> body;

  IMPLEMENT_MAP_CONTAINER((Core::Data::SharedMap, argTypes),
                          (TiObject, retType),
                          (Block, body));

  IMPLEMENT_AST_MAP_PRINTABLE(Function, << this->name.get()
                                        << (this->variadic.get() != 0 ? STR(" ...variadic") : STR("")));


  //============================================================================
  // Constructors & Destructor

  public: Function()
  {
  }

  public: Function(Char const *name, SharedPtr<Core::Data::SharedMap> const &args, Bool var, TioSharedPtr const &ret,
                   SharedPtr<Block> const &body) :
    name(name), argTypes(args), variadic(var), retType(ret), body(body)
  {
    OWN_SHAREDPTR(this->argTypes);
    OWN_SHAREDPTR(this->retType);
    OWN_SHAREDPTR(this->body);
  }

  public: virtual ~Function()
  {
    DISOWN_SHAREDPTR(this->argTypes);
    DISOWN_SHAREDPTR(this->retType);
    DISOWN_SHAREDPTR(this->body);
  }


  //============================================================================
  // Member Functions

  public: virtual Str const& getQualifier() const
  {
    return ID_GENERATOR->getDesc(this->getId());
  }

  public: void setName(Char const *n)
  {
    this->name.set(n);
  }

  public: Core::Data::String const* getName() const
  {
    return &this->name;
  }

  public: void setArgTypes(SharedPtr<Core::Data::SharedMap> const &args)
  {
    UPDATE_OWNED_SHAREDPTR(this->argTypes, args);
  }

  public: SharedPtr<Core::Data::SharedMap> const& getArgTypes() const
  {
    return this->argTypes;
  }

  public: void setVariadic(Bool v)
  {
    this->variadic.set(v);
  }

  public: Bool isVariadic() const
  {
    return this->variadic.get() != 0;
  }

  public: void setRetType(TioSharedPtr const &ret)
  {
    UPDATE_OWNED_SHAREDPTR(this->retType, ret);
  }

  public: TioSharedPtr const& getRetType() const
  {
    return this->retType;
  }

  public: void setBody(SharedPtr<Block> const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }

  public: SharedPtr<Block> const& getBody() const
  {
    return this->body;
  }


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const;

}; // class

} } // namespace

#endif
