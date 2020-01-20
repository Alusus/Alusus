/**
 * @file Core/Basic/BindingMap.h
 * Contains the header of class Core::Basic::BindingMap.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_BINDINGMAP_H
#define CORE_BASIC_BINDINGMAP_H

namespace Core::Basic
{

class BindingMap : public SharedMap<TiObject>
{
  //============================================================================
  // Type Info

  TYPE_INFO(BindingMap, SharedMap<TiObject>, "Core.Basic", "Core", "alusus.org");


  //============================================================================
  // Signals & Slots

  private: Slot<void, SharedMapBase<TiObject, TiObject>*, ContentChangeOp, Int> baseChangeSlot = {
    this, &BindingMap::onBaseContentChanged
  };
  private: Slot<void, SharedMapBase<TiObject, TiObject>*> baseDestroySlot = {
    [=](SharedMapBase<TiObject, TiObject>*)->void
    {
      this->setBase(0);
    }
  };


  //============================================================================
  // Constructor & Destructor

  public: BindingMap() : SharedMap<TiObject>(true)
  {
  }

  public: BindingMap(const std::initializer_list<Argument> &args) : SharedMap<TiObject>(args, true)
  {
  }


  //============================================================================
  // Member Functions

  public: void setBase(BindingMap *base);

  private: void onBaseContentChanged(SharedMapBase<TiObject, TiObject> *src, ContentChangeOp op, Int index);

  private: void onBaseDetached();

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(ARGS...)>> setFunction(Char const *name, std::function<RT(ARGS...)> fn)
  {
    VALIDATE_NOT_NULL(name);
    SharedPtr<TiFunctionOverride<RT(ARGS...)>> tifn;
    auto index = this->findIndex(name);
    if (index == -1) {
      tifn = TiFunctionOverride<RT(ARGS...)>::create(fn);
    } else {
      auto super = this->get(index).ti_cast<TiFunctionBase>();
      tifn = TiFunctionOverride<RT(ARGS...)>::create(fn, super);
    }
    this->set(name, tifn);
    return tifn;
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(ARGS...)>> setFunction(Char const *name, RT(*fn)(ARGS...))
  {
    VALIDATE_NOT_NULL(name, fn);
    return this->setFunction(name, std::function<RT(ARGS...)>(fn));
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(ARGS...)>> updateFunction(Char const *name,
                                                              TiFunctionBase *currentTifn,
                                                              std::function<RT(ARGS...)> fn)
  {
    VALIDATE_NOT_NULL(name);
    auto index = this->findIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, S("name"), S("Not found."), name);
    }
    auto newTifn = TiFunctionOverride<RT(ARGS...)>::create(fn,
      currentTifn == 0 ? SharedPtr<TiFunctionBase>() : getSharedPtr(currentTifn->getSuper(), true));
    auto tifn = this->get(index).ti_cast_get<TiFunctionBase>();
    if (tifn == 0) {
      throw EXCEPTION(InvalidArgumentException, S("name"), S("No function override is defined at this key."), name);
    }
    if (tifn == currentTifn) {
      this->set(index, newTifn);
    } else {
      while (tifn != 0 && tifn->getSuper() != currentTifn) tifn = tifn->getSuper();
      if (tifn == 0) {
        throw EXCEPTION(InvalidArgumentException, S("name"),
                        S("Provided funciton override is not found at the given key."), name);
      }
      tifn->setSuper(newTifn);
    }
    return newTifn;
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(ARGS...)>> updateFunction(Char const *name,
                                                              TiFunctionBase *currentTifn,
                                                              RT(*fn)(ARGS...))
  {
    VALIDATE_NOT_NULL(name, fn);
    return this->updateFunction(name, currentTifn, std::function<RT(ARGS...)>(fn));
  }

  public: void updateFunctionChain(Char const *name, TiFunctionBase *currentTifn,
                                   SharedPtr<TiFunctionBase> const &newTifn);

  public: void resetFunction(Char const *name, TiFunctionBase *currentTifn);

  public: void resetFunctionChain(Char const *name, TiFunctionBase *currentTifn);

}; // class

} // namespace

#endif
