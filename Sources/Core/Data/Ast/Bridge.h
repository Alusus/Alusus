/**
 * @file Core/Data//Ast/Bridge.h
 * Contains the header of class Core::Data::Ast::Bridge.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_BRIDGE_H
#define CORE_DATA_AST_BRIDGE_H

namespace Core::Data::Ast
{

class Bridge : public Node,
               public Binding, public MapContaining<TiObject>, public Core::Data::Ast::MetaHaving,
               public Core::Data::Clonable, public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Bridge, Node, "Core.Data.Ast", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(
      Binding, MapContaining<TiObject>, Core::Data::Ast::MetaHaving, Core::Data::Clonable, Core::Data::Printable
    )
  ));
  OBJECT_FACTORY(Bridge);


  //============================================================================
  // Member Variables

  private: TioSharedPtr target;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(Bridge);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (target, TiObject, SHARED_REF, setTarget(value), target.get())
  );

  IMPLEMENT_AST_MAP_CLONABLE(Bridge);

  IMPLEMENT_AST_MAP_PRINTABLE(Bridge);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Bridge);

  IMPLEMENT_ATTR_CONSTRUCTOR(Bridge);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Bridge);

  public: virtual ~Bridge()
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
