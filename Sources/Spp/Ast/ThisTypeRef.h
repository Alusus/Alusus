/**
 * @file Spp/Ast/ThisTypeRef.h
 * Contains the header of class Spp::Ast::ThisTypeRef.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_THISTYPEREF_H
#define SPP_AST_THISTYPEREF_H

namespace Spp::Ast
{

class ThisTypeRef : public Core::Data::Node,
                    public Binding, public Core::Data::Ast::MetaHaving, public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(ThisTypeRef, Core::Data::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Data::Node, Binding, Core::Data::Ast::MetaHaving, Core::Data::Printable
  );
  OBJECT_FACTORY(ThisTypeRef);


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(ThisTypeRef);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ThisTypeRef);

  IMPLEMENT_ATTR_CONSTRUCTOR(ThisTypeRef);

  public: virtual ~ThisTypeRef()
  {
  }


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const
  {
    stream << S("ThisTypeRef");
    Word id = this->getProdId();
    if (id != UNKNOWN_ID) {
      stream << S(" [") << ID_GENERATOR->getDesc(id) << S("]");
    }
  }

}; // class

} // namespace

#endif
