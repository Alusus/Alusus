/**
 * @file Core/Main/RootScopeHandler.h
 * Contains the header of class Core::Main::RootScopeHandler.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_MAIN_ROOTSCOPEHANDLER_H
#define CORE_MAIN_ROOTSCOPEHANDLER_H

namespace Core::Main
{

class RootScopeHandler : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(RootScopeHandler, TiObject, "Core.Main", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(DynamicBinding, DynamicInterfacing),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACING(interfaceList);


  //============================================================================
  // Member Variables

  private: Ast::Seeker *seeker;

  private: SharedPtr<Ast::Scope> rootScope;


  //============================================================================
  // Constructors & Destructor

  public: RootScopeHandler()
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: virtual ~RootScopeHandler()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void initBindingCaches();
  private: void initBindings();

  public: void setRootScope(SharedPtr<Ast::Scope> const &s)
  {
    this->rootScope = s;
  }

  public: SharedPtr<Ast::Scope> const& getRootScope() const
  {
    return this->rootScope;
  }

  public: void setSeeker(Ast::Seeker *s)
  {
    this->seeker = s;
  }

  public: Ast::Seeker* getSeeker() const
  {
    return this->seeker;
  }

  /// @}

  /// @name Main Functions
  /// @{

  public: METHOD_BINDING_CACHE(addNewElement,
    void, (SharedPtr<Ast::Node> const& /* data */, Processing::Parser* /* parser */, Processing::ParserState* /* state */)
  );
  private: static void _addNewElement(TiObject *self, SharedPtr<Ast::Node> const &data,
    Core::Processing::Parser *parser, Processing::ParserState *state
  );

  /// @}

}; // class

} // namespace

#endif
