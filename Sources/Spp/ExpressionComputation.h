/**
 * @file Spp/ExpressionComputation.h
 * Contains the header of class Spp::ExpressionComputation.
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_EXPRESSIONCOMPUTATION_H
#define SPP_EXPRESSIONCOMPUTATION_H

namespace Spp
{

class ExpressionComputation : public ObjTiInterface
{
  //============================================================================
  // Type Info

  OBJ_INTERFACE_INFO(ExpressionComputation, ObjTiInterface, "Spp", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: TiObject *owner;


  //============================================================================
  // Constructor

  public: ExpressionComputation(TiObject *o) : owner(o)
  {
    Basic::initBindingCaches(this->owner, {
      &this->computeResultType
    });
  }


  //============================================================================
  // Member Functions

  /// @name ObjTiInterface Implementation
  /// @{

  public: virtual TiObject* getTiObject()
  {
    return this->owner;
  }

  public: virtual TiObject const* getTiObject() const
  {
    return this->owner;
  }

  /// @}

  /// @name ExpressionComputation Functions
  /// @{

  public: METHOD_BINDING_CACHE(computeResultType,
    Bool, (TiObject* /* astNode */, TiObject*& /* result */, Bool& /* resultIsValue */)
  );

  /// @}

}; // class

} // namespace

#endif
