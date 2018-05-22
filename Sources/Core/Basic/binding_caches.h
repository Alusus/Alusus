/**
 * @file Core/Basic/binding_caches.h
 * Contains the header of interface Core::Basic::binding_caches.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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

  private: Slot<void, BindingMapBase*, ContentChangeOp, Int> bindingChangeSlot = {
    this, &BindingCache::onBindingChanged
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

  public: operator SharedPtr<T> const&() const
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
      throw EXCEPTION(InvalidArgumentException, STR("owner"),
                      STR("Provided object does not implement DynamicBinding."));
    }
    this->bindingMap = bindings->getBindingMap();
    this->bindingChangeSlot.disconnect();
    this->bindingMap->changeNotifier.connect(this->bindingChangeSlot);
    this->update();
  }

  private: void onBindingChanged(BindingMapBase *src, ContentChangeOp op, Int i)
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

  public: SharedPtr<T> const& get() const
  {
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
    }
    if (this->index == -1) return SharedPtr<T>::null;
    else return this->bindingMap->get(this->index);
  }

  public: void set(SharedPtr<T> const &o)
  {
    if (this->index == -1) {
      if (this->bindingMap == 0) {
        throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
      }
      this->index = this->bindingMap->set(this->name, o);
    } else {
      this->bindingMap->set(this->index, o);
    }
    this->obj = o.get();
  }

}; // class BindingCache


template<class RT, class ...ARGS> class FunctionBindingCache : public BindingCache<TiFunctionBase>
{
  //============================================================================
  // Types

  public: struct FunctionCalleeInstance
  {
    TiObject *callee;
    FunctionCalleeInstance(TiObject *c) : callee(c) {}
    RT operator()(ARGS... args)
    {
      return Core::Basic::call<RT, ARGS...>(this->callee, args...);
    }
  };


  //============================================================================
  // Constructor & Destructor

  public: FunctionBindingCache(Char const *name) : BindingCache<TiFunctionBase>(name)
  {
  }


  //============================================================================
  // Operators

  public: void operator=(RT (*fn)(ARGS...))
  {
    this->set(fn);
  }

  public: RT operator()(ARGS... args)
  {
    return Core::Basic::call<RT, ARGS...>(this->obj, args...);
  }


  //============================================================================
  // Member Functions

  public: SharedPtr<TiFunctionOverride<RT(ARGS...)>> set(RT (*fn)(ARGS...))
  {
    VALIDATE_NOT_NULL(fn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
    }
    return this->bindingMap->setFunction(this->name, fn);
  }

  public: SharedPtr<TiFunctionOverride<RT(TiFunctionBase*, ARGS...)>> set(RT (*fn)(TiFunctionBase*, ARGS...))
  {
    VALIDATE_NOT_NULL(fn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
    }
    return this->bindingMap->setFunction(this->name, fn);
  }

  public: SharedPtr<TiFunctionOverride<RT(ARGS...)>> set(std::function<RT(ARGS...)> fn)
  {
    VALIDATE_NOT_NULL(fn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
    }
    return this->bindingMap->setFunction(this->name, fn);
  }

  public: SharedPtr<TiFunctionOverride<RT(TiFunctionBase*, ARGS...)>> set(
    std::function<RT(TiFunctionBase*, ARGS...)> fn
  ) {
    VALIDATE_NOT_NULL(fn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
    }
    return this->bindingMap->setFunction(this->name, fn);
  }

  public: SharedPtr<TiFunctionOverride<RT(ARGS...)>> update(TiFunctionBase *currentTifn, RT (*fn)(ARGS...))
  {
    VALIDATE_NOT_NULL(fn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
    }
    return this->bindingMap->updateFunction(this->name, currentTifn, fn);
  }

  public: void updateChain(TiFunctionBase *currentTifn, SharedPtr<TiFunctionBase> const &newTifn)
  {
    VALIDATE_NOT_NULL(newTifn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
    }
    this->bindingMap->updateFunctionChain(this->name, currentTifn, newTifn);
  }

  public: void reset(TiFunctionBase *currentTifn)
  {
    VALIDATE_NOT_NULL(currentTifn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
    }
    this->bindingMap->resetFunction(this->name, currentTifn);
  }

  public: void resetChain(TiFunctionBase *currentTifn)
  {
    VALIDATE_NOT_NULL(currentTifn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
    }
    this->bindingMap->resetFunctionChain(this->name, currentTifn);
  }

  public: FunctionCalleeInstance useCallee(TiFunctionBase *c)
  {
    return FunctionCalleeInstance(c);
  }

}; // class FunctionBindingCache


template<class RT, class ...ARGS> class MethodBindingCache : public FunctionBindingCache<RT, ARGS...>
{
  //============================================================================
  // Member Variables

  protected: TiObject *self;


  //============================================================================
  // Types

  public: struct MethodCalleeInstance
  {
    TiObject *callee, *self;
    MethodCalleeInstance(TiObject *c, TiObject *s) : callee(c), self(s) {}
    RT operator()(ARGS... args)
    {
      return Core::Basic::call<RT, TiObject*, ARGS...>(this->callee, this->self, args...);
    }
  };


  //============================================================================
  // Constructor & Destructor

  public: MethodBindingCache(Char const *name) : FunctionBindingCache<RT, ARGS...>(name), self(0)
  {
  }


  //============================================================================
  // Operators

  public: void operator=(RT (*fn)(TiObject*, ARGS...))
  {
    this->set(fn);
  }

  public: template<class C> void operator=(RT (C::*fn)(ARGS...))
  {
    this->set(
      [=](TiObject *self, ARGS... args)->RT {
        PREPARE_SELF(c, C);
        return (c->*fn)(args...);
      }
    );
  }

  public: RT operator()(ARGS... args)
  {
    return Core::Basic::call<RT, TiObject*, ARGS...>(this->obj, this->self, args...);
  }


  //============================================================================
  // Member Functions

  public: virtual void init(TiObject *owner)
  {
    if (owner == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("owner"), STR("Argument is null."));
    }
    this->self = owner;
    FunctionBindingCache<RT, ARGS...>::init(owner);
  }

  public: SharedPtr<TiFunctionOverride<RT(TiObject*,ARGS...)>> set(RT (*fn)(TiObject*, ARGS...))
  {
    VALIDATE_NOT_NULL(fn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
    }
    return this->bindingMap->setFunction(this->name, fn);
  }

  public: SharedPtr<TiFunctionOverride<RT(TiFunctionBase*, TiObject*, ARGS...)>> set(
    RT (*fn)(TiFunctionBase*, TiObject*, ARGS...)
  ) {
    VALIDATE_NOT_NULL(fn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
    }
    return this->bindingMap->setFunction(this->name, fn);
  }

  public: SharedPtr<TiFunctionOverride<RT(TiObject*, ARGS...)>> set(std::function<RT(TiObject*, ARGS...)> fn)
  {
    VALIDATE_NOT_NULL(fn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
    }
    return this->bindingMap->setFunction(this->name, fn);
  }

  public: SharedPtr<TiFunctionOverride<RT(TiFunctionBase*, TiObject*, ARGS...)>> set(
    std::function<RT(TiFunctionBase*, TiObject*, ARGS...)> fn
  ) {
    VALIDATE_NOT_NULL(fn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
    }
    return this->bindingMap->setFunction(this->name, fn);
  }

  public: SharedPtr<TiFunctionOverride<RT(TiObject*, ARGS...)>> update(TiFunctionBase *currentTifn,
                                                                       RT (*fn)(TiObject*, ARGS...))
  {
    VALIDATE_NOT_NULL(fn);
    if (this->bindingMap == 0) {
      throw EXCEPTION(GenericException, STR("Bindign cache not initialized yet."));
    }
    return this->bindingMap->updateFunction(this->name, currentTifn, fn);
  }

  public: MethodCalleeInstance useCallee(TiFunctionBase *c)
  {
    return MethodCalleeInstance(c, this->self);
  }

}; // class FunctionBindingCache


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

#define BINDING_CACHE(name, type) BindingCache<type> name = { #name }

#define FUNCTION_BINDING_CACHE_ARGS(name, retType, args) \
  FunctionBindingCache<retType, COMMA_EXPAND_ARGS args> name = { #name }
#define FUNCTION_BINDING_CACHE_NOARGS(name, retType) \
  FunctionBindingCache<retType> name = { #name }
#define FUNCTION_BINDING_CACHE(name, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, _, _, _, \
               FUNCTION_BINDING_CACHE_ARGS, \
               FUNCTION_BINDING_CACHE_NOARGS)(name, __VA_ARGS__)

#define METHOD_BINDING_CACHE_ARGS(name, retType, args) \
  MethodBindingCache<retType, COMMA_EXPAND_ARGS args> name = { #name }
#define METHOD_BINDING_CACHE_NOARGS(name, retType) \
  MethodBindingCache<retType> name = { #name }
#define METHOD_BINDING_CACHE(name, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, _, _, _, \
               METHOD_BINDING_CACHE_ARGS, \
               METHOD_BINDING_CACHE_NOARGS)(name, __VA_ARGS__)

#endif
