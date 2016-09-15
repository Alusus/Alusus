/**
 * @file Spp/Ast/PointerType.h
 * Contains the header of class Spp::Ast::PointerType.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_POINTERTYPE_H
#define SPP_AST_POINTERTYPE_H

namespace Spp { namespace Ast
{

class PointerType : public ValueType
{
  friend class PointerTypeTemplate;

  //============================================================================
  // Type Info

  TYPE_INFO(PointerType, ValueType, "Spp.Ast", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: ValueType const *contentType;


  //============================================================================
  // Constructors & Destructor

  private: PointerType(ValueType const *contentType);


  //============================================================================
  // Member Functions

  public: ValueType const* getContentType() const
  {
    return this->contentType;
  }

  public: virtual bool isImplicitlyCastableTo(ValueType const *type, ExecutionContext const *context) const
  {
    auto pointerType = tio_cast<PointerType>(type);
    if (pointerType != 0 && pointerType->getContentType() == 0) return true;
  }

  public: virtual bool isExplicitlyCastableTo(ValueType const *type, ExecutionContext const *context) const
  {
    return type->isDerivedFrom<PointerType>();
  }

}; // class

} } // namespace

#endif
