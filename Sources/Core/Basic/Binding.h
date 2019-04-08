/**
 * @file Core/Basic/Binding.h
 * Contains the header of interface Basic::Binding.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_BINDING_H
#define CORE_BASIC_BINDING_H

namespace Core::Basic
{

//==============================================================================
// Forward Declarations

class Binding;
template <class RT, class ...ARGS> RT call(TiObject *callee, ARGS ...args);
template <class RT, class ...ARGS> RT call(Binding *self, Char const *name, ARGS... args);


//==============================================================================
// Binding Interface

class Binding : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Binding, TiInterface, "Core.Basic", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual Int setMember(Char const *name, TiObject *val)
  {
    throw EXCEPTION(InvalidArgumentException, S("name"), S("Member not found"), name);
  }
  public: virtual void setMember(Int index, TiObject *val)
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }

  public: virtual Word getMemberCount() const
  {
    return 0;
  }

  public: virtual TiObject* getMember(Char const *name) const
  {
    throw EXCEPTION(InvalidArgumentException, S("name"), S("Member not found"), name);
  }
  public: virtual TiObject* getMember(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }

  public: virtual TypeInfo* getMemberNeededType(Char const *name) const
  {
    throw EXCEPTION(InvalidArgumentException, S("name"), S("Member not found"), name);
  }
  public: virtual TypeInfo* getMemberNeededType(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }

  public: virtual HoldMode getMemberHoldMode(Char const *name) const
  {
    throw EXCEPTION(InvalidArgumentException, S("name"), S("Member not found"), name);
  }
  public: virtual HoldMode getMemberHoldMode(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }

  public: virtual SbStr const& getMemberKey(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }
  public: virtual Int findMemberIndex(Char const *name) const
  {
    return -1;
  }


  //============================================================================
  // Helper Functions

  public: template <class T,
                    typename std::enable_if<std::is_base_of<TiObject, T>::value, int>::type = 0>
    void setMember(Char const *name, T const &val)
  {
    Int index = this->findMemberIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, S("name"), S("Member not found."), name);
    }
    if (this->getMemberHoldMode(index) != HoldMode::VALUE) {
      throw EXCEPTION(InvalidArgumentException, S("name"), S("Member is not held by value."), name);
    }
    if (!val.isDerivedFrom(this->getMemberNeededType(index))) {
      throw EXCEPTION(InvalidArgumentException, S("name"), S("Object type doesn't match needed type."), name);
    }
    this->setMember(index, const_cast<TiObject*>(&val));
  }

  public: template <class T> void setMember(Char const *name, SharedPtr<T> const &val)
  {
    Int index = this->findMemberIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, S("name"), S("Member not found."), name);
    }
    if (this->getMemberHoldMode(index) != HoldMode::SHARED_REF &&
        this->getMemberHoldMode(index) != HoldMode::WEAK_REF)
    {
      throw EXCEPTION(InvalidArgumentException, S("name"), S("Member is not held by shared or weak ptr."), name);
    }
    if (val != 0 && !val->isDerivedFrom(this->getMemberNeededType(index))) {
      throw EXCEPTION(InvalidArgumentException, S("name"), S("Object type doesn't match needed type."), name);
    }
    this->setMember(index, val.get());
  }

  public: template <class T> T& refMember(Int index)
  {
    if (this->getMemberHoldMode(index) != HoldMode::VALUE) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Member is not held by value."), index);
    }
    if (T::getTypeInfo() != this->getMemberNeededType(index)) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Object type doesn't match needed type."), index);
    }
    ASSERT(this->getMember(index) != 0);
    return *static_cast<T*>(this->getMember(index));
  }

  public: template <class T> T& refMember(Char const *name)
  {
    Int index = this->findMemberIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, S("name"), S("Member not found."), name);
    }
    if (this->getMemberHoldMode(index) != HoldMode::VALUE) {
      throw EXCEPTION(InvalidArgumentException, S("name"), S("Member is not held by value."), name);
    }
    if (T::getTypeInfo() != this->getMemberNeededType(index)) {
      throw EXCEPTION(InvalidArgumentException, S("name"), S("Object type doesn't match needed type."), name);
    }
    ASSERT(this->getMember(index) != 0);
    return *static_cast<T*>(this->getMember(index));
  }

  public: template <class T> T const& refMember(Char const *name) const
  {
    return const_cast<Binding*>(this)->refMember<T>(name);
  }

  public: template <class T> SharedPtr<T> getSharedMember(Char const *name) const
  {
    Int index = this->findMemberIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, S("name"), S("Member not found."), name);
    }
    if (this->getMemberHoldMode(index) != HoldMode::SHARED_REF &&
        this->getMemberHoldMode(index) != HoldMode::WEAK_REF)
    {
      throw EXCEPTION(InvalidArgumentException, S("name"), S("Member is not held by shared or weak ptr."), name);
    }
    return getSharedPtr<T>(this->getMember(index));
  }

  public: template <class T> T* getMember(Char const *name) const
  {
    return ti_cast<T>(this->getMember(name));
  }

  public: template <class RT, class ...ARGS> RT call(Char const *name, ARGS... args)
  {
    return Core::Basic::call<RT, TiObject*, ARGS...>(this, name, this->getTiObject(), args...);
  }

  public: template <class RT, class ...ARGS> RT call(TiObject *callee, ARGS... args)
  {
    return Core::Basic::call<RT, TiObject*, ARGS...>(callee, this->getTiObject(), args...);
  }

}; // class

} // namespace

#endif
