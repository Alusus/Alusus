/**
 * @file Spp/Ast/UseStatement.h
 * Contains the header of class Spp::Ast::UseStatement.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_USESTATEMENT_H
#define SPP_AST_USESTATEMENT_H

namespace Spp::Ast
{

class UseStatement : public Core::Data::Node,
                     public virtual Binding, public virtual MapContaining<TiObject>,
                     public virtual Core::Data::Ast::MetaHaving,
                     public virtual Core::Data::Clonable,
                     public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(UseStatement, Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(Node,
    Binding,
    MapContaining<TiObject>,
    Core::Data::Ast::MetaHaving,
    Core::Data::Clonable,
    Core::Data::Printable
  );


  //============================================================================
  // Member Variables

  private: TioSharedPtr target;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(UseStatement);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (target, TiObject, SHARED_REF, setTarget(value), target.get())
  );

  IMPLEMENT_AST_MAP_CLONABLE(UseStatement);

  IMPLEMENT_AST_MAP_PRINTABLE(UseStatement);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(UseStatement);

  IMPLEMENT_ATTR_CONSTRUCTOR(UseStatement);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(UseStatement);

  public: virtual ~UseStatement()
  {
    DISOWN_SHAREDPTR(this->target);
  }


  //============================================================================
  // Member Functions

  public: void setTarget(TioSharedPtr const &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->target, t);
  }
  private: void setTarget(TiObject *t)
  {
    this->setTarget(getSharedPtr(t));
  }

  public: TioSharedPtr const& getTarget() const
  {
    return this->target;
  }

}; // class

} // namespace

#endif
