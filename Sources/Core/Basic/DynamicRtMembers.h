/**
 * @file Core/Basic/DynamicRtMembers.h
 * Contains the header of interface Basic::DynamicRtMembers.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_DYNAMICRTMEMBERS_H
#define CORE_BASIC_DYNAMICRTMEMBERS_H

namespace Core { namespace Basic
{

class DynamicRtMembers : public RtMembers
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(DynamicRtMembers, RtMembers, "Core.Basic", "Core", "alusus.net");


  //============================================================================
  // Types

  typedef SharedMap<TiObject, TiObject> Map;


  //============================================================================
  // Abstract Functions

  public: virtual Map* getRtMembersMap() = 0;

  public: virtual void inherit(DynamicRtMembers *base) = 0;

  public: virtual void inherit(TiObject *base)
  {
    auto rtMembers = ti_cast<DynamicRtMembers>(base);
    if (rtMembers == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("base"),
                      STR("Object is invalid or does not implement DynamicRtMembers."));
    }
    this->inherit(rtMembers);
  }

  public: template <class RT, class ...ARGS>
    void setRtFunction(Char const *name, std::function<RT(ARGS...)> fn, Bool replace = false)
  {
    auto map = this->getRtMembersMap();
    SharedPtr<TiFunctionBase> tifn;
    if (replace) {
      tifn = TiFunction<RT(ARGS...)>::create(fn);
    } else {
      auto index = map->findIndex(name);
      if (index == -1) {
        tifn = TiFunction<RT(ARGS...)>::create(fn);
      } else {
        auto super = map->get(index).ti_cast<TiFunctionBase>();
        if (super == 0) {
          tifn = TiFunction<RT(ARGS...)>::create(fn);
        } else {
          tifn = TiFunctionOverride<RT(ARGS...)>::create(fn, super);
        }
      }
    }
    map->set(name, tifn);
  }

  public: template <class RT, class ...ARGS>
    void setRtFunction(Char const *name, RT(*fn)(ARGS...), Bool replace = false)
  {
    this->setRtFunction(name, std::function<RT(ARGS...)>(fn), replace);
  }

  public: template <class RT, class CT, class ...ARGS>
    void setRtMemberFunction(Char const *name, RT (CT::*fn)(ARGS...), Bool replace = false)
  {
    this->setRtFunction(name, std::function<RT(CT*, ARGS...)>(fn), replace);
  }

}; // class


//==============================================================================
// Macros

#define IMPLEMENT_DYNAMIC_RT_MEMBERS \
  private: Core::Basic::DynamicRtMembers::Map map; \
  public: virtual Map* getRtMembersMap() \
  { \
    return &this->map; \
  } \
  public: virtual void inherit(Core::Basic::DynamicRtMembers *base) \
  { \
    if (base == 0) { \
      throw EXCEPTION(InvalidArgumentException, STR("base"), STR("Must not be null.")); \
    } \
    this->map.setBase(base->getRtMembersMap()); \
  } \
  public: virtual Int setRtMember(Char const *name, TiObject *val) \
  { \
    return this->map.set(name, getSharedPtr(val)); \
  } \
  public: virtual void setRtMember(Int index, TiObject *val) \
  { \
    this->map.set(index, getSharedPtr(val)); \
  } \
  public: virtual void removeRtMember(Char const *name) \
  { \
    this->map.remove(name); \
  } \
  public: virtual void removeRtMember(Int index) \
  { \
    this->map.remove(index); \
  } \
  public: virtual Word getRtMemberCount() const \
  { \
    return this->map.getCount(); \
  } \
  public: virtual TiObject* getRtMember(Char const *name) \
  { \
    return this->map.get(name).get(); \
  } \
  public: virtual TiObject* getRtMember(Int index) \
  { \
    return this->map.get(index).get(); \
  } \
  public: virtual TiObject const* getRtMember(Char const *name) const \
  { \
    return this->map.get(name).get(); \
  } \
  public: virtual TiObject const* getRtMember(Int index) const \
  { \
    return this->map.get(index).get(); \
  } \
  public: virtual TypeInfo* getRtMemberNeededType(Char const *name) const \
  { \
    return TiObject::getTypeInfo(); \
  } \
  public: virtual TypeInfo* getRtMemberNeededType(Int index) const \
  { \
    return TiObject::getTypeInfo(); \
  } \
  public: virtual HoldMethod getRtMemberHoldMethod(Char const *name) const \
  { \
    return HoldMethod::SHARED_REF; \
  } \
  public: virtual HoldMethod getRtMemberHoldMethod(Int index) const \
  { \
    return HoldMethod::SHARED_REF; \
  } \
  public: virtual SbStr const& getRtMemberKey(Int index) const \
  { \
    return this->map.getKey(index); \
  } \
  public: virtual Int findRtMemberIndex(Char const *name) const \
  { \
    return this->map.findIndex(name); \
  }

} } // namespace

#endif
