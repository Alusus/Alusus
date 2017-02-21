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

class NodePathResolver : public TiObject, public virtual DynamicBindings
{
  //============================================================================
  // Type Info

  TYPE_INFO(NodePathResolver, TiObject, "Spp", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(TiObject, DynamicBindings);


  //============================================================================
  // Member Variables

  private: Core::Data::Seeker *seeker;


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_RT_BINDING;


  //============================================================================
  // Constructors

  NodePathResolver(Core::Data::Seeker *s) : seeker(s)
  {
    this->initialize();
  }

  NodePathResolver(NodePathResolver *parent)
  {
    this->inherit(parent);
    this->seeker = parent->getSeeker();
  }


  //============================================================================
  // Member Functions

  /// @name Initialization
  /// @{

  private: void initialize();

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
    this->call<void, Core::Data::Node const*, StrStream&>(this->resolve, node, path);
  }

  private: Spp::Ast::Type* traceType(TiObject *ref);

  /// @}

  /// @name Path Resolving Functions
  /// @{

  public: BINDING_INDEX_CACHE(resolve, this->getBindingMap());
  public: BINDING_INDEX_CACHE(resolveDefinition, this->getBindingMap());
  public: BINDING_INDEX_CACHE(resolveFunction, this->getBindingMap());
  public: BINDING_INDEX_CACHE(resolveTemplateInstance, this->getBindingMap());

  private: static void _resolve(Bindings *_self, Core::Data::Node const *node, StrStream &path);
  private: static void _resolveDefinition(Bindings *_self, Core::Data::Ast::Definition const *def, StrStream &path);
  private: static void _resolveFunction(Bindings *_self, Spp::Ast::Function const *func, StrStream &path);
  private: static void _resolveTemplateInstance(Bindings *_self, Spp::Ast::Block const *block, StrStream &path);

  /// @}

}; // class

} // namespace

#endif
