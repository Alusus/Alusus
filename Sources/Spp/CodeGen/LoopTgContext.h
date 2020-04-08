/**
 * @file Spp/CodeGen/LoopTgContext.h
 * Contains the header of class Spp::CodeGen::LoopTgContext.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_LOOPTGCONTEXT_H
#define SPP_CODEGEN_LOOPTGCONTEXT_H

namespace Spp { namespace CodeGen
{

class LoopTgContext : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(LoopTgContext, TiObject, "Spp.CodeGen", "Spp", "alusus.org");


  //============================================================================
  // Constructor & Destructor

  public: LoopTgContext()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual TiObject* getConditionContext() const = 0;
  public: virtual TiObject* getUpdaterContext() const = 0;
  public: virtual TiObject* getBodyContext() const = 0;

}; // class

} } // namespace

#endif
