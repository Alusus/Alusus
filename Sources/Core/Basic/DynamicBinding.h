/**
 * @file Core/Basic/DynamicBinding.h
 * Contains the header of interface Core::Basic::DynamicBinding.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_DYNAMICBINDING_H
#define CORE_BASIC_DYNAMICBINDING_H

namespace Core::Basic
{

class DynamicBinding : public Binding
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(DynamicBinding, Binding, "Core.Basic", "Core", "alusus.org");


  //============================================================================
  // Member Functions

  /// @name Binding Overrides
  /// @{

  public: virtual Int setMember(Char const *name, TiObject *val)
  {
    return this->getBindingMap()->set(name, getSharedPtr(val));
  }

  public: virtual void setMember(Int index, TiObject *val)
  {
    this->getBindingMap()->set(index, getSharedPtr(val));
  }

  public: virtual void removeMember(Char const *name)
  {
    this->getBindingMap()->remove(name);
  }

  public: virtual void removeMember(Int index)
  {
    this->getBindingMap()->remove(index);
  }

  public: virtual Word getMemberCount() const
  {
    return this->getBindingMap()->getCount();
  }

  public: virtual TiObject* getMember(Char const *name) const
  {
    return this->getBindingMap()->get(name).get();
  }

  public: virtual TiObject* getMember(Int index) const
  {
    return this->getBindingMap()->get(index).get();
  }

  public: virtual TypeInfo const* getMemberNeededType(Char const *name) const
  {
    return TiObject::getTypeInfo();
  }

  public: virtual TypeInfo const* getMemberNeededType(Int index) const
  {
    return TiObject::getTypeInfo();
  }

  public: virtual HoldMode getMemberHoldMode(Char const *name) const
  {
    return HoldMode::SHARED_REF;
  }

  public: virtual HoldMode getMemberHoldMode(Int index) const
  {
    return HoldMode::SHARED_REF;
  }

  public: virtual SbStr const getMemberKey(Int index) const
  {
    return this->getBindingMap()->getKey(index);
  }

  public: virtual Int findMemberIndex(Char const *name) const
  {
    return this->getBindingMap()->findIndex(name);
  }

  /// @}

  /// @name DynamicBinding Functions
  /// @{

  public: virtual BindingMap* getBindingMap() = 0;

  public: virtual BindingMap const* getBindingMap() const
  {
    return const_cast<DynamicBinding*>(this)->getBindingMap();
  }

  public: virtual void inheritBindings(DynamicBinding *base)
  {
    this->getBindingMap()->setBase(base->getBindingMap());
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(ARGS...)>> setFunction(Char const *name, RT (*fn)(ARGS...))
  {
    VALIDATE_NOT_NULL(name, fn);
    return this->getBindingMap()->setFunction(name, fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(TiObject*, ARGS...)>> setMethod(Char const *name, RT (*fn)(TiObject*, ARGS...))
  {
    return this->setFunction(name, fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(TiFunctionBase*, ARGS...)>>
      setFunction(Char const *name, RT (*fn)(TiFunctionBase*, ARGS...))
  {
    VALIDATE_NOT_NULL(name, fn);
    return this->getBindingMap()->setFunction(name, fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(TiFunctionBase*, TiObject*, ARGS...)>>
      setMethod(Char const *name, RT (*fn)(TiFunctionBase*, TiObject*, ARGS...))
  {
    return this->setFunction(name, fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(ARGS...)>>
      setFunction(Char const *name, std::function<RT(ARGS...)> fn)
  {
    VALIDATE_NOT_NULL(name, fn);
    return this->getBindingMap()->setFunction(name, fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(TiObject*, ARGS...)>>
      setMethod(Char const *name, std::function<RT(TiObject*, ARGS...)> fn)
  {
    return this->setFunction(name, fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(TiFunctionBase*, ARGS...)>>
      setFunction(Char const *name, std::function<RT(TiFunctionBase*, ARGS...)> fn)
  {
    VALIDATE_NOT_NULL(name, fn);
    return this->getBindingMap()->setFunction(name, fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(TiFunctionBase*, TiObject*, ARGS...)>>
      setMethod(Char const *name, std::function<RT(TiFunctionBase*, TiObject*, ARGS...)> fn)
  {
    return this->setFunction(name, fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(ARGS...)>>
      updateFunction(Char const *name, TiFunctionBase *currentTifn, RT (*fn)(ARGS...))
  {
    VALIDATE_NOT_NULL(name, fn);
    return this->getBindingMap()->updateFunction(name, currentTifn, fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(TiObject*, ARGS...)>>
      updateMethod(Char const *name, TiFunctionBase *currentTifn, RT (*fn)(TiObject*, ARGS...))
  {
    return this->updateMethod(name, currentTifn, fn);
  }

  public: void updateFunctionChain(Char const *name, TiFunctionBase *currentTifn,
                                   SharedPtr<TiFunctionBase> const &newTifn)
  {
    VALIDATE_NOT_NULL(name, newTifn);
    this->getBindingMap()->updateFunctionChain(name, currentTifn, newTifn);
  }

  public: void resetFunction(Char const *name, TiFunctionBase *currentTifn)
  {
    VALIDATE_NOT_NULL(name, currentTifn);
    this->getBindingMap()->resetFunction(name, currentTifn);
  }

  public: void resetFunctionChain(Char const *name, TiFunctionBase *currentTifn)
  {
    VALIDATE_NOT_NULL(name, currentTifn);
    this->getBindingMap()->resetFunctionChain(name, currentTifn);
  }

  /// @}

}; // class


//==============================================================================
// Macros

#define IMPLEMENT_DYNAMIC_BINDINGS(bindingMap) \
  private: Core::Basic::BindingMap bindingMap; \
  public: virtual BindingMap* getBindingMap() \
  { \
    return &this->bindingMap; \
  }

} // namespace

#endif
