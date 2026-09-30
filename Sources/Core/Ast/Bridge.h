/**
 * @file Core/Ast/Bridge.h
 * Contains the header of class Core::Ast::Bridge.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_BRIDGE_H
#define CORE_AST_BRIDGE_H

namespace Core::Ast
{

class Bridge : public Node, public MapContaining<Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(Bridge, Node, "Core.Ast", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(MapContaining<Node>)
  ));
  OBJECT_FACTORY(Bridge);


  //============================================================================
  // Member Variables

  private: SharedPtr<Node> target;


  //============================================================================
  // Implementations

  IMPLEMENT_MAP_CONTAINING(MapContaining<Node>,
    (target, Node, SHARED_REF, setTarget(value), target.get())
  );


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

  public: void setTarget(SharedPtr<Node> const &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->target, t);
  }
  private: void setTarget(Node *t)
  {
    this->setTarget(getSharedPtr(t));
  }

  public: SharedPtr<Node> const& getTarget() const
  {
    return this->target;
  }

}; // class

} // namespace

#endif
