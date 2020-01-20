/**
 * @file Core/Data/Ast/List.h
 * Contains the header of class Core::Data::Ast::List.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_LIST_H
#define CORE_DATA_AST_LIST_H

namespace Core::Data::Ast
{

class List : public NbList,
             public Binding, public MetaHaving, public Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(List, NbList, "Core.Data.Ast", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(Binding, MetaHaving, Printable)
  ));
  OBJECT_FACTORY(List);


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(List);

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

}; // class

} // namespace

#endif
