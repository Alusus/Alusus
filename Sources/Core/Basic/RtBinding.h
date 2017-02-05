/**
 * @file Core/Basic/RtBinding.h
 * Contains the header of interface Basic::RtBinding.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_RTBINDING_H
#define CORE_BASIC_RTBINDING_H

namespace Core { namespace Basic
{

//==============================================================================
// Forward Declarations

class RtBinding;
template <class RT, class ...ARGS> RT call(TiObject *callee, ARGS ...args);
template <class RT, class ...ARGS> RT call(RtBinding *self, Char const *name, ARGS... args);


//==============================================================================
// Types

s_enum(HoldMethod, SHARED_REF, WEAK_REF, PLAIN_REF, OWNER, VALUE);


//==============================================================================
// RtBinding Interface

class RtBinding : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(RtBinding, TiInterface, "Core.Basic", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual Int setMember(Char const *name, TiObject *val) = 0;
  public: virtual void setMember(Int index, TiObject *val) = 0;

  public: virtual void removeMember(Char const *name) = 0;
  public: virtual void removeMember(Int index) = 0;

  public: virtual Word getMemberCount() const = 0;

  public: virtual TiObject* getMember(Char const *name) = 0;
  public: virtual TiObject* getMember(Int index) = 0;

  public: virtual TiObject const* getMember(Char const *name) const = 0;
  public: virtual TiObject const* getMember(Int index) const = 0;

  public: virtual TypeInfo* getMemberNeededType(Char const *name) const = 0;
  public: virtual TypeInfo* getMemberNeededType(Int index) const = 0;

  public: virtual HoldMethod getMemberHoldMethod(Char const *name) const = 0;
  public: virtual HoldMethod getMemberHoldMethod(Int index) const = 0;

  public: virtual SbStr const& getMemberKey(Int index) const = 0;
  public: virtual Int findMemberIndex(Char const *name) const = 0;


  //============================================================================
  // Helper Functions

  public: template <class T> void setMember(Char const *name, T const &val)
  {
    Int index = this->findMemberIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."));
    }
    if (this->getMemberHoldMethod(index) != HoldMethod::VALUE) {
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
    if (this->getMemberHoldMethod(index) != HoldMethod::SHARED_REF &&
        this->getMemberHoldMethod(index) != HoldMethod::WEAK_REF)
    {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not held by shared or weak ptr."));
    }
    if (!val->isDerivedFrom(this->getMemberNeededType(index))) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Object type doesn't match needed type."));
    }
    this->setMember(index, val.get());
  }

  public: template <class T> T const& refMember(Char const *name)
  {
    Int index = this->findMemberIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."));
    }
    if (this->getMemberHoldMethod(index) != HoldMethod::VALUE) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not held by value."));
    }
    if (T::getTypeInfo() != this->getMemberNeededType(index)) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Object type doesn't match needed type."));
    }
    ASSERT(this->getMember(index) != 0);
    return *static_cast<T*>(this->getMember(index));
  }

  public: template <class T> SharedPtr<T> getSharedMember(Char const *name)
  {
    Int index = this->findMemberIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."));
    }
    if (this->getMemberHoldMethod(index) != HoldMethod::SHARED_REF &&
        this->getMemberHoldMethod(index) != HoldMethod::WEAK_REF)
    {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not held by shared or weak ptr."));
    }
    return getSharedPtr<T>(this->getMember(index));
  }

  public: template <class T> T* getMember(Char const *name)
  {
    return ti_cast<T>(this->getMember(name));
  }

  public: template <class RT, class ...ARGS> RT call(Char const *name, ARGS... args)
  {
    return Core::Basic::call<RT>(this, name, this, args...);
  }

  public: template <class RT, class ...ARGS> RT call(TiObject *callee, ARGS... args)
  {
    return Core::Basic::call<RT>(callee, this, args...);
  }

}; // class

} } // namespace

#endif
