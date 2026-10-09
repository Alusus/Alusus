/**
 * @file Spp/Ast/Variable.h
 * Contains the header of class Spp::Ast::Variable.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_VARIABLE_H
#define SPP_AST_VARIABLE_H

namespace Spp::Ast
{

class Variable : public Core::Ast::Node,
                 public MapContaining<Core::Ast::Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(Variable, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, MapContaining<Core::Ast::Node>
  );
  OBJECT_FACTORY(Variable);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Ast::Node> typeRef;
  private: TiBool valueOnly;
  private: Type *type = 0;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Core::Ast::Node,
    (valueOnly, TiBool, VALUE, setValueOnly(value), &valueOnly)
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>,
    (typeRef, Core::Ast::Node, SHARED_REF, setTypeRef(value), typeRef.get()),
    (type, Type, PLAIN_REF, setType(value), type)
  );

  IMPLEMENT_AST_PRINTABLE(Variable, (this->valueOnly ? S("valueOnly") : S("inMem")));


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Variable);

  IMPLEMENT_ATTR_CONSTRUCTOR(Variable);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Variable);

  public: virtual ~Variable()
  {
    DISOWN_SHAREDPTR(this->typeRef);
  }


  //============================================================================
  // Member Functions

  public: void setTypeRef(SharedPtr<Core::Ast::Node> const &r)
  {
    UPDATE_OWNED_SHAREDPTR(this->typeRef, r);
  }
  private: void setTypeRef(Core::Ast::Node *r)
  {
    this->setTypeRef(getSharedPtr(r));
  }

  public: SharedPtr<Core::Ast::Node> const& getTypeRef() const
  {
    return this->typeRef;
  }

  public: void setValueOnly(Bool vo)
  {
    this->valueOnly = vo;
  }
  public: void setValueOnly(TiBool const *vo)
  {
    this->valueOnly = vo == 0 ? false : vo->get();
  }

  public: Bool isValueOnly() const
  {
    return this->valueOnly.get();
  }

  public: void setType(Type *t)
  {
    this->type = t;
  }

  public: Type* getType() const
  {
    return this->type;
  }

}; // class

} // namespace

#endif
