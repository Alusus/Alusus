/**
 * @file Spp/Executing.h
 * Contains the header of class Spp::Executing.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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
      &this->addElementToBuild,
      &this->addElementToExecutionEntry,
      &this->execute,
      &this->prepareToExecuteFunction
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

  public: METHOD_BINDING_CACHE(addElementToBuild,
    Bool, (Core::Ast::Node* /* element */, BuildSession* /* buildSession */)
  );

  public: METHOD_BINDING_CACHE(addElementToExecutionEntry,
    Bool, (Core::Ast::Node* /* element */, BuildSession* /* buildSession */)
  );

  public: METHOD_BINDING_CACHE(execute,
    Bool, (BuildSession* /* buildSession */)
  );

  /**
   * @brief Run global constructors then return a pointer to the given function.
   *
   * Unlike execute(), which runs the build session's default execution entry (a nullary function), this
   * runs the global constructors then resolves and returns the JIT address of the given function element
   * (which must have already been generated via addElementToBuild()), without calling it. The caller casts
   * the returned pointer to the function's actual C++ signature and calls it directly, e.g. to call it with
   * arguments that don't fit the nullary execution-entry mechanism. Returns 0 on failure.
   */
  public: METHOD_BINDING_CACHE(prepareToExecuteFunction,
    void*, (Core::Ast::Node* /* element */, BuildSession* /* buildSession */)
  );

  /// @}

}; // class

} // namespace

#endif
