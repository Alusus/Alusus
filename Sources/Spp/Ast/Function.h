/**
 * @file Spp/Ast/Function.h
 * Contains the header of class Spp::Ast::Function.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_FUNCTION_H
#define SPP_AST_FUNCTION_H

namespace Spp::Ast
{

class FunctionType;

class Function : public Core::Ast::Node,
                 public Binding, public MapContaining<Core::Ast::Node>,
                 public Core::Ast::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Function, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, Binding, MapContaining<Core::Ast::Node>,
    Core::Ast::Printable
  );
  OBJECT_FACTORY(Function);


  //============================================================================
  // Member Variables

  private: TiStr name;
  private: TiBool inlined;
  private: SharedPtr<FunctionType> type;
  private: SharedPtr<Core::Ast::Scope> body;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (name, TiStr, VALUE, setName(value), &name),
    (inlined, TiBool, VALUE, setInlined(value), &inlined),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Ast::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>,
    (type, FunctionType, SHARED_REF, setType(value), type.get()),
    (body, Core::Ast::Scope, SHARED_REF, setBody(value), body.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(Function, << this->name.get());


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Function);

  IMPLEMENT_ATTR_CONSTRUCTOR(Function);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Function);

  public: virtual ~Function()
  {
    DISOWN_SHAREDPTR(this->type);
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
    this->setName(n == 0 ? S("") : n->get());
  }

  public: TiStr const& getName() const
  {
    return this->name;
  }

  public: void setInlined(Bool i)
  {
    this->inlined = i;
  }
  public: void setInlined(TiBool const *i)
  {
    this->setInlined(i == 0 ? false : i->get());
  }

  public: Bool getInlined() const
  {
    return this->inlined;
  }

  public: void setType(SharedPtr<FunctionType> const &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->type, t);
  }
  public: void setType(FunctionType *t)
  {
    this->setType(getSharedPtr(t));
  }

  public: SharedPtr<FunctionType> const& getType() const
  {
    return this->type;
  }

  public: void setBody(SharedPtr<Core::Ast::Scope> const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }
  private: void setBody(Core::Ast::Scope *b)
  {
    this->setBody(getSharedPtr(b));
  }

  public: SharedPtr<Core::Ast::Scope> const& getBody() const
  {
    return this->body;
  }

}; // class

} // namespace

#endif
