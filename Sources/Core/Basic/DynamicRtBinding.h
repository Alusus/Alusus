/**
 * @file Core/Basic/DynamicRtBinding.h
 * Contains the header of interface Core::Basic::DynamicRtBinding.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_DYNAMICRTBINDING_H
#define CORE_BASIC_DYNAMICRTBINDING_H

namespace Core { namespace Basic
{

class DynamicRtBinding : public RtBinding
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(DynamicRtBinding, RtBinding, "Core.Basic", "Core", "alusus.net");


  //============================================================================
  // Member Functions

  /// @name RtBinding Overrides
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

  public: virtual TiObject* getMember(Char const *name)
  {
    return this->getBindingMap()->get(name).get();
  }

  public: virtual TiObject* getMember(Int index)
  {
    return this->getBindingMap()->get(index).get();
  }

  public: virtual TiObject const* getMember(Char const *name) const
  {
    return this->getBindingMap()->get(name).get();
  }

  public: virtual TiObject const* getMember(Int index) const
  {
    return this->getBindingMap()->get(index).get();
  }

  public: virtual TypeInfo* getMemberNeededType(Char const *name) const
  {
    return TiObject::getTypeInfo();
  }

  public: virtual TypeInfo* getMemberNeededType(Int index) const
  {
    return TiObject::getTypeInfo();
  }

  public: virtual HoldMethod getMemberHoldMethod(Char const *name) const
  {
    return HoldMethod::SHARED_REF;
  }

  public: virtual HoldMethod getMemberHoldMethod(Int index) const
  {
    return HoldMethod::SHARED_REF;
  }

  public: virtual SbStr const& getMemberKey(Int index) const
  {
    return this->getBindingMap()->getKey(index);
  }

  public: virtual Int findMemberIndex(Char const *name) const
  {
    return this->getBindingMap()->findIndex(name);
  }

  /// @}

  /// @name DynamicRtBinding Functions
  /// @{

  public: virtual BindingMap* getBindingMap() = 0;

  public: virtual BindingMap const* getBindingMap() const
  {
    return const_cast<DynamicRtBinding*>(this)->getBindingMap();
  }

  public: virtual void inherit(DynamicRtBinding *base)
  {
    this->getBindingMap()->setBase(base->getBindingMap());
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(RtBinding*,ARGS...)>> setFunction(Char const *name,
                                                                      RT (*fn)(RtBinding*,ARGS...))
  {
    VALIDATE_NOT_NULL(name, fn);
    return this->getBindingMap()->setFunction(name, fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(TiFunctionBase*, RtBinding*,ARGS...)>>
      setFunction(Char const *name, RT (*fn)(TiFunctionBase*, RtBinding*,ARGS...))
  {
    VALIDATE_NOT_NULL(name, fn);
    return this->getBindingMap()->setFunction(name, fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(RtBinding*,ARGS...)>> setFunction(Char const *name,
                                                                      std::function<RT(RtBinding*,ARGS...)> fn)
  {
    VALIDATE_NOT_NULL(name, fn);
    return this->getBindingMap()->setFunction(name, fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(TiFunctionBase*, RtBinding*,ARGS...)>>
      setFunction(Char const *name, std::function<RT(TiFunctionBase*, RtBinding*,ARGS...)> fn)
  {
    VALIDATE_NOT_NULL(name, fn);
    return this->getBindingMap()->setFunction(name, fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(RtBinding*,ARGS...)>> updateFunction(Char const *name,
                                                                         TiFunctionBase *currentTifn,
                                                                         RT (*fn)(RtBinding*,ARGS...))
  {
    VALIDATE_NOT_NULL(name, fn);
    return this->getBindingMap()->updateFunction(name, currentTifn, fn);
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

  /// @name BindingIndexCache Helper Functions
  /// @{

  public: TiObject* getMember(BindingIndexCache &bic)
  {
    return this->getMember(bic.get());
  }

  public: template <class T> T* getMember(BindingIndexCache &bic)
  {
    return ti_cast<T>(this->getMember(bic.get()));
  }

  public: using RtBinding::call;

  public: template <class RT, class ...ARGS> RT call(BindingIndexCache &bic, ARGS... args)
  {
    return Core::Basic::call<RT, RtBinding*, ARGS...>(this->getMember(bic), this, args...);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(RtBinding*,ARGS...)>> setFunction(BindingIndexCache &bic,
                                                                      RT (*fn)(RtBinding*,ARGS...))
  {
    VALIDATE_NOT_NULL(fn);
    return this->getBindingMap()->setFunction(bic.getName(), fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(TiFunctionBase*, RtBinding*,ARGS...)>>
      setFunction(BindingIndexCache &bic, RT (*fn)(TiFunctionBase*, RtBinding*,ARGS...))
  {
    VALIDATE_NOT_NULL(fn);
    return this->getBindingMap()->setFunction(bic.getName(), fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(RtBinding*,ARGS...)>> setFunction(BindingIndexCache &bic,
                                                                      std::function<RT(RtBinding*,ARGS...)> fn)
  {
    VALIDATE_NOT_NULL(fn);
    return this->getBindingMap()->setFunction(bic.getName(), fn);
  }

  public: template <class RT, class ...ARGS>
    SharedPtr<TiFunctionOverride<RT(TiFunctionBase*, RtBinding*,ARGS...)>>
      setFunction(BindingIndexCache &bic, std::function<RT(TiFunctionBase*, RtBinding*,ARGS...)> fn)
  {
    VALIDATE_NOT_NULL(fn);
    return this->getBindingMap()->setFunction(bic.getName(), fn);
  }

  public: void resetFunction(BindingIndexCache &bic, TiFunctionBase *currentTifn)
  {
    VALIDATE_NOT_NULL(currentTifn);
    this->getBindingMap()->resetFunction(bic.getName(), currentTifn);
  }

  /// @}

}; // class


//==============================================================================
// Macros

#define IMPLEMENT_DYNAMIC_RT_BINDING \
  private: Core::Basic::BindingMap bindingMap; \
  public: virtual BindingMap* getBindingMap() \
  { \
    return &this->bindingMap; \
  }

} } // namespace

#endif
