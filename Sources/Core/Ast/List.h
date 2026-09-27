/**
 * @file Core/Ast/List.h
 * Contains the header of class Core::Ast::List.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_LIST_H
#define CORE_AST_LIST_H

namespace Core::Ast
{

class List : public SharedListBase<Node, Node>,
             public Binding
{
  //============================================================================
  // Type Info

  typedef SharedListBase<Node, Node> _MyBase;
  TYPE_INFO(List, _MyBase, "Core.Ast", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(Binding)
  ));
  OBJECT_FACTORY(List);


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_AST_LIST_PRINTABLE(List);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(List);

  IMPLEMENT_ATTR_CONSTRUCTOR(List);

  IMPLEMENT_ATTR_LIST_CONSTRUCTOR(List);

  public: virtual ~List()
  {
    this->destruct();
  }


  //============================================================================
  // Member Functions

  /// @name Abstract Function Implementations
  /// @{

  protected: virtual SharedPtr<Node> prepareForSet(
    Int index, SharedPtr<Node> const &obj, Bool inherited, Bool newEntry
  ) {
    return obj;
  }

  protected: virtual void finalizeSet(
    Int index, SharedPtr<Node> const &obj, Bool inherited, Bool newEntry
  ) {
    if (obj != 0) obj->setOwner(this);
  }

  protected: virtual void prepareForUnset(
    Int index, SharedPtr<Node> const &obj, Bool inherited
  ) {
    if (obj != 0 && obj->getOwner() == this) obj->setOwner(0);
  }

  /// @}

}; // class

} // namespace

#endif
