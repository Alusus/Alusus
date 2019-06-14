/**
 * @file Spp/Ast/FunctionType.h
 * Contains the header of class Spp::Ast::FunctionType.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_FUNCTIONTYPE_H
#define SPP_AST_FUNCTIONTYPE_H

namespace Spp::Ast
{

class FunctionType : public Type, public MapContaining<TiObject>
{
  //============================================================================
  // Type Info

  TYPE_INFO(FunctionType, Type, "Spp.Ast", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(
      MapContaining<TiObject>
    )
  ));
  OBJECT_FACTORY(FunctionType);


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

  private: SharedPtr<Core::Data::Ast::Map> argTypes;
  private: TioSharedPtr retType;


  //============================================================================
  // Implementations

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (argTypes, Core::Data::Ast::Map, SHARED_REF, setArgTypes(value), argTypes.get()),
    (retType, TiObject, SHARED_REF, setRetType(value), retType.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(FunctionType);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(FunctionType);

  IMPLEMENT_ATTR_CONSTRUCTOR(FunctionType);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(FunctionType);

  public: virtual ~FunctionType()
  {
    DISOWN_SHAREDPTR(this->argTypes);
    DISOWN_SHAREDPTR(this->retType);
  }


  //============================================================================
  // Member Functions

  public: virtual TypeMatchStatus matchTargetType(Type const *type, Helper *helper, ExecutionContext const *ec) const;

  public: void setArgTypes(SharedPtr<Core::Data::Ast::Map> const &args)
  {
    UPDATE_OWNED_SHAREDPTR(this->argTypes, args);
  }
  private: void setArgTypes(Core::Data::Ast::Map *args)
  {
    this->setArgTypes(getSharedPtr(args));
  }

  public: SharedPtr<Core::Data::Ast::Map> const& getArgTypes() const
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
  private: void setRetType(TiObject *ret)
  {
    this->setRetType(getSharedPtr(ret));
  }

  public: TioSharedPtr const& getRetType() const
  {
    return this->retType;
  }

  public: Type* traceRetType(Helper *helper) const;

  public: TypeMatchStatus matchCall(
    Containing<TiObject> *types, Helper *helper, Spp::ExecutionContext const *ec
  );

  public: TypeMatchStatus matchNextArg(
    TiObject *nextType, ArgMatchContext &matchContext, Helper *helper, Spp::ExecutionContext const *ec
  );

}; // class

} // namespace

#endif
