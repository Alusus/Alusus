/**
 * @file Core/Ast/Map.h
 * Contains the header of class Core::Ast::Map.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_MAP_H
#define CORE_AST_MAP_H

namespace Core::Ast
{

class Map : public SharedMapBase<Node, Node>
{
  //============================================================================
  // Type Info

  typedef SharedMapBase<Node, Node> _MyBase;
  TYPE_INFO(Map, _MyBase, "Core.Ast", "Core", "alusus.org");
  OBJECT_FACTORY(Map);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Map);

  IMPLEMENT_ATTR_CONSTRUCTOR(Map);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Map);

  public: Map(
    std::initializer_list<Argument<TiObject>> const &attrs,
    std::initializer_list<Argument<ContentType>> const &elements,
    Bool useIndex
  ) : _MyBase(useIndex)
  {
    ATTR_INITIALIZATION_LOOP(attrs)
    MAP_ELEMENT_INITIALIZATION_LOOP(elements)
  }

  public: static SharedPtr<Map> create(
    std::initializer_list<Argument<TiObject>> const &attrs,
    std::initializer_list<Argument<ContentType>> const &elements,
    Bool useIndex
  ) {
    return newSrdObj<Map>(attrs, elements, useIndex);
  }

  public: virtual ~Map()
  {
    this->destruct();
  }


  //============================================================================
  // Member Functions

  /// @name Abstract Function Implementations
  /// @{

  protected: virtual SharedPtr<Node> prepareForSet(
    Char const *key, Int index, SharedPtr<Node> const &obj, Bool inherited, Bool newEntry
  ) {
    return obj;
  }

  protected: virtual void finalizeSet(
    Char const *key, Int index, SharedPtr<Node> const &obj, Bool inherited, Bool newEntry
  ) {
    if (obj != 0) obj->setOwner(this);
  }

  protected: virtual void prepareForUnset(
    Char const *key, Int index, SharedPtr<Node> const &obj, Bool inherited
  ) {
    if (obj != 0 && obj->getOwner() == this) obj->setOwner(0);
  }

  /// @}

}; // class

} // namespace

#endif
