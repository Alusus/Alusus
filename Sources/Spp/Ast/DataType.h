/**
 * @file Spp/Ast/DataType.h
 * Contains the header of class Spp::Ast::DataType.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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

  private: SharedPtr<Core::Data::Ast::Scope> body;


  //============================================================================
  // Implementations

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (body, Core::Data::Ast::Scope, SHARED_REF, setBody(value), body.get())
  );


  //============================================================================
  // Constructors & Destructor

  public: virtual ~DataType()
  {
    DISOWN_SHAREDPTR(this->body);
  }


  //============================================================================
  // Member Functions

  public: void setBody(SharedPtr<Core::Data::Ast::Scope> const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }
  private: void setBody(Core::Data::Ast::Scope *b)
  {
    this->setBody(getSharedPtr(b));
  }

  public: SharedPtr<Core::Data::Ast::Scope> const& getBody() const
  {
    return this->body;
  }

}; // class

} // namespace

#endif
