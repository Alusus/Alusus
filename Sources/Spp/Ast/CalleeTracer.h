/**
 * @file Spp/Ast/CalleeTracer.h
 * Contains the header of class Spp::Ast::CalleeTracer.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_CALLEETRACER_H
#define SPP_AST_CALLEETRACER_H

namespace Spp::Ast
{

class CalleeTracer : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(CalleeTracer, TiObject, "Spp.Ast", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(DynamicBinding, DynamicInterfacing),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Member Variables

  private: Helper *helper;


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACING(interfaceList);


  //============================================================================
  // Constructor

  CalleeTracer(Helper *h) : helper(h)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  CalleeTracer(CalleeTracer *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->helper = parent->getHelper();
  }


  //============================================================================
  // Member Functions

  /// @name Initialization
  /// @{

  private: void initBindingCaches();

  private: void initBindings();

  /// @}

  /// @name Property Getters
  /// @{

  public: Helper* getHelper() const
  {
    return this->helper;
  }

  public: Core::Data::Seeker* getSeeker() const
  {
    return this->helper->getSeeker();
  }

  /// @}

  /// @name Main Functions
  /// @{

  public: METHOD_BINDING_CACHE(lookupCallee,
    void, (CalleeLookupRequest& /* request */, CalleeLookupResult& /* result */)
  );
  private: static void _lookupCallee(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result);

  public: METHOD_BINDING_CACHE(lookupCallee_routing,
    void, (CalleeLookupRequest& /* request */, CalleeLookupResult& /* result */)
  );
  private: static void _lookupCallee_routing(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result);

  public: METHOD_BINDING_CACHE(lookupCallee_function,
    void, (CalleeLookupRequest& /* request */, CalleeLookupResult& /* result */)
  );
  private: static void _lookupCallee_function(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result);

  public: METHOD_BINDING_CACHE(lookupCallee_type,
    void, (CalleeLookupRequest& /* request */, CalleeLookupResult& /* result */)
  );
  private: static void _lookupCallee_type(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result);

  public: METHOD_BINDING_CACHE(lookupCallee_template,
    void, (CalleeLookupRequest& /* request */, CalleeLookupResult& /* result */)
  );
  private: static void _lookupCallee_template(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result);

  public: METHOD_BINDING_CACHE(lookupCallee_scope,
    void, (CalleeLookupRequest& /* request */, CalleeLookupResult& /* result */)
  );
  private: static void _lookupCallee_scope(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result);

  public: METHOD_BINDING_CACHE(lookupCallee_argPack,
    void, (CalleeLookupRequest& /* request */, CalleeLookupResult& /* result */)
  );
  private: static void _lookupCallee_argPack(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result);

  public: METHOD_BINDING_CACHE(lookupCallee_var,
    void, (CalleeLookupRequest& /* request */, CalleeLookupResult& /* result */)
  );
  private: static void _lookupCallee_var(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result);

  public: METHOD_BINDING_CACHE(lookupCallee_literal,
    void, (CalleeLookupRequest& /* request */, CalleeLookupResult& /* result */)
  );
  private: static void _lookupCallee_literal(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result);

  public: METHOD_BINDING_CACHE(lookupCallee_funcPtr,
    void, (CalleeLookupRequest& /* request */, CalleeLookupResult& /* result */)
  );
  private: static void _lookupCallee_funcPtr(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result);

  public: METHOD_BINDING_CACHE(lookupCallee_customOp,
    void, (CalleeLookupRequest& /* request */, CalleeLookupResult& /* result */)
  );
  private: static void _lookupCallee_customOp(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result);

  public: METHOD_BINDING_CACHE(lookupCallee_builtInOp,
    void, (CalleeLookupRequest& /* request */, CalleeLookupResult& /* result */)
  );
  private: static void _lookupCallee_builtInOp(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result);

  /// @}

  /// @name Helper Functions
  /// @{

  private: static void selectBetterResult(CalleeLookupResult const &newResult, CalleeLookupResult &result);

  private: static Char const* findOperationModifier(Core::Data::Ast::Definition *def);

  /// @}

}; // class

} // namespace

#endif
