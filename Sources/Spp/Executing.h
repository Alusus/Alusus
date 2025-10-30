/**
 * @file Spp/Executing.h
 * Contains the header of class Spp::Executing.
 *
 * @copyright Copyright (C) 2025 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_EXECUTING_H
#define SPP_EXECUTING_H

namespace Spp
{

class Executing : public ObjTiInterface
{
  //============================================================================
  // Type Info

  OBJ_INTERFACE_INFO(Executing, ObjTiInterface, "Spp", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: TiObject *owner;


  //============================================================================
  // Constructor

  public: Executing(TiObject *o) : owner(o)
  {
    Basic::initBindingCaches(this->owner, {
      &this->prepareBuild,
      &this->prepareExecutionEntry,
      &this->finalizeExecutionEntry,
      &this->addElementToExecutionEntry,
      &this->execute
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

  /// @name Executing Functions
  /// @{

  public: METHOD_BINDING_CACHE(prepareBuild,
    SharedPtr<BuildSession>, (Int /* buildType */, Char const* /* targetTriple */)
  );

  public: METHOD_BINDING_CACHE(prepareExecutionEntry, void, (BuildSession* /* buildSession */));

  public: METHOD_BINDING_CACHE(finalizeExecutionEntry, Bool, (BuildSession* /* buildSession */));

  public: METHOD_BINDING_CACHE(addElementToExecutionEntry,
    Bool, (TiObject* /* element */, BuildSession* /* buildSession */)
  );

  public: METHOD_BINDING_CACHE(execute,
    Bool, (BuildSession* /* buildSession */)
  );

  /// @}

}; // class

} // namespace

#endif
