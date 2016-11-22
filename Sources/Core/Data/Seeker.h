/**
 * @file Core/Data/Seeker.h
 * Contains the header of class Core::Data::Seeker.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_SEEKER_H
#define CORE_DATA_SEEKER_H

namespace Core { namespace Data
{

class Seeker : public TiObject, public virtual DynamicRtMembers
{
  //============================================================================
  // Type Info

  TYPE_INFO(Seeker, TiObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(TiObject, DynamicRtMembers);


  //============================================================================
  // Types

  public: ti_s_enum(SeekVerb, Integer, "Core.Data", "Core", "alusus.net",
                    MOVE, STOP, PERFORM_AND_MOVE, PERFORM_AND_STOP);

  public: typedef std::function<SeekVerb(Seeker *seeker, TiObject *&obj)> SeekSetCallback;
  public: typedef std::function<SeekVerb(Seeker *seeker, TiObject *obj)> SeekRemoveCallback;
  public: typedef std::function<SeekVerb(Seeker *seeker, TiObject *obj)> SeekForeachCallback;


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_RT_MEMBERS;


  //============================================================================
  // Constructors

  Seeker()
  {
  }

  Seeker(Seeker *parent)
  {
    this->inherit(parent);
  }


  //============================================================================
  // Member Functions

  /// @name Core Operation Functions
  /// @{

  public: void set(TiObject const *ref, TiObject *target, SeekSetCallback callback)
  {
    callVoidRtMethod(this, "_set", ref, target, callback);
  }

  public: void remove(TiObject const *ref, TiObject *target, SeekRemoveCallback callback)
  {
    callVoidRtMethod(this, "_remove", ref, target, callback);
  }

  public: void foreach(TiObject const *ref, TiObject *target, SeekForeachCallback callback)
  {
    callVoidRtMethod(this, "_foreach", ref, target, callback);
  }

  /// @}

  /// @name Helper Functions
  /// @{

  public: Bool trySet(TiObject const *ref, TiObject *target, TiObject *val);

  public: void set(TiObject const *ref, TiObject *target, TiObject *val)
  {
    if (!this->trySet(ref, target, val)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool tryRemove(TiObject const *ref, TiObject *target);

  public: void remove(TiObject const *ref, TiObject *target)
  {
    if (!this->tryRemove(ref, target)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool tryGet(TiObject const *ref, TiObject *target, TiObject *&retVal);

  public: TiObject* tryGet(TiObject const *ref, TiObject *target)
  {
    TiObject *result = 0;
    this->tryGet(ref, target, result);
    return result;
  }

  public: virtual TiObject* get(TiObject const *ref, TiObject *target)
  {
    TiObject *retVal = this->tryGet(ref, target);
    if (retVal == 0) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
    return retVal;
  }

  public: static Bool isPerform(SeekVerb verb)
  {
    return verb == SeekVerb::PERFORM_AND_STOP || verb == SeekVerb::PERFORM_AND_MOVE;
  }

  public: static Bool isMove(SeekVerb verb)
  {
    return verb == SeekVerb::MOVE || verb == SeekVerb::PERFORM_AND_MOVE;
  }

  /// @}

}; // class

} } // namespace

#endif
