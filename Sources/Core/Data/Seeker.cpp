/**
 * @file Core/Data/Seeker.cpp
 * Contains the implementation of class Core::Data::Seeker.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data
{

Bool Seeker::trySet(TiObject const *ref, TiObject *target, TiObject *val)
{
  Bool result = false;
  this->set(ref, target, [=,&result](Seeker *seeker, TiObject *&obj)->SeekVerb {
    obj = val;
    result = true;
    return SeekVerb::PERFORM_AND_MOVE;
  });
  return result;
}


Bool Seeker::tryRemove(TiObject const *ref, TiObject *target)
{
  Bool ret = false;
  this->remove(ref, target, [&ret](Seeker *seeker, TiObject *o)->SeekVerb {
    ret = true;
    return SeekVerb::PERFORM_AND_MOVE;
  });
  return ret;
}


Bool Seeker::tryGet(TiObject const *ref, TiObject *target, TiObject *&retVal)
{
  Bool ret = false;
  this->foreach(ref, target, [&ret,&retVal](Seeker *seeker, TiObject *o)->SeekVerb {
    retVal = o;
    ret = true;
    return SeekVerb::STOP;
  });
  return ret;
}

} } // namespace
