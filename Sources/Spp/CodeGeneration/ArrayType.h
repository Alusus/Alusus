/**
 * @file Spp/Ast/ArrayType.h
 * Contains the header of class Spp::Ast::ArrayType.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_ARRAYTYPE_H
#define SPP_AST_ARRAYTYPE_H

namespace Spp { namespace Ast
{

class ArrayType : public ValueType
{
  friend class ArrayTypeTemplate;

  //============================================================================
  // Type Info

  TYPE_INFO(ArrayType, ValueType, "Spp.Ast", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: ValueType const *contentType;

  private: Word size;


  //============================================================================
  // Constructors & Destructor

  private: ArrayType(ValueType const *contentType, Word s);


  //============================================================================
  // Member Functions

  public: ValueType const* getContentType() const
  {
    return this->contentType;
  }

  public: Word getSize() const
  {
    return this->size;
  }

  // TODO:

  public: virtual bool isImplicitlyCastableTo(ValueType const *type, ExecutionContext const *context) const
  {
    auto ArrayType = tio_cast<ArrayType>(type);
    if (ArrayType != 0 && ArrayType->getContentType() == 0) return true;
  }

  public: virtual bool isExplicitlyCastableTo(ValueType const *type, ExecutionContext const *context) const
  {
    return type->isDerivedFrom<ArrayType>();
  }

}; // class

} } // namespace

#endif
