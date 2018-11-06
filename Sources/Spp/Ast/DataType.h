/**
 * @file Spp/Ast/DataType.h
 * Contains the header of class Spp::Ast::DataType.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_DATATYPE_H
#define SPP_AST_DATATYPE_H

namespace Spp::Ast
{

class DataType : public Type, public virtual ExMapContaining<TiObject>
{
  //============================================================================
  // Type Info

  TYPE_INFO(DataType, Type, "Spp.Ast", "Spp", "alusus.net", (
    INHERITANCE_INTERFACES(
      ExMapContaining<TiObject>
    )
  ));


  //============================================================================
  // Member Variables

  private: SharedPtr<Block> body;


  //============================================================================
  // Implementations

  IMPLEMENT_MAP_CONTAINING(ExMapContaining<TiObject>, (body, Block, SHARED_REF, setBody(value), body.get()));


  //============================================================================
  // Constructors & Destructor

  public: virtual ~DataType()
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

}; // class

} // namespace

#endif
