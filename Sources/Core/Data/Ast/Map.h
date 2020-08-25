/**
 * @file Core/Data/Ast/Map.h
 * Contains the header of class Core::Data::Ast::Map.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_MAP_H
#define CORE_DATA_AST_MAP_H

namespace Core::Data::Ast
{

class Map : public NbMap,
            public Binding, public MetaHaving, public Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Map, NbMap, "Core.Data.Ast", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(Binding, MetaHaving, Printable)
  ));
  OBJECT_FACTORY(Map);


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(Map);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(Map);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Map);

  IMPLEMENT_ATTR_CONSTRUCTOR(Map);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Map);

  public: Map(
    std::initializer_list<Argument> const &attrs, std::initializer_list<Argument> const &elements, Bool useIndex
  ) : NbMap(useIndex)
  {
    ATTR_INITIALIZATION_LOOP(attrs)
    MAP_ELEMENT_INITIALIZATION_LOOP(elements)
  }

  public: static SharedPtr<Map> create(
    std::initializer_list<Argument> const &attrs, std::initializer_list<Argument> const &elements, Bool useIndex
  ) {
    return newSrdObj<Map>(attrs, elements, useIndex);
  }

}; // class

} // namespace

#endif
