/**
 * @file Core/Basic/RtMembers.h
 * Contains the header of interface Basic::RtMembers.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_RTMEMBERS_H
#define CORE_BASIC_RTMEMBERS_H

namespace Core { namespace Basic
{

//==============================================================================
// Types

s_enum(HoldMethod, SHARED_REF, WEAK_REF, PLAIN_REF, OWNER, VALUE);


//==============================================================================
// RtMembers Interface

class RtMembers : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(RtMembers, TiInterface, "Core.Basic", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual Int setRtMember(Char const *name, TiObject *val) = 0;
  public: virtual void setRtMember(Int index, TiObject *val) = 0;

  public: virtual void removeRtMember(Char const *name) = 0;
  public: virtual void removeRtMember(Int index) = 0;

  public: virtual Word getRtMemberCount() const = 0;

  public: virtual TiObject* getRtMember(Char const *name) = 0;
  public: virtual TiObject* getRtMember(Int index) = 0;

  public: virtual TiObject const* getRtMember(Char const *name) const = 0;
  public: virtual TiObject const* getRtMember(Int index) const = 0;

  public: virtual TypeInfo* getRtMemberNeededType(Char const *name) const = 0;
  public: virtual TypeInfo* getRtMemberNeededType(Int index) const = 0;

  public: virtual HoldMethod getRtMemberHoldMethod(Char const *name) const = 0;
  public: virtual HoldMethod getRtMemberHoldMethod(Int index) const = 0;

  public: virtual SbStr const& getRtMemberKey(Int index) const = 0;
  public: virtual Int findRtMemberIndex(Char const *name) const = 0;


  //============================================================================
  // Helper Functions

  public: template <class T> void setRtMember(Char const *name, T const &val)
  {
    Int index = this->findRtMemberIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."));
    }
    if (this->getRtMemberHoldMethod(index) != HoldMethod::VALUE) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not held by value."));
    }
    if (!val.isDerivedFrom(this->getRtMemberNeededType(index))) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Object type doesn't match needed type."));
    }
    this->setRtMember(index, const_cast<TiObject*>(&val));
  }

  public: template <class T> void setRtMember(Char const *name, SharedPtr<T> const &val)
  {
    Int index = this->findRtMemberIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."));
    }
    if (this->getRtMemberHoldMethod(index) != HoldMethod::SHARED_REF &&
        this->getRtMemberHoldMethod(index) != HoldMethod::WEAK_REF)
    {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not held by shared or weak ptr."));
    }
    if (!val->isDerivedFrom(this->getRtMemberNeededType(index))) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Object type doesn't match needed type."));
    }
    this->setRtMember(index, val.get());
  }

  public: template <class T> T const& refRtMember(Char const *name)
  {
    Int index = this->findRtMemberIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."));
    }
    if (this->getRtMemberHoldMethod(index) != HoldMethod::VALUE) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not held by value."));
    }
    if (T::getTypeInfo() != this->getRtMemberNeededType(index)) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Object type doesn't match needed type."));
    }
    ASSERT(this->getRtMember(index) != 0);
    return *static_cast<T*>(this->getRtMember(index));
  }

  public: template <class T> SharedPtr<T> getSharedRtMember(Char const *name)
  {
    Int index = this->findRtMemberIndex(name);
    if (index == -1) {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."));
    }
    if (this->getRtMemberHoldMethod(index) != HoldMethod::SHARED_REF &&
        this->getRtMemberHoldMethod(index) != HoldMethod::WEAK_REF)
    {
      throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not held by shared or weak ptr."));
    }
    return getSharedPtr<T>(this->getRtMember(index));
  }

  public: template <class T> T* getRtMember(Char const *name)
  {
    return ti_cast<T>(this->getRtMember(name));
  }

}; // class

} } // namespace

#endif
