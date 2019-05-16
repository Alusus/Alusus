/**
 * @file Spp/CodeGen/IfTgContext.h
 * Contains the header of class Spp::CodeGen::IfTgContext.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_IFTGCONTEXT_H
#define SPP_CODEGEN_IFTGCONTEXT_H

namespace Spp { namespace CodeGen
{

class IfTgContext : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(IfTgContext, TiObject, "Spp.CodeGen", "Spp", "alusus.org");


  //============================================================================
  // Constructor & Destructor

  public: IfTgContext()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual TiObject* getConditionContext() const = 0;
  public: virtual TiObject* getBodyContext() const = 0;
  public: virtual TiObject* getElseContext() const = 0;

}; // class

} } // namespace

#endif
