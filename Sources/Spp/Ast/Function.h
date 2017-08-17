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

using namespace Core;
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

  public: s_enum(CallMatchStatus, NONE, CASTED, EXACT);

  public: struct ArgMatchContext
  {
    Int index;
    Int subIndex;
    Type *type;
    ArgMatchContext() : index(-1), subIndex(-1), type(0) {}
  };


  //============================================================================
  // Member Variables

  private: Core::Data::String name;
  private: SharedPtr<Core::Data::SharedMap> argTypes;
  private: TioSharedPtr retType;
  private: SharedPtr<Block> body;


  //============================================================================
  // Implementations

  IMPLEMENT_METADATA(Function);

  IMPLEMENT_BINDINGS(Bindings,
                     (name, Core::Data::String, VALUE, setName(value), &name),
                     (prodId, TiWord, VALUE, setProdId(value), &prodId),
                     (sourceLocation, Core::Data::SourceLocation, VALUE, setSourceLocation(value), &sourceLocation));

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
  public: void setName(Core::Data::String const *n)
  {
    this->setName(n == 0 ? STR("") : n->get());
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

  public: Word getArgCount() const
  {
    return this->argTypes == 0 ? 0 : this->argTypes->getCount();
  }

  public: Type* traceArgType(Int index, Core::Data::Seeker *seeker) const
  {
    if (this->argTypes == 0 || this->argTypes->getCount() == 0) {
      throw EXCEPTION(GenericException, STR("Function takes no arguments."));
    }
    if (this->argTypes == 0 || index < 0 || index >= this->argTypes->getCount()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
    }
    return traceType(this->argTypes->get(index), seeker);
  }

  public: Bool isVariadic() const;

  public: void setRetType(TioSharedPtr const &ret)
  {
    UPDATE_OWNED_SHAREDPTR(this->retType, ret);
  }

  public: TioSharedPtr const& getRetType() const
  {
    return this->retType;
  }

  public: Type* traceRetType(Core::Data::Seeker *seeker) const
  {
    return traceType(this->retType.get(), seeker);
  }

  public: void setBody(SharedPtr<Block> const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }

  public: SharedPtr<Block> const& getBody() const
  {
    return this->body;
  }

  public: CallMatchStatus matchCall(
    Core::Basic::Container<Core::Basic::TiObject> *types, ExecutionContext const *executionContext,
    Core::Standard::RootManager *rootManager
  );

  public: CallMatchStatus matchNextArg(
    Core::Basic::TiObject *nextType, ArgMatchContext &matchContext,
    ExecutionContext const *executionContext, Core::Standard::RootManager *rootManager
  );

}; // class

} } // namespace

#endif
