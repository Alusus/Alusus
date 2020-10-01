/**
 * @file Spp/Ast/NodePathResolver.h
 * Contains the header of class Spp::Ast::NodePathResolver.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_NODEPATHRESOLVER_H
#define SPP_NODEPATHRESOLVER_H

namespace Spp { namespace Ast
{

class NodePathResolver : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(NodePathResolver, TiObject, "Spp.Ast", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(DynamicBinding, DynamicInterfacing),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACING(interfaceList);


  //============================================================================
  // Constructors

  NodePathResolver()
  {
    this->initBindingCaches();
    this->initBindings();
  }

  NodePathResolver(NodePathResolver *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
  }


  //============================================================================
  // Member Functions

  /// @name Initialization
  /// @{

  private: void initBindingCaches();

  private: void initBindings();

  /// @}

  /// @name Helper Functions
  /// @{

  public: Str doResolve(Core::Data::Node const *node, Helper *helper)
  {
    StrStream path;
    this->doResolve(node, helper, path);
    return path.str().c_str();
  }

  public: void doResolve(Core::Data::Node const *node, Helper *helper, StrStream &path)
  {
    this->resolve(node, helper, path);
  }

  /// @}

  /// @name Path Resolving Functions
  /// @{

  public: METHOD_BINDING_CACHE(resolve, void, (Core::Data::Node const*, Helper*, StrStream&));
  private: static void _resolve(TiObject *self, Core::Data::Node const *node, Helper *helper, StrStream &path);

  public: METHOD_BINDING_CACHE(resolveDefinition, void, (Core::Data::Ast::Definition const*, Helper*, StrStream&));
  private: static void _resolveDefinition(
    TiObject *self, Core::Data::Ast::Definition const *def, Helper *helper, StrStream &path
  );

  public: METHOD_BINDING_CACHE(resolveFunction, void, (Spp::Ast::Function const*, Helper*, StrStream&));
  private: static void _resolveFunction(
    TiObject *self, Spp::Ast::Function const *func, Helper *helper, StrStream &path
  );

  public: METHOD_BINDING_CACHE(resolveFunctionType, void, (Spp::Ast::FunctionType const*, Helper*, StrStream&));
  private: static void _resolveFunctionType(
    TiObject *self, Spp::Ast::FunctionType const *funcType, Helper *helper, StrStream &path
  );

  public: METHOD_BINDING_CACHE(resolveFunctionArg, void, (TiObject*, Helper*, StrStream&));
  private: static void _resolveFunctionArg(TiObject *self, TiObject *arg, Helper *helper, StrStream &path);

  public: METHOD_BINDING_CACHE(resolveTemplateInstance, void, (Core::Data::Ast::Scope const*, Helper*, StrStream&));
  private: static void _resolveTemplateInstance(
    TiObject *self, Core::Data::Ast::Scope const *block, Helper *helper, StrStream &path
  );

  public: METHOD_BINDING_CACHE(resolveOther, void, (Core::Data::Node const*, Helper*, StrStream&));
  private: static void _resolveOther(TiObject *self, Core::Data::Node const *node, Helper *helper, StrStream &path);

  /// @}

}; // class

} } // namespace

#endif
