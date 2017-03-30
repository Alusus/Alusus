/**
 * @file Spp/NodePathResolver.h
 * Contains the header of class Spp::NodePathResolver.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_NODEPATHRESOLVER_H
#define SPP_NODEPATHRESOLVER_H

namespace Spp
{

class NodePathResolver : public TiObject, public virtual DynamicBindings, public virtual DynamicInterfaces
{
  //============================================================================
  // Type Info

  TYPE_INFO(NodePathResolver, TiObject, "Spp", "Spp", "alusus.net", (
    INHERITANCE_INTERFACES(DynamicBindings, DynamicInterfaces),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Member Variables

  private: Core::Data::Seeker *seeker;


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACES(interfaceList);


  //============================================================================
  // Constructors

  NodePathResolver(Core::Data::Seeker *s) : seeker(s)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  NodePathResolver(NodePathResolver *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->seeker = parent->getSeeker();
  }


  //============================================================================
  // Member Functions

  /// @name Initialization
  /// @{

  private: void initBindingCaches();

  private: void initBindings();

  public: Core::Data::Seeker* getSeeker() const
  {
    return this->seeker;
  }

  /// @}

  /// @name Helper Functions
  /// @{

  public: Str doResolve(Core::Data::Node const *node)
  {
    StrStream path;
    this->doResolve(node, path);
    return path.str();
  }

  public: void doResolve(Core::Data::Node const *node, StrStream &path)
  {
    this->resolve(node, path);
  }

  private: Spp::Ast::Type* traceType(TiObject *ref);

  /// @}

  /// @name Path Resolving Functions
  /// @{

  public: METHOD_BINDING_CACHE(resolve, void, (Core::Data::Node const*, StrStream&));
  public: METHOD_BINDING_CACHE(resolveDefinition, void, (Core::Data::Ast::Definition const*, StrStream&));
  public: METHOD_BINDING_CACHE(resolveFunction, void, (Spp::Ast::Function const*, StrStream&));
  public: METHOD_BINDING_CACHE(resolveTemplateInstance, void, (Spp::Ast::Block const*, StrStream&));

  private: static void _resolve(TiObject *self, Core::Data::Node const *node, StrStream &path);
  private: static void _resolveDefinition(TiObject *self, Core::Data::Ast::Definition const *def, StrStream &path);
  private: static void _resolveFunction(TiObject *self, Spp::Ast::Function const *func, StrStream &path);
  private: static void _resolveTemplateInstance(TiObject *self, Spp::Ast::Block const *block, StrStream &path);

  /// @}

}; // class

} // namespace

#endif
