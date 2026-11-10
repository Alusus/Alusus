/**
 * @file Core/Basic/BindingMap.h
 * Contains the header of class Core::Basic::BindingMap.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
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

  public: BindingMap(const std::initializer_list<Argument<TiObject>> &args) : SharedMap<TiObject>(args, true)
  {
  }


  //============================================================================
  // Member Functions

  public: void setBase(BindingMap *base);

  private: void onBaseContentChanged(SharedMapBase<TiObject, TiObject> *src, ContentChangeOp op, Int index);

  private: void onBaseDetached();

  /// Set the given override on top of the callee currently at the given key, making that callee its super.
  private: void addOverride(Char const *name, SharedPtr<TiCallee> const &tifn);

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(ARGS...)>> setFunction(Char const *name, std::function<RT(ARGS...)> fn)
  {
    VALIDATE_NOT_NULL(name);
    auto tifn = TiFunctionOverride<RT(ARGS...)>::create(fn);
    this->addOverride(name, tifn);
    return tifn;
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(ARGS...)>> setFunction(Char const *name, RT(*fn)(ARGS...))
  {
    VALIDATE_NOT_NULL(name, fn);
    return this->setFunction(name, std::function<RT(ARGS...)>(fn));
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiMethodOverride<RT(ARGS...)>> setMethod(Char const *name, RT(*fn)(TiObject*, ARGS...))
  {
    VALIDATE_NOT_NULL(name, fn);
    auto tifn = TiMethodOverride<RT(ARGS...)>::create(fn);
    this->addOverride(name, tifn);
    return tifn;
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(ARGS...)>> updateFunction(Char const *name,
                                                              TiCallee *currentTifn,
                                                              std::function<RT(ARGS...)> fn)
  {
    VALIDATE_NOT_NULL(name);
    auto newTifn = TiFunctionOverride<RT(ARGS...)>::create(fn,
      currentTifn == 0 ? SharedPtr<TiCallee>() : getSharedPtr(currentTifn->getSuper(), true));
    this->updateCalleeChain(name, currentTifn, newTifn);
    return newTifn;
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(ARGS...)>> updateFunction(Char const *name,
                                                              TiCallee *currentTifn,
                                                              RT(*fn)(ARGS...))
  {
    VALIDATE_NOT_NULL(name, fn);
    return this->updateFunction(name, currentTifn, std::function<RT(ARGS...)>(fn));
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiMethodOverride<RT(ARGS...)>> updateMethod(Char const *name,
                                                          TiCallee *currentTifn,
                                                          RT(*fn)(TiObject*, ARGS...))
  {
    VALIDATE_NOT_NULL(name, fn);
    auto newTifn = TiMethodOverride<RT(ARGS...)>::create(fn,
      currentTifn == 0 ? SharedPtr<TiCallee>() : getSharedPtr(currentTifn->getSuper(), true));
    this->updateCalleeChain(name, currentTifn, newTifn);
    return newTifn;
  }

  public: void updateCalleeChain(Char const *name, TiCallee *currentTifn,
                                 SharedPtr<TiCallee> const &newTifn);

  public: void resetCallee(Char const *name, TiCallee *currentTifn);

  public: void resetCalleeChain(Char const *name, TiCallee *currentTifn);

  /// Check whether the given callee is part of the callee chain at the given key.
  public: Bool hasCallee(Char const *name, TiCallee *tifn) const;

}; // class

} // namespace

#endif
