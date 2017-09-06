/**
 * @file Core/Data/Seeker.h
 * Contains the header of class Core::Data::Seeker.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_SEEKER_H
#define CORE_DATA_SEEKER_H

namespace Core { namespace Data
{

class Seeker : public TiObject, public virtual DynamicBindings, public virtual DynamicInterfaces
{
  //============================================================================
  // Type Info

  TYPE_INFO(Seeker, TiObject, "Core.Data", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(DynamicBindings, DynamicInterfaces),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Types

  public: ti_s_enum(SeekVerb, Integer, "Core.Data", "Core", "alusus.net",
                    MOVE, STOP, PERFORM_AND_MOVE, PERFORM_AND_STOP);

  public: typedef std::function<SeekVerb(TiObject *&obj, Notice *notice)> SeekSetCallback;
  public: typedef std::function<SeekVerb(TiObject *obj, Notice *notice)> SeekRemoveCallback;
  public: typedef std::function<SeekVerb(TiObject *obj, Notice *notice)> SeekForeachCallback;


  //============================================================================
  // Types

  private: struct PrecomputedRecord
  {
    TiObject const *ref;
    TiObject *target;
    TiObject *result;
    PrecomputedRecord(TiObject const *ref, TiObject *target, TiObject *result)
      : ref(ref), target(target), result(result)
    {
    }
  };


  //============================================================================
  // Member Variables

  private: std::vector<PrecomputedRecord> precomputedRecords;


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACES(interfaceList);


  //============================================================================
  // Constructors

  Seeker()
  {
    this->initBindingCaches();
    this->initBindings();
  }

  Seeker(Seeker *parent)
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

  public: void doSet(TiObject const *ref, TiObject *target, SeekSetCallback const &cb)
  {
    this->set(ref, target, cb);
  }

  public: void doRemove(TiObject const *ref, TiObject *target, SeekRemoveCallback const &cb)
  {
    this->remove(ref, target, cb);
  }

  public: void doForeach(TiObject const *ref, TiObject *target, SeekForeachCallback const &cb)
  {
    for (PrecomputedRecord &record : this->precomputedRecords) {
      if (record.ref == ref && record.target == target) {
        cb(record.result, 0);
        return;
      }
    }
    this->foreach(ref, target, cb);
  }

  public: void doContinueForeach(
    TiObject const *continueRef, TiObject *continueResult, TiObject const *ref, TiObject *target,
    SeekForeachCallback const &cb
  ) {
    this->precomputedRecords.push_back(PrecomputedRecord(continueRef, target, continueResult));
    this->doForeach(ref, target, cb);
    this->precomputedRecords.pop_back();
  }

  public: Bool trySet(TiObject const *ref, TiObject *target, TiObject *val);

  public: void doSet(TiObject const *ref, TiObject *target, TiObject *val)
  {
    if (!this->trySet(ref, target, val)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool tryRemove(TiObject const *ref, TiObject *target);

  public: void doRemove(TiObject const *ref, TiObject *target)
  {
    if (!this->tryRemove(ref, target)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool tryGet(TiObject const *ref, TiObject *target, TiObject *&retVal);

  public: TiObject* tryGet(TiObject const *ref, TiObject *target)
  {
    TiObject *result = 0;
    this->tryGet(ref, target, result);
    return result;
  }

  public: TiObject* doGet(TiObject const *ref, TiObject *target)
  {
    TiObject *retVal = this->tryGet(ref, target);
    if (retVal == 0) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
    return retVal;
  }

  public: static Bool isPerform(SeekVerb verb)
  {
    return verb == SeekVerb::PERFORM_AND_STOP || verb == SeekVerb::PERFORM_AND_MOVE;
  }

  public: static Bool isMove(SeekVerb verb)
  {
    return verb == SeekVerb::MOVE || verb == SeekVerb::PERFORM_AND_MOVE;
  }

  /// @}

  /// @name Main Seek Functions
  /// @{

  public: METHOD_BINDING_CACHE(set, void, (TiObject const*, TiObject*, SeekSetCallback const&));
  public: METHOD_BINDING_CACHE(remove, void, (TiObject const*, TiObject*, SeekRemoveCallback const&));
  public: METHOD_BINDING_CACHE(foreach, void, (TiObject const*, TiObject*, SeekForeachCallback const&));

  private: static void _set(TiObject *self, TiObject const *ref, TiObject *target, SeekSetCallback const &cb);
  private: static void _remove(TiObject *self, TiObject const *ref, TiObject *target, SeekRemoveCallback const &cb);
  private: static void _foreach(TiObject *self, TiObject const *ref, TiObject *target, SeekForeachCallback const &cb);

  /// @}

  /// @name Identifier Seek Functions
  /// @{

  public: METHOD_BINDING_CACHE(setByIdentifier,
    void, (Data::Ast::Identifier const*, TiObject*, SeekSetCallback const&)
  );
  public: METHOD_BINDING_CACHE(setByIdentifier_sharedRepository,
    void, (Data::Ast::Identifier const*, Data::SharedRepository*, SeekSetCallback const&)
  );
  public: METHOD_BINDING_CACHE(setByIdentifier_ast,
    void, (Data::Ast::Identifier const*, TiObject*, SeekSetCallback const&)
  );

  private: static void _setByIdentifier(TiObject *self, Data::Ast::Identifier const *identifier,
                                        TiObject *data, SeekSetCallback const &cb);
  private: static void _setByIdentifier_sharedRepository(TiObject *self, Data::Ast::Identifier const *identifier,
                                                         Data::SharedRepository *repo, SeekSetCallback const &cb);
  private: static void _setByIdentifier_ast(TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data,
                                            SeekSetCallback const &cb);

  public: METHOD_BINDING_CACHE(removeByIdentifier,
    void, (Data::Ast::Identifier const*, TiObject*, SeekRemoveCallback const&)
  );
  public: METHOD_BINDING_CACHE(removeByIdentifier_sharedRepository,
    void, (Data::Ast::Identifier const*, Data::SharedRepository*, SeekRemoveCallback const&)
  );
  public: METHOD_BINDING_CACHE(removeByIdentifier_ast,
    void, (Data::Ast::Identifier const*, TiObject*, SeekRemoveCallback const&)
  );

  private: static void _removeByIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, SeekRemoveCallback const &cb
  );
  private: static void _removeByIdentifier_sharedRepository(
    TiObject *self, Data::Ast::Identifier const *identifier, Data::SharedRepository *repo, SeekRemoveCallback const &cb
  );
  private: static void _removeByIdentifier_ast(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, SeekRemoveCallback const &cb
  );

  public: METHOD_BINDING_CACHE(foreachByIdentifier,
    void, (Data::Ast::Identifier const*, TiObject*, SeekForeachCallback const&)
  );
  public: METHOD_BINDING_CACHE(foreachByIdentifier_sharedRepository,
    void, (Data::Ast::Identifier const*, Data::SharedRepository*, SeekForeachCallback const&)
  );
  public: METHOD_BINDING_CACHE(foreachByIdentifier_ast,
    void, (Data::Ast::Identifier const*, TiObject*, SeekForeachCallback const&)
  );

  private: static void _foreachByIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, SeekForeachCallback const &cb
  );
  private: static void _foreachByIdentifier_sharedRepository(
    TiObject *self, Data::Ast::Identifier const *identifier, Data::SharedRepository *repo, SeekForeachCallback const &cb
  );
  private: static void _foreachByIdentifier_ast(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, SeekForeachCallback const &cb
  );

  /// @}

  /// @name LinkOperator Seek Functions
  /// @{

  public: METHOD_BINDING_CACHE(setByLinkOperator,
    void, (Data::Ast::LinkOperator const*, TiObject*, SeekSetCallback const&)
  );
  public: METHOD_BINDING_CACHE(setByLinkOperator_routing,
    SeekVerb, (Data::Ast::LinkOperator const*, TiObject*, SeekSetCallback const&)
  );
  public: METHOD_BINDING_CACHE(setByLinkOperator_scopeDotIdentifier,
    SeekVerb, (Data::Ast::Identifier const*, Data::Ast::Scope*, SeekSetCallback const&)
  );
  public: METHOD_BINDING_CACHE(setByLinkOperator_mapDotIdentifier,
    SeekVerb, (Data::Ast::Identifier const*, Data::MapContainer*, SeekSetCallback const&)
  );

  private: static void _setByLinkOperator(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, SeekSetCallback const &cb
  );
  private: static SeekVerb _setByLinkOperator_routing(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, SeekSetCallback const &cb
  );
  private: static SeekVerb _setByLinkOperator_scopeDotIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, Data::Ast::Scope *scope, SeekSetCallback const &cb
  );
  private: static SeekVerb _setByLinkOperator_mapDotIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, Data::MapContainer *map, SeekSetCallback const &cb
  );

  public: METHOD_BINDING_CACHE(removeByLinkOperator,
    void, (Data::Ast::LinkOperator const*, TiObject*, SeekRemoveCallback const&)
  );
  public: METHOD_BINDING_CACHE(removeByLinkOperator_routing,
    SeekVerb, (Data::Ast::LinkOperator const*, TiObject*, SeekRemoveCallback const&)
  );
  public: METHOD_BINDING_CACHE(removeByLinkOperator_scopeDotIdentifier,
    SeekVerb, (Data::Ast::Identifier const*, Data::Ast::Scope*, SeekRemoveCallback const&)
  );
  public: METHOD_BINDING_CACHE(removeByLinkOperator_mapDotIdentifier,
    SeekVerb, (Data::Ast::Identifier const*, Data::MapContainer*, SeekRemoveCallback const&)
  );

  private: static void _removeByLinkOperator(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, SeekRemoveCallback const &cb
  );
  private: static SeekVerb _removeByLinkOperator_routing(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, SeekRemoveCallback const &cb
  );
  private: static SeekVerb _removeByLinkOperator_scopeDotIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, Data::Ast::Scope *scope, SeekRemoveCallback const &cb
  );
  private: static SeekVerb _removeByLinkOperator_mapDotIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, Data::MapContainer *map, SeekRemoveCallback const &cb
  );

  public: METHOD_BINDING_CACHE(foreachByLinkOperator,
    void, (Data::Ast::LinkOperator const*, TiObject*, SeekForeachCallback const&)
  );
  public: METHOD_BINDING_CACHE(foreachByLinkOperator_routing,
    SeekVerb, (Data::Ast::LinkOperator const*, TiObject*, SeekForeachCallback const&)
  );
  public: METHOD_BINDING_CACHE(foreachByLinkOperator_scopeDotIdentifier,
    SeekVerb, (Data::Ast::Identifier*, Data::Ast::Scope*, SeekForeachCallback const&)
  );
  public: METHOD_BINDING_CACHE(foreachByLinkOperator_mapDotIdentifier,
    SeekVerb, (Data::Ast::Identifier const*, Data::MapContainer*, SeekForeachCallback const&)
  );

  private: static void _foreachByLinkOperator(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, SeekForeachCallback const &cb
  );
  private: static SeekVerb _foreachByLinkOperator_routing(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, SeekForeachCallback const &cb
  );
  private: static SeekVerb _foreachByLinkOperator_scopeDotIdentifier(
    TiObject *self, Data::Ast::Identifier *identifier, Data::Ast::Scope *scope, SeekForeachCallback const &cb
  );
  private: static SeekVerb _foreachByLinkOperator_mapDotIdentifier(
    TiObject *_self, Data::Ast::Identifier const *identifier, Data::MapContainer *map, SeekForeachCallback const &cb
  );

  /// @}

}; // class

} } // namespace

#endif
