/**
 * @file Spp/Ast/Type.h
 * Contains the header of class Spp::Ast::Type.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_TYPE_H
#define SPP_AST_TYPE_H

namespace Spp::Ast
{

using namespace Core;

class Type : public Core::Data::Node,
             public virtual Binding, public virtual MapContaining<TiObject>,
             public virtual Core::Data::Ast::MetaHaving,
             public virtual Core::Data::Clonable, public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Type, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net", (
    INHERITANCE_INTERFACES(
      Binding,
      MapContaining<TiObject>,
      Core::Data::Ast::MetaHaving,
      Core::Data::Clonable,
      Core::Data::Printable
    )
  ));


  //============================================================================
  // Member Variables

  private: SharedPtr<Block> body;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(Type);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>, (body, Block, setBody(value), body.get()));

  IMPLEMENT_AST_MAP_PRINTABLE(Type);


  //============================================================================
  // Constructor / Destructor

  public: virtual ~Type()
  {
    DISOWN_SHAREDPTR(this->body);
  }


  //============================================================================
  // Member Functions

  public: void setBody(SharedPtr<Block> const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }
  private: void setBody(Block *b)
  {
    this->setBody(getSharedPtr(b));
  }

  public: SharedPtr<Block> const& getBody() const
  {
    return this->body;
  }

  public: virtual Bool isImplicitlyCastableTo(Type const *type, Helper *helper, Spp::ExecutionContext const *ec) const = 0;

  public: virtual Bool isExplicitlyCastableTo(Type const *type, Helper *helper, Spp::ExecutionContext const *ec) const = 0;

}; // class

} // namespace

#endif
