/**
 * @file Spp/Ast/CalleePointer.h
 * Contains the header of class Spp::Ast::CalleePointer.
 *
 * @copyright Copyright (C) 2024 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_CALLEEPOINTER_H
#define SPP_AST_CALLEEPOINTER_H

namespace Spp::Ast
{

class CalleePointer : public Core::Data::Node,
                      public Binding, public MapContaining<TiObject>,
                      public Core::Data::Ast::MetaHaving, public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(CalleePointer, Core::Data::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Data::Node, Binding, MapContaining<TiObject>,
    Core::Data::Ast::MetaHaving, Core::Data::Printable
  );
  OBJECT_FACTORY(CalleePointer);


  //============================================================================
  // Member Variables

  private: TiObject *callee = 0;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(CalleePointer);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (callee, TiObject, PLAIN_REF, setCallee(value), callee)
  );

  IMPLEMENT_AST_MAP_PRINTABLE(CalleePointer);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(CalleePointer);

  IMPLEMENT_ATTR_CONSTRUCTOR(CalleePointer);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(CalleePointer);

  public: virtual ~CalleePointer()
  {
  }


  //============================================================================
  // Member Functions

  public: void setCallee(TiObject *c)
  {
    this->callee = c;
  }

  public: TiObject* getCallee() const
  {
    return this->callee;
  }

}; // class

} // namespace

#endif
