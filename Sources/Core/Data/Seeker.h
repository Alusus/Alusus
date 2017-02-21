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

class Seeker : public TiObject, public virtual DynamicBindings
{
  //============================================================================
  // Type Info

  TYPE_INFO(Seeker, TiObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(TiObject, DynamicBindings);


  //============================================================================
  // Types

  public: ti_s_enum(SeekVerb, Integer, "Core.Data", "Core", "alusus.net",
                    MOVE, STOP, PERFORM_AND_MOVE, PERFORM_AND_STOP);

  public: typedef std::function<SeekVerb(TiObject *&obj)> SeekSetCallback;
  public: typedef std::function<SeekVerb(TiObject *obj)> SeekRemoveCallback;
  public: typedef std::function<SeekVerb(TiObject *obj)> SeekForeachCallback;


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_RT_BINDING;


  //============================================================================
  // Constructors

  Seeker()
  {
    this->initialize();
  }

  Seeker(Seeker *parent)
  {
    this->inherit(parent);
  }


  //============================================================================
  // Member Functions

  /// @name Initialization
  /// @{

  private: void initialize();

  /// @}

  /// @name Helper Functions
  /// @{

  public: void doSet(TiObject const *ref, TiObject *target, SeekSetCallback callback)
  {
    this->call<void>(this->set, ref, target, callback);
  }

  public: void doRemove(TiObject const *ref, TiObject *target, SeekRemoveCallback callback)
  {
    this->call<void>(this->remove, ref, target, callback);
  }

  public: void doForeach(TiObject const *ref, TiObject *target, SeekForeachCallback callback)
  {
    this->call<void>(this->foreach, ref, target, callback);
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

  public: BINDING_INDEX_CACHE(set, &this->bindingMap);
  public: BINDING_INDEX_CACHE(remove, &this->bindingMap);
  public: BINDING_INDEX_CACHE(foreach, &this->bindingMap);

  private: static void _set(Bindings *_self, TiObject const *ref, TiObject *target, SeekSetCallback cb);
  private: static void _remove(Bindings *_self, TiObject const *ref, TiObject *target, SeekRemoveCallback cb);
  private: static void _foreach(Bindings *_self, TiObject const *ref, TiObject *target, SeekForeachCallback cb);

  /// @}

  /// @name Identifier Seek Functions
  /// @{

  public: BINDING_INDEX_CACHE(setByIdentifier, &this->bindingMap);
  public: BINDING_INDEX_CACHE(setByIdentifier_sharedRepository, &this->bindingMap);
  public: BINDING_INDEX_CACHE(setByIdentifier_ast, &this->bindingMap);

  private: static void _setByIdentifier(Bindings *_self, Data::Ast::Identifier const *identifier,
                                        TiObject *data, SeekSetCallback cb);
  private: static void _setByIdentifier_sharedRepository(Bindings *_self, Data::Ast::Identifier const *identifier,
                                                         Data::SharedRepository *repo, SeekSetCallback cb);
  private: static void _setByIdentifier_ast(Bindings *_self, Data::Ast::Identifier const *identifier, TiObject *data,
                                            SeekSetCallback cb);

  public: BINDING_INDEX_CACHE(removeByIdentifier, &this->bindingMap);
  public: BINDING_INDEX_CACHE(removeByIdentifier_sharedRepository, &this->bindingMap);
  public: BINDING_INDEX_CACHE(removeByIdentifier_ast, &this->bindingMap);

  private: static void _removeByIdentifier(Bindings *_self, Data::Ast::Identifier const *identifier, TiObject *data,
                                           SeekRemoveCallback cb);
  private: static void _removeByIdentifier_sharedRepository(Bindings *_self, Data::Ast::Identifier const *identifier,
                                                            Data::SharedRepository *repo, SeekRemoveCallback cb);
  private: static void _removeByIdentifier_ast(Bindings *_self, Data::Ast::Identifier const *identifier,
                                               TiObject *data, SeekRemoveCallback cb);

  public: BINDING_INDEX_CACHE(foreachByIdentifier, &this->bindingMap);
  public: BINDING_INDEX_CACHE(foreachByIdentifier_sharedRepository, &this->bindingMap);
  public: BINDING_INDEX_CACHE(foreachByIdentifier_ast, &this->bindingMap);

  private: static void _foreachByIdentifier(Bindings *_self, Data::Ast::Identifier const *identifier, TiObject *data,
                                            SeekForeachCallback cb);
  private: static void _foreachByIdentifier_sharedRepository(Bindings *_self, Data::Ast::Identifier const *identifier,
                                                             Data::SharedRepository *repo, SeekForeachCallback cb);
  private: static void _foreachByIdentifier_ast(Bindings *_self, Data::Ast::Identifier const *identifier,
                                                TiObject *data, SeekForeachCallback cb);

  /// @}

  /// @name LinkOperator Seek Functions
  /// @{

  public: BINDING_INDEX_CACHE(setByLinkOperator, &this->bindingMap);
  public: BINDING_INDEX_CACHE(setByLinkOperator_routing, &this->bindingMap);
  public: BINDING_INDEX_CACHE(setByLinkOperator_scopeDotIdentifier, &this->bindingMap);
  public: BINDING_INDEX_CACHE(setByLinkOperator_mapDotIdentifier, &this->bindingMap);

  private: static void _setByLinkOperator(Bindings *_self, Data::Ast::LinkOperator const *link, TiObject *data,
                                          SeekSetCallback cb);
  private: static SeekVerb _setByLinkOperator_routing(Bindings *_self, Data::Ast::LinkOperator const *link,
                                                      TiObject *data, SeekSetCallback cb);
  private: static SeekVerb _setByLinkOperator_scopeDotIdentifier(Bindings *_self,
                                                                 Data::Ast::Identifier const *identifier,
                                                                 Data::Ast::Scope *scope, SeekSetCallback cb);
  private: static SeekVerb _setByLinkOperator_mapDotIdentifier(Bindings *_self,
                                                               Data::Ast::Identifier const *identifier,
                                                               Data::MapContainer *map, SeekSetCallback cb);

  public: BINDING_INDEX_CACHE(removeByLinkOperator, &this->bindingMap);
  public: BINDING_INDEX_CACHE(removeByLinkOperator_routing, &this->bindingMap);
  public: BINDING_INDEX_CACHE(removeByLinkOperator_scopeDotIdentifier, &this->bindingMap);
  public: BINDING_INDEX_CACHE(removeByLinkOperator_mapDotIdentifier, &this->bindingMap);

  private: static void _removeByLinkOperator(Bindings *_self, Data::Ast::LinkOperator const *link, TiObject *data,
                                             SeekRemoveCallback cb);
  private: static SeekVerb _removeByLinkOperator_routing(Bindings *_self, Data::Ast::LinkOperator const *link,
                                                         TiObject *data, SeekRemoveCallback cb);
  private: static SeekVerb _removeByLinkOperator_scopeDotIdentifier(Bindings *_self,
                                                                    Data::Ast::Identifier const *identifier,
                                                                    Data::Ast::Scope *scope, SeekRemoveCallback cb);
  private: static SeekVerb _removeByLinkOperator_mapDotIdentifier(Bindings *_self,
                                                                  Data::Ast::Identifier const *identifier,
                                                                  Data::MapContainer *map, SeekRemoveCallback cb);

  public: BINDING_INDEX_CACHE(foreachByLinkOperator, &this->bindingMap);
  public: BINDING_INDEX_CACHE(foreachByLinkOperator_routing, &this->bindingMap);
  public: BINDING_INDEX_CACHE(foreachByLinkOperator_scopeDotIdentifier, &this->bindingMap);
  public: BINDING_INDEX_CACHE(foreachByLinkOperator_mapDotIdentifier, &this->bindingMap);

  private: static void _foreachByLinkOperator(Bindings *_self, Data::Ast::LinkOperator const *link, TiObject *data,
                                              SeekForeachCallback cb);
  private: static SeekVerb _foreachByLinkOperator_routing(Bindings *_self, Data::Ast::LinkOperator const *link,
                                                          TiObject *data, SeekForeachCallback cb);
  private: static SeekVerb _foreachByLinkOperator_scopeDotIdentifier(Bindings *_self,
                                                                     Data::Ast::Identifier *identifier,
                                                                     Data::Ast::Scope *scope, SeekForeachCallback cb);
  private: static SeekVerb _foreachByLinkOperator_mapDotIdentifier(Bindings *_self,
                                                                   Data::Ast::Identifier const *identifier,
                                                                   Data::MapContainer *map, SeekForeachCallback cb);

  /// @}

}; // class

} } // namespace

#endif
