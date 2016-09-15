/**
 * @file Spp/Ast/PointerTypeTemplate.h
 * Contains the header of class Spp::Ast::PointerTypeTemplate.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_POINTERTYPETEMPLATE_H
#define SPP_AST_POINTERTYPETEMPLATE_H

namespace Spp { namespace Ast
{

using namespace Core;

class PointerTypeTemplate : public Core::Data::Node, public virtual Core::Data::IdHolder
{
  //============================================================================
  // Type Info

  TYPE_INFO(PointerTypeTemplate, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES_1(Core::Data::Node, Core::Data::IdHolder);


  //============================================================================
  // Member Variables

  private: std::vector<SharedPtr<PointerType>> pointerTypes;


  //============================================================================
  // Constructors & Destructor

  public: PointerTypeTemplate()
  {
    this->pointerTypes.push_back(std::make_shared<PointerType>(0));
  }

  public: virtual ~PointerTypeTemplate()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual ValueType* getDefaultValueType(Seeker *seeker)
  {
    return this->pointerTypes.first().get();
  }

  public: virtual ValueType* getValueType(TiObject *templateInput, Seeker *seeker);

  public: ValueType* getValueType(ValueType *contentType);

}; // class

} } // namespace

#endif
