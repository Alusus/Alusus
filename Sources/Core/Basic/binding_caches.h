/**
 * @file Core/Basic/binding_caches.h
 * Contains the header of interface Core::Basic::binding_caches.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_BINDINGCACHES_H
#define CORE_BASIC_BINDINGCACHES_H

namespace Core { namespace Basic
{

//==============================================================================
// Classes

class BindingCacheBase
{
  //============================================================================
  // Member Variables

  protected: Char const *name;


  //============================================================================
  // Constructors & Destructor

  public: BindingCacheBase(Char const *n) : name(n)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void init(TiObject *owner) = 0;

  public: Char const* getName() const
  {
    return this->name;
  }

}; // class BindingCacheBase


template<class T> class BindingCache : public BindingCacheBase
{
  //============================================================================
  // Member Variables

  protected: BindingMap *bindingMap;
  protected: T* obj;
  protected: Int index;


  //============================================================================
  // Slots

  private: Slot<void, SharedMapBase<TiObject, TiObject>*, ContentChangeOp, Int> bindingChangeSlot = {
    [=](SharedMapBase<TiObject, TiObject> *src, ContentChangeOp op, Int i)->void
    {
      this->onBindingChanged(src, op, i);
    }
  };

  // The binding map can be destroyed before the cache, depending on the order in which the owner's members are
  // destroyed, so we need to stop using it once it's destroyed.
  private: Slot<void, SharedMapBase<TiObject, TiObject>*> bindingDestroySlot = {
    [=](SharedMapBase<TiObject, TiObject> *src)->void
    {
      this->bindingMap = 0;
    }
  };


  //============================================================================
  // Constructor & Destructor

  public: BindingCache(Char const *name) : BindingCacheBase(name), bindingMap(0), obj(0), index(-1)
  {
  }


  //============================================================================
  // Operators

  public: void operator=(SharedPtr<T> const &o)
  {
    this->set(o);
  }

  public: operator SharedPtr<T>() const
  {
    return this->get();
  }

  public: operator T*() const
  {
    return this->obj;
  }

  public: T* operator->() const
  {
    return this->obj;
  }


  //============================================================================
  // Member Functions

  public: virtual void init(TiObject *owner)
  {
    auto bindings = ti_cast<DynamicBinding>(owner);
    if (bindings == 0) {
      throw EXCEPTION(InvalidArgumentException, S("owner"),
                      S("Provided object does not implement DynamicBinding."));
    }
    this->bindingMap = bindings->getBindingMap();
    this->bindingChangeSlot.disconnect();
    this->bindingDestroySlot.disconnect();
    this->bindingMap->changeNotifier.connect(this->bindingChangeSlot);
    this->bindingMap->destroyNotifier.connect(this->bindingDestroySlot);

    this->index = -1;
    auto i = this->bindingMap->findIndex(this->name);
    this->onBindingChanged(this->bindingMap, ContentChangeOp::ADDED, i);
  }

  protected: virtual void onBindingChanged(SharedMapBase<TiObject, TiObject> *src, ContentChangeOp op, Int i)
  {
    if (op == ContentChangeOp::ADDED) {
      if (this->index == -1) {
        this->update();
      } else if (i <= this->index) {
        ++this->index;
      }
    } else if (op == ContentChangeOp::UPDATED) {
      if (i == this->index) {
        this->update();
      }
    } else if (op == ContentChangeOp::REMOVED) {
      if (i == this->index) {
        this->index = -1;
        this->obj = 0;
      } else if (i < this->index) {
        --this->index;
      }
    }
  }

  private: void update()
  {
    this->index = this->bindingMap->findIndex(this->name);
    if (this->index != -1) this->obj = this->bindingMap->get(this->index).template ti_cast_get<T>();
    else this->obj = 0;
  }

  public: void set(SharedPtr<T> const &o)
  {
    if (this->index == -1) {
      if (this->bindingMap == 0) {
        throw EXCEPTION(GenericException, S("Binding cache not initialized yet."));
      }
      this->index = this->bindingMap->set(this->name, o);
    } else {
      this->bindingMap->set(this->index, o);
    }
    this->obj = o.get();
  }

  public: void remove()
  {
    if (this->index == -1) {
      if (this->bindingMap == 0) {
        throw EXCEPTION(GenericException, S("Binding cache not initialized yet."));
      }
      this->bindingMap->remove(this->name);
    } else {
      this->bindingMap->remove(this->index);
      this->index = -1;
    }
  }

  public: SharedPtr<T> get() const
  {
    return getSharedPtr(this->obj);
  }

}; // class BindingCache


template<class RT, class ...ARGS> class FunctionBindingCache : public BindingCache<TiCallee>
{
  //============================================================================
  // Member Variables

  /// The function currently bound to this cache's key.
  private: TiFunction<RT(ARGS...)> *callee;

  /**
   * @brief The override that holds this cache's function, if any.
   * This is either an override created by this cache, or an existing override
   * of the same function found in the callee chain, as is the case when the
   * owner inherits its bindings from another object.
   */
  private: SharedPtr<TiFunctionOverride<RT(ARGS...)>> ownOverride;

  /**
   * @brief Whether ownOverride was created by this cache.
   * Only an override created by this cache gets replaced by set() or removed
   * by reset(); an override found in the callee chain belongs to whichever
   * cache created it.
   */
  private: Bool ownOverrideCreated;

  /// The function overridden by ownOverride, if any.
  private: TiFunction<RT(ARGS...)> *superCallee;


  //============================================================================
  // Constructor & Destructor

  public: FunctionBindingCache(Char const *name) :
    BindingCache<TiCallee>(name), callee(0), ownOverrideCreated(false), superCallee(0)
  {
  }

  public: ~FunctionBindingCache()
  {
    this->reset();
  }


  //============================================================================
  // Operators

  public: void operator=(RT (*fn)(ARGS...))
  {
    this->set(fn);
  }

  public: RT operator()(ARGS... args)
  {
    if (this->callee != 0) return this->callee->fn(args...);
    else if (this->obj == 0) throw EXCEPTION(GenericException, S("Function is not set."));
    else throw EXCEPTION(GenericException, S("Function signature mismatch."));
  }


  //============================================================================
  // Member Functions

  public: using BindingCache<TiCallee>::init;

  /**
   * @brief Initialize the cache and bind the given function as an override at its key.
   * If the callee chain at the key already has an override of the same
   * function, as is the case when the owner inherits its bindings from an
   * object that has the same override, then that override is used instead of
   * creating a new one.
   * @note When an owner inherits from another object, it must inherit the
   *       bindings before the interfaces, otherwise the inherited override
   *       won't be found and a second override of the same function gets
   *       created, causing callSuper to call the same function recursively.
   */
  public: void init(TiObject *owner, RT (*fn)(ARGS...))
  {
    VALIDATE_NOT_NULL(fn);
    BindingCache<TiCallee>::init(owner);
    auto existing = this->findOverride(fn);
    if (existing != 0) {
      this->ownOverride = getSharedPtr(existing);
      this->ownOverrideCreated = false;
      this->updateSuperCallee();
    } else {
      this->set(fn);
    }
  }

  protected: virtual void onBindingChanged(SharedMapBase<TiObject, TiObject> *src, ContentChangeOp op, Int i)
  {
    BindingCache<TiCallee>::onBindingChanged(src, op, i);
    this->callee = ti_cast<TiFunction<RT(ARGS...)>>(this->obj);
    this->updateSuperCallee();
  }

  private: void updateSuperCallee()
  {
    this->superCallee = this->ownOverride == 0 ? 0 : ti_cast<TiFunction<RT(ARGS...)>>(this->ownOverride->getSuper());
  }

  /// Find the override of the given function in the callee chain at this cache's key.
  private: TiFunctionOverride<RT(ARGS...)>* findOverride(RT (*fn)(ARGS...)) const
  {
    for (TiCallee *tifn = this->obj; tifn != 0; tifn = tifn->getSuper()) {
      auto functionOverride = ti_cast<TiFunctionOverride<RT(ARGS...)>>(tifn);
      if (functionOverride == 0) continue;
      auto target = functionOverride->fn.template target<RT (*)(ARGS...)>();
      if (target != 0 && *target == fn) return functionOverride;
    }
    return 0;
  }

  /**
   * @brief Set the given function as an override of the function at this cache's key.
   * If an override was previously created by this cache, it gets replaced by
   * the new one, keeping its position in the callee chain.
   */
  public: SharedPtr<TiFunctionOverride<RT(ARGS...)>> set(std::function<RT(ARGS...)> fn)
  {
    VALIDATE_NOT_NULL(fn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, S("Binding cache not initialized yet."));
    }
    if (
      this->ownOverride != 0 && this->ownOverrideCreated &&
      this->bindingMap->hasCallee(this->name, this->ownOverride.get())
    ) {
      this->ownOverride = this->bindingMap->updateFunction(this->name, this->ownOverride.get(), fn);
    } else {
      this->ownOverride = this->bindingMap->setFunction(this->name, fn);
    }
    this->ownOverrideCreated = true;
    this->updateSuperCallee();
    return this->ownOverride;
  }

  public: SharedPtr<TiFunctionOverride<RT(ARGS...)>> set(RT (*fn)(ARGS...))
  {
    VALIDATE_NOT_NULL(fn);
    return this->set(std::function<RT(ARGS...)>(fn));
  }

  /// Remove the override created by this cache, if any, from the binding map.
  public: void reset()
  {
    if (this->ownOverride == 0) return;
    if (
      this->ownOverrideCreated && this->bindingMap != 0 &&
      this->bindingMap->hasCallee(this->name, this->ownOverride.get())
    ) {
      this->bindingMap->resetCallee(this->name, this->ownOverride.get());
    }
    this->ownOverride.reset();
    this->ownOverrideCreated = false;
    this->superCallee = 0;
  }

  /// Call the function overridden by the override bound to this cache.
  public: RT callSuper(ARGS... args)
  {
    if (this->superCallee != 0) return this->superCallee->fn(args...);
    else if (this->ownOverride == 0) throw EXCEPTION(GenericException, S("No override is bound to this cache."));
    else throw EXCEPTION(GenericException, S("Super function is not set or has a mismatching signature."));
  }

}; // class FunctionBindingCache


template<class RT, class ...ARGS> class MethodBindingCache : public BindingCache<TiCallee>
{
  //============================================================================
  // Types

  public: typedef typename TiMethod<RT(ARGS...)>::Fn Fn;


  //============================================================================
  // Member Variables

  protected: TiObject *self;

  /// The function currently bound to this cache's key.
  private: Fn callee;

  /**
   * @brief The override that holds this cache's function, if any.
   * This is either an override created by this cache, or an existing override
   * of the same function found in the callee chain, as is the case when the
   * owner inherits its bindings from another object.
   */
  private: SharedPtr<TiMethodOverride<RT(ARGS...)>> ownOverride;

  /**
   * @brief Whether ownOverride was created by this cache.
   * Only an override created by this cache gets replaced by set() or removed
   * by reset(); an override found in the callee chain belongs to whichever
   * cache created it.
   */
  private: Bool ownOverrideCreated;

  /// The function overridden by ownOverride, if any.
  private: Fn superCallee;


  //============================================================================
  // Constructor & Destructor

  public: MethodBindingCache(Char const *name) :
    BindingCache<TiCallee>(name), self(0), callee(0), ownOverrideCreated(false), superCallee(0)
  {
  }

  public: ~MethodBindingCache()
  {
    this->reset();
  }


  //============================================================================
  // Operators

  public: void operator=(Fn fn)
  {
    this->set(fn);
  }

  public: RT operator()(ARGS... args)
  {
    if (this->callee != 0) return this->callee(this->self, args...);
    else if (this->obj == 0) throw EXCEPTION(GenericException, S("Function is not set."));
    else throw EXCEPTION(GenericException, S("Function signature mismatch."));
  }


  //============================================================================
  // Member Functions

  public: virtual void init(TiObject *owner)
  {
    if (owner == 0) {
      throw EXCEPTION(InvalidArgumentException, S("owner"), S("Argument is null."));
    }
    this->self = owner;
    BindingCache<TiCallee>::init(owner);
  }

  /**
   * @brief Initialize the cache and bind the given function as an override at its key.
   * If the callee chain at the key already has an override of the same
   * function, as is the case when the owner inherits its bindings from an
   * object that has the same override, then that override is used instead of
   * creating a new one.
   * @note When an owner inherits from another object, it must inherit the
   *       bindings before the interfaces, otherwise the inherited override
   *       won't be found and a second override of the same function gets
   *       created, causing callSuper to call the same function recursively.
   */
  public: void init(TiObject *owner, Fn fn)
  {
    VALIDATE_NOT_NULL(fn);
    this->init(owner);
    auto existing = this->findOverride(fn);
    if (existing != 0) {
      this->ownOverride = getSharedPtr(existing);
      this->ownOverrideCreated = false;
      this->updateSuperCallee();
    } else {
      this->set(fn);
    }
  }

  protected: virtual void onBindingChanged(SharedMapBase<TiObject, TiObject> *src, ContentChangeOp op, Int i)
  {
    BindingCache<TiCallee>::onBindingChanged(src, op, i);
    auto method = ti_cast<TiMethod<RT(ARGS...)>>(this->obj);
    this->callee = method == 0 ? 0 : method->fn;
    this->updateSuperCallee();
  }

  private: void updateSuperCallee()
  {
    auto super = this->ownOverride == 0 ? 0 : ti_cast<TiMethod<RT(ARGS...)>>(this->ownOverride->getSuper());
    this->superCallee = super == 0 ? 0 : super->fn;
  }

  /// Find the override of the given function in the callee chain at this cache's key.
  private: TiMethodOverride<RT(ARGS...)>* findOverride(Fn fn) const
  {
    for (TiCallee *tifn = this->obj; tifn != 0; tifn = tifn->getSuper()) {
      auto methodOverride = ti_cast<TiMethodOverride<RT(ARGS...)>>(tifn);
      if (methodOverride != 0 && methodOverride->fn == fn) return methodOverride;
    }
    return 0;
  }

  /**
   * @brief Set the given function as an override of the function at this cache's key.
   * If an override was previously created by this cache, it gets replaced by
   * the new one, keeping its position in the callee chain.
   */
  public: SharedPtr<TiMethodOverride<RT(ARGS...)>> set(Fn fn)
  {
    VALIDATE_NOT_NULL(fn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, S("Binding cache not initialized yet."));
    }
    if (
      this->ownOverride != 0 && this->ownOverrideCreated &&
      this->bindingMap->hasCallee(this->name, this->ownOverride.get())
    ) {
      this->ownOverride = this->bindingMap->updateMethod(this->name, this->ownOverride.get(), fn);
    } else {
      this->ownOverride = this->bindingMap->setMethod(this->name, fn);
    }
    this->ownOverrideCreated = true;
    this->updateSuperCallee();
    return this->ownOverride;
  }

  /// Remove the override created by this cache, if any, from the binding map.
  public: void reset()
  {
    if (this->ownOverride == 0) return;
    if (
      this->ownOverrideCreated && this->bindingMap != 0 &&
      this->bindingMap->hasCallee(this->name, this->ownOverride.get())
    ) {
      this->bindingMap->resetCallee(this->name, this->ownOverride.get());
    }
    this->ownOverride.reset();
    this->ownOverrideCreated = false;
    this->superCallee = 0;
  }

  /// Call the function overridden by the override bound to this cache.
  public: RT callSuper(ARGS... args)
  {
    if (this->superCallee != 0) return this->superCallee(this->self, args...);
    else if (this->ownOverride == 0) throw EXCEPTION(GenericException, S("No override is bound to this cache."));
    else throw EXCEPTION(GenericException, S("Super function is not set or has a mismatching signature."));
  }

}; // class MethodBindingCache


//==============================================================================
// Functions

inline void initBindingCaches(TiObject *self, std::initializer_list<BindingCacheBase*> const &bindingCaches)
{
  for (auto bindingCache : bindingCaches) {
    bindingCache->init(self);
  }
}

} } // namespace


//==============================================================================
// Macros

#define BINDING_CACHE(name, type) BindingCache<type> name = { S(#name) }

#define FUNCTION_BINDING_CACHE_ARGS(name, retType, args) \
  FunctionBindingCache<retType, COMMA_EXPAND_ARGS args> name = { S(#name) }
#define FUNCTION_BINDING_CACHE_NOARGS(name, retType) \
  FunctionBindingCache<retType> name = { #name }
#define FUNCTION_BINDING_CACHE(name, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, _, _, _, \
               FUNCTION_BINDING_CACHE_ARGS, \
               FUNCTION_BINDING_CACHE_NOARGS)(name, __VA_ARGS__)

#define METHOD_BINDING_CACHE_ARGS(name, retType, args) \
  MethodBindingCache<retType, COMMA_EXPAND_ARGS args> name = { S(#name) }
#define METHOD_BINDING_CACHE_NOARGS(name, retType) \
  MethodBindingCache<retType> name = { S(#name) }
#define METHOD_BINDING_CACHE(name, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, _, _, _, \
               METHOD_BINDING_CACHE_ARGS, \
               METHOD_BINDING_CACHE_NOARGS)(name, __VA_ARGS__)

#endif
