/**
 * @file Core/Basic/Bindings.h
 * Contains the header of interface Basic::Bindings.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_BINDINGS_H
#define CORE_BASIC_BINDINGS_H

namespace Core { namespace Basic
{

//==============================================================================
// Forward Declarations

class Bindings;
template <class RT, class ...ARGS> RT call(TiObject *callee, ARGS ...args);
template <class RT, class ...ARGS> RT call(Bindings *self, Char const *name, ARGS... args);


//==============================================================================
// Types

s_enum(HoldMode, SHARED_REF, WEAK_REF, PLAIN_REF, OWNER, VALUE);


//==============================================================================
// Bindings Interface

class Bindings : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Bindings, TiInterface, "Core.Basic", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual Int setMember(Char const *name, TiObject *val)
  {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found"), name);
  }
  public: virtual void setMember(Int index, TiObject *val)
  {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
  }

  public: virtual void removeMember(Char const *name)
  {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found"), name);
  }
  public: virtual void removeMember(Int index)
  {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
  }

  public: virtual Word getMemberCount() const
  {
    return 0;
  }

  public: virtual TiObject* getMember(Char const *name) const
  {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found"), name);
  }
  public: virtual TiObject* getMember(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
  }

  public: virtual TypeInfo* getMemberNeededType(Char const *name) const
  {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found"), name);
  }
  public: virtual TypeInfo* getMemberNeededType(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
  }

  public: virtual HoldMode getMemberHoldMode(Char const *name) const
  {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found"), name);
  }
  public: virtual HoldMode getMemberHoldMode(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
  }

  public: virtual SbStr const& getMemberKey(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
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
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."));
    }
    if (this->getMemberHoldMode(index) != HoldMode::VALUE) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not held by value."));
    }
    if (!val.isDerivedFrom(this->getMemberNeededType(index))) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Object type doesn't match needed type."));
    }
    this->setMember(index, const_cast<TiObject*>(&val));
  }

  public: template <class T> void setMember(Char const *name, SharedPtr<T> const &val)
  {
    Int index = this->findMemberIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."));
    }
    if (this->getMemberHoldMode(index) != HoldMode::SHARED_REF &&
        this->getMemberHoldMode(index) != HoldMode::WEAK_REF)
    {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not held by shared or weak ptr."));
    }
    if (!val->isDerivedFrom(this->getMemberNeededType(index))) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Object type doesn't match needed type."));
    }
    this->setMember(index, val.get());
  }

  public: template <class T> T& refMember(Char const *name)
  {
    Int index = this->findMemberIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."));
    }
    if (this->getMemberHoldMode(index) != HoldMode::VALUE) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not held by value."));
    }
    if (T::getTypeInfo() != this->getMemberNeededType(index)) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Object type doesn't match needed type."));
    }
    ASSERT(this->getMember(index) != 0);
    return *static_cast<T*>(this->getMember(index));
  }

  public: template <class T> T const& refMember(Char const *name) const
  {
    return const_cast<Bindings*>(this)->refMember<T>(name);
  }

  public: template <class T> SharedPtr<T> getSharedMember(Char const *name) const
  {
    Int index = this->findMemberIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."));
    }
    if (this->getMemberHoldMode(index) != HoldMode::SHARED_REF &&
        this->getMemberHoldMode(index) != HoldMode::WEAK_REF)
    {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not held by shared or weak ptr."));
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

} } // namespace

#endif
