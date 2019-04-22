/**
 * @file Core/Data/Ast/Map.h
 * Contains the header of class Core::Data::Ast::Map.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_MAP_H
#define CORE_DATA_AST_MAP_H

namespace Core::Data::Ast
{

class Map : public NbMap,
            public Binding, public MetaHaving, public Clonable, public Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Map, NbMap, "Core.Data.Ast", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(Binding, MetaHaving, Clonable, Printable)
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(Map);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_AST_MAP_CLONABLE(Map);

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
    return std::make_shared<Map>(attrs, elements, useIndex);
  }

}; // class

} // namespace

#endif
