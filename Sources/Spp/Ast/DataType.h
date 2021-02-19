/**
 * @file Spp/Ast/DataType.h
 * Contains the header of class Spp::Ast::DataType.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_DATATYPE_H
#define SPP_AST_DATATYPE_H

namespace Spp::Ast
{

class DataType : public Type, public MapContaining<TiObject>
{
  //============================================================================
  // Type Info

  TYPE_INFO(DataType, Type, "Spp.Ast", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(
      MapContaining<TiObject>
    )
  ));


  //============================================================================
  // Member Variables

  private: SharedPtr<Spp::Ast::Block> body;


  //============================================================================
  // Implementations

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (body, Spp::Ast::Block, SHARED_REF, setBody(value), body.get())
  );


  //============================================================================
  // Constructors & Destructor

  public: virtual ~DataType()
  {
    DISOWN_SHAREDPTR(this->body);
  }


  //============================================================================
  // Member Functions

  public: void setBody(SharedPtr<Spp::Ast::Block> const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }
  private: void setBody(Spp::Ast::Block *b)
  {
    this->setBody(getSharedPtr(b));
  }

  public: SharedPtr<Spp::Ast::Block> const& getBody() const
  {
    return this->body;
  }

}; // class

} // namespace

#endif
