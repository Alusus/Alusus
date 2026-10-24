/**
 * @file Spp/SeekerExtension.h
 * Contains the header of class Spp::SeekerExtension.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_SEEKEREXTENSION_H
#define SPP_SEEKEREXTENSION_H

namespace Spp
{

class SeekerExtension : public ObjTiInterface
{
  //============================================================================
  // Type Info

  OBJ_INTERFACE_INFO(SeekerExtension, ObjTiInterface, "Spp", "Spp", "alusus.org");


  //============================================================================
  // Types

  public: ti_s_enum(Action, TiInt, "Spp", "Spp", "alusus.org",
    CHILD_SCOPE = 100
  );

  public: s_enum(Flags,
    SKIP_CHILDREN = (1 << 16)
  );


  //============================================================================
  // Member Variables

  private: TiObject *owner;


  //============================================================================
  // Constructor

  public: SeekerExtension(TiObject *o) : owner(o)
  {
    Basic::initBindingCaches(this->owner, {
      &this->astHelper
    });
    this->foreach.init(this->owner, &SeekerExtension::_foreach);
    this->extForeach.init(this->owner, &SeekerExtension::_extForeach);
    this->foreach_identifierLevel.init(this->owner, &SeekerExtension::_foreach_identifierLevel);
    this->foreach_linkOperatorRouting.init(this->owner, &SeekerExtension::_foreach_linkOperatorRouting);
    this->foreach_identifierOnFunc.init(this->owner, &SeekerExtension::_foreach_identifierOnFunc);
    this->foreach_identifierOnDataType.init(this->owner, &SeekerExtension::_foreach_identifierOnDataType);
    this->foreach_paramPass.init(this->owner, &SeekerExtension::_foreach_paramPass);
    this->foreach_paramPassRouting.init(this->owner, &SeekerExtension::_foreach_paramPassRouting);
    this->foreach_paramPassOnTemplate.init(this->owner, &SeekerExtension::_foreach_paramPassOnTemplate);
    this->foreach_thisTypeRef.init(this->owner, &SeekerExtension::_foreach_thisTypeRef);
    this->foreach_comparison.init(this->owner, &SeekerExtension::_foreach_comparison);
    this->foreach_comparisonLevel.init(this->owner, &SeekerExtension::_foreach_comparisonLevel);
    this->foreach_comparisonOnScope.init(this->owner, &SeekerExtension::_foreach_comparisonOnScope);
    this->foreach_computeComparison.init(this->owner, &SeekerExtension::_foreach_computeComparison);
  }


  //============================================================================
  // Member Properties

  public: BINDING_CACHE(astHelper, Ast::Helper);


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

  /// @name Setup Functions
  /// @{

  public: static void extend(Core::Ast::Seeker *seeker, SharedPtr<Ast::Helper> const &astHelper);
  public: static void unextend(Core::Ast::Seeker *seeker);

  /// @}

  /// @name Seek Functions
  /// @{

  // The following caches override bindings of the Seeker. The overrides get removed automatically when this
  // extension is destroyed.

  public: METHOD_BINDING_CACHE(foreach,
    Core::Ast::Seeker::Verb,
    (Core::Ast::Node const*, Core::Ast::Node*, Core::Ast::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Ast::Seeker::Verb _foreach(
    TiObject *self, Core::Ast::Node const *ref, Core::Ast::Node *target,
    Core::Ast::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(extForeach,
    Core::Ast::Seeker::Verb,
    (Core::Ast::Node const*, Core::Ast::Node*, Core::Ast::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Ast::Seeker::Verb _extForeach(
    TiObject *self, Core::Ast::Node const *ref, Core::Ast::Node *target,
    Core::Ast::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifierLevel,
    Core::Ast::Seeker::Verb,
    (Core::Ast::Identifier const*, Core::Ast::Node*, Core::Ast::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Ast::Seeker::Verb _foreach_identifierLevel(
    TiObject *self, Core::Ast::Identifier const *identifier, Core::Ast::Node *data,
    Core::Ast::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifierOnFunc,
    Core::Ast::Seeker::Verb, (
      Core::Ast::Identifier const*, Ast::Function*, Core::Ast::Seeker::ForeachCallback const&, Word
    )
  );
  private: static Core::Ast::Seeker::Verb _foreach_identifierOnFunc(
    TiObject *self, Core::Ast::Identifier const *identifier, Ast::Function *function,
    Core::Ast::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifierOnDataType,
    Core::Ast::Seeker::Verb, (
      Core::Ast::Identifier const*, Ast::DataType*, Core::Ast::Seeker::ForeachCallback const&, Word
    )
  );
  private: static Core::Ast::Seeker::Verb _foreach_identifierOnDataType(
    TiObject *self, Core::Ast::Identifier const *identifier, Ast::DataType *type,
    Core::Ast::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_linkOperatorRouting,
    Core::Ast::Seeker::Verb,
    (Core::Ast::LinkOperator const*, Core::Ast::Node*, Core::Ast::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Ast::Seeker::Verb _foreach_linkOperatorRouting(
    TiObject *self, Core::Ast::LinkOperator const *link, Core::Ast::Node *data,
    Core::Ast::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_paramPass,
    Core::Ast::Seeker::Verb, (Core::Ast::ParamPass const*, Core::Ast::Node*, Core::Ast::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Ast::Seeker::Verb _foreach_paramPass(
    TiObject *self, Core::Ast::ParamPass const *paramPass, Core::Ast::Node *data,
    Core::Ast::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_paramPassRouting,
    Core::Ast::Seeker::Verb, (
      Core::Ast::ParamPass const*, Core::Ast::Node*, Core::Ast::Seeker::ForeachCallback const&, Word
    )
  );
  private: static Core::Ast::Seeker::Verb _foreach_paramPassRouting(
    TiObject *self, Core::Ast::ParamPass const *paramPass, Core::Ast::Node *data,
    Core::Ast::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_paramPassOnTemplate,
    Core::Ast::Seeker::Verb,
    (Core::Ast::Node*, Spp::Ast::Template*, Core::Ast::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Ast::Seeker::Verb _foreach_paramPassOnTemplate(
    TiObject *self, Core::Ast::Node *param, Spp::Ast::Template *tmplt,
    Core::Ast::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_thisTypeRef,
    Core::Ast::Seeker::Verb, (Core::Ast::Node*, Core::Ast::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Ast::Seeker::Verb _foreach_thisTypeRef(
    TiObject *self, Core::Ast::Node *data, Core::Ast::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_comparison,
    Core::Ast::Seeker::Verb, (Core::Ast::Node const*, Core::Ast::Node*, Core::Ast::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Ast::Seeker::Verb _foreach_comparison(
    TiObject *self, Core::Ast::Node const *comparison, Core::Ast::Node *data,
    Core::Ast::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_comparisonLevel,
    Core::Ast::Seeker::Verb, (Core::Ast::Node const*, Core::Ast::Node*, Core::Ast::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Ast::Seeker::Verb _foreach_comparisonLevel(
    TiObject *self, Core::Ast::Node const *comparison, Core::Ast::Node *data,
    Core::Ast::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_comparisonOnScope,
    Core::Ast::Seeker::Verb, (
      Core::Ast::Node const*, Core::Ast::Scope*, Core::Ast::Seeker::ForeachCallback const&, Word
    )
  );
  private: static Core::Ast::Seeker::Verb _foreach_comparisonOnScope(
    TiObject *self, Core::Ast::Node const *comparison, Core::Ast::Scope *scope,
    Core::Ast::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_computeComparison, Bool, (Core::Ast::Node const*, Core::Ast::Node*));
  private: static Bool _foreach_computeComparison(TiObject *self, Core::Ast::Node const *comparison, Core::Ast::Node *target);

  /// @}

}; // class

} // namespace

#endif
