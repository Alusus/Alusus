/**
 * @file Spp/SeekerExtension.h
 * Contains the header of class Spp::SeekerExtension.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
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

  public: struct Overrides
  {
    TiFunctionBase *foreachRef;
    TiFunctionBase *extForeachRef;
    TiFunctionBase *foreach_identifierLevelRef;
    TiFunctionBase *foreach_identifierOnFuncRef;
    TiFunctionBase *foreach_identifierOnDataTypeRef;
    TiFunctionBase *foreach_linkOperatorRoutingRef;
    TiFunctionBase *foreach_paramPassRef;
    TiFunctionBase *foreach_paramPassRoutingRef;
    TiFunctionBase *foreach_paramPassOnTemplateRef;
    TiFunctionBase *foreach_thisTypeRefRef;
    TiFunctionBase *foreach_comparisonRef;
    TiFunctionBase *foreach_comparisonLevelRef;
    TiFunctionBase *foreach_comparisonOnScopeRef;
    TiFunctionBase *foreach_computeComparisonRef;
  };

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
      &this->astHelper,
      &this->foreach_identifierOnFunc,
      &this->foreach_identifierOnDataType,
      &this->foreach_paramPass,
      &this->foreach_paramPassRouting,
      &this->foreach_paramPassOnTemplate,
      &this->foreach_thisTypeRef,
      &this->foreach_comparison,
      &this->foreach_comparisonLevel,
      &this->foreach_comparisonOnScope,
      &this->foreach_computeComparison
    });
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

  public: static Overrides* extend(Core::Data::Seeker *seeker, SharedPtr<Ast::Helper> const &astHelper);
  public: static void unextend(Core::Data::Seeker *seeker, Overrides *overrides);

  /// @}

  /// @name Seek Functions
  /// @{

  private: static Core::Data::Seeker::Verb _foreach(
    TiFunctionBase *base, TiObject *self, TiObject const *ref, TiObject *target,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  private: static Core::Data::Seeker::Verb _extForeach(
    TiFunctionBase *base, TiObject *self, TiObject const *ref, TiObject *target,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  private: static Core::Data::Seeker::Verb _foreach_identifierLevel(
    TiFunctionBase *base, TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifierOnFunc,
    Core::Data::Seeker::Verb, (
      Data::Ast::Identifier const*, Ast::Function*, Core::Data::Seeker::ForeachCallback const&, Word
    )
  );
  private: static Core::Data::Seeker::Verb _foreach_identifierOnFunc(
    TiObject *self, Data::Ast::Identifier const *identifier, Ast::Function *function,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifierOnDataType,
    Core::Data::Seeker::Verb, (
      Data::Ast::Identifier const*, Ast::DataType*, Core::Data::Seeker::ForeachCallback const&, Word
    )
  );
  private: static Core::Data::Seeker::Verb _foreach_identifierOnDataType(
    TiObject *self, Data::Ast::Identifier const *identifier, Ast::DataType *type,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  private: static Core::Data::Seeker::Verb _foreach_linkOperatorRouting(
    TiFunctionBase *base, TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_paramPass,
    Core::Data::Seeker::Verb, (Data::Ast::ParamPass const*, TiObject*, Core::Data::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Data::Seeker::Verb _foreach_paramPass(
    TiObject *self, Data::Ast::ParamPass const *paramPass, TiObject *data,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_paramPassRouting,
    Core::Data::Seeker::Verb, (
      Data::Ast::ParamPass const*, TiObject *, Core::Data::Seeker::ForeachCallback const&, Word
    )
  );
  private: static Core::Data::Seeker::Verb _foreach_paramPassRouting(
    TiObject *self, Data::Ast::ParamPass const *paramPass, TiObject *data,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_paramPassOnTemplate,
    Core::Data::Seeker::Verb, (TiObject*, Spp::Ast::Template*, Core::Data::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Data::Seeker::Verb _foreach_paramPassOnTemplate(
    TiObject *self, TiObject *param, Spp::Ast::Template *tmplt, Core::Data::Seeker::ForeachCallback const &cb,
    Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_thisTypeRef,
    Core::Data::Seeker::Verb, (TiObject*, Core::Data::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Data::Seeker::Verb _foreach_thisTypeRef(
    TiObject *self, TiObject *data, Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_comparison,
    Core::Data::Seeker::Verb, (TiObject const*, TiObject*, Core::Data::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Data::Seeker::Verb _foreach_comparison(
    TiObject *self, TiObject const *comparison, TiObject *data,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_comparisonLevel,
    Core::Data::Seeker::Verb, (TiObject const*, TiObject*, Core::Data::Seeker::ForeachCallback const&, Word)
  );
  private: static Core::Data::Seeker::Verb _foreach_comparisonLevel(
    TiObject *self, TiObject const *comparison, TiObject *data,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_comparisonOnScope,
    Core::Data::Seeker::Verb, (
      TiObject const*, Core::Data::Ast::Scope*, Core::Data::Seeker::ForeachCallback const&, Word
    )
  );
  private: static Core::Data::Seeker::Verb _foreach_comparisonOnScope(
    TiObject *self, TiObject const *comparison, Core::Data::Ast::Scope *scope,
    Core::Data::Seeker::ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_computeComparison, Bool, (TiObject const*, TiObject*));
  private: static Bool _foreach_computeComparison(TiObject *self, TiObject const *comparison, TiObject *target);

  /// @}

}; // class

} // namespace

#endif
