/**
 * @file Core/Data/ReferenceSeeker.cpp
 * Contains the implementation of class Core::Data::ReferenceSeeker.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data
{

Bool ReferenceSeeker::trySet(Reference const *ref, TiObject *target, TiObject *val) const
{
  if (ref == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Cannot be null."));
  }

  if (target == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("target"), STR("Cannot be null."));
  }

  Bool result = false;
  if (ref->getNext() == 0) {
    if (ref->isSingleValued()) {
      if (ref->getResultValidator() != 0) {
        TiObject *tempResult = 0;
        ref->getValue(this->dataProvider, target, tempResult);
        if (ref->getResultValidator()->validate(tempResult) == false) return false;
      }
      return ref->setValue(this->dataProvider, target, val);
    } else {
      ref->setValue(this->dataProvider, target,
        [=,&result](Int i, TiObject *&obj)->RefOp {
          if (ref->getResultValidator() != 0 && ref->getResultValidator()->validate(obj) == false) {
            return RefOp::MOVE;
          }
          obj = val;
          result = true;
          return RefOp::PERFORM_AND_STOP;
        });
    }
  } else {
    if (ref->isSingleValued()) {
      TiObject *innerTarget;
      if (!ref->getValue(this->dataProvider, target, innerTarget)) return false;
      if (innerTarget == 0) return false;
      // Prepare innerSrc.
      if (innerTarget->isA<PlainPairedPtr>()) {
        innerTarget = static_cast<PlainPairedPtr*>(innerTarget)->object;
      } else if (innerTarget->isA<SharedPairedPtr>()) {
        innerTarget = static_cast<SharedPairedPtr*>(innerTarget)->object.get();
      }
      if (ref->getResultValidator()!=0 && ref->getResultValidator()->validate(innerTarget)==false) {
        return false;
      }
      // Recurse into next level.
      return this->trySet(ref->getNext().get(), innerTarget, val);
    } else {
      ref->forEachValue(this->dataProvider, target,
        [=,&result](Int i, TiObject *innerTarget)->RefOp {
          // Prepare innerSrc.
          if (innerTarget->isA<PlainPairedPtr>()) {
            innerTarget = static_cast<PlainPairedPtr*>(innerTarget)->object;
          } else if (innerTarget->isA<SharedPairedPtr>()) {
            innerTarget = static_cast<SharedPairedPtr*>(innerTarget)->object.get();
          }
          if (ref->getResultValidator()!=0 && ref->getResultValidator()->validate(innerTarget)==false) {
            return RefOp::MOVE;
          }
          // Recurse into next level.
          if (this->trySet(ref->getNext().get(), innerTarget, val)) {
            result = true;
            return RefOp::STOP;
          } else {
            return RefOp::MOVE;
          }
        });
    }
  }
  return result;
}


RefOp ReferenceSeeker::set(Reference const *ref, TiObject *target, SeekerSetLambda handler,
                           Int *index) const
{
  if (ref == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Cannot be null."));
  }

  if (target == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("target"), STR("Cannot be null."));
  }

  // Prepare parent and index.
  Int _index = 0;
  if (index == 0) index = &_index;

  RefOp result = RefOp::MOVE;
  if (ref->getNext() == 0) {
    ref->setValue(this->dataProvider, target,
      [=,&result,&index](Int i, TiObject *&obj)->RefOp {
        if (ref->getResultValidator() != 0 && ref->getResultValidator()->validate(obj) == false) {
          return RefOp::MOVE;
        }
        RefOp ret = handler(*index, obj);
        *index++;
        if (!isMove(ret)) result = RefOp::STOP;
        return ret;
      });
  } else {
    ref->forEachValue(this->dataProvider, target,
      [=,&result](Int i, TiObject *innerTarget)->RefOp {
        // Prepare innerSrc.
        if (innerTarget->isA<PlainPairedPtr>()) {
          innerTarget = static_cast<PlainPairedPtr*>(innerTarget)->object;
        } else if (innerTarget->isA<SharedPairedPtr>()) {
          innerTarget = static_cast<SharedPairedPtr*>(innerTarget)->object.get();
        }
        if (ref->getResultValidator()!=0 && ref->getResultValidator()->validate(innerTarget)==false) {
          return RefOp::MOVE;
        }
        // Recurse into next level.
        RefOp ret = this->set(ref->getNext().get(), innerTarget, handler, index);
        result = ret;
        return ret;
      });
  }
  return result;
}


RefOp ReferenceSeeker::remove(Reference const *ref, TiObject *target, SeekerRemoveLambda handler,
                              Int *index) const
{
  if (ref == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Cannot be null."));
  }

  if (target == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("target"), STR("Cannot be null."));
  }

  // Prepare parent and index.
  Int _index = 0;
  if (index == 0) index = &_index;

  RefOp result = RefOp::MOVE;
  if (ref->getNext() == 0) {
    ref->removeValue(this->dataProvider, target,
      [=,&result,&index](Int i, TiObject *obj)->RefOp {
        if (ref->getResultValidator() != 0 && ref->getResultValidator()->validate(obj) == false) {
          return RefOp::MOVE;
        }
        RefOp ret = handler(*index, obj);
        *index++;
        if (!isMove(ret)) result = RefOp::STOP;
        return ret;
      });
  } else {
    ref->forEachValue(this->dataProvider, target,
      [=,&result](Int i, TiObject *innerTarget)->RefOp {
        // Prepare innerSrc.
        if (innerTarget->isA<PlainPairedPtr>()) {
          innerTarget = static_cast<PlainPairedPtr*>(innerTarget)->object;
        } else if (innerTarget->isA<SharedPairedPtr>()) {
          innerTarget = static_cast<SharedPairedPtr*>(innerTarget)->object.get();
        }
        if (ref->getResultValidator()!=0 && ref->getResultValidator()->validate(innerTarget)==false) {
          return RefOp::MOVE;
        }
        // Recurse into next level.
        RefOp ret = this->remove(ref->getNext().get(), innerTarget, handler, index);
        result = ret;
        return ret;
      });
  }
  return result;
}


Bool ReferenceSeeker::tryGet(Reference const *ref, TiObject *source, TiObject *&retVal,
                             TypeInfo const *parentTypeInfo, TiObject **retParent) const
{
  if (ref == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Cannot be null."));
  }

  if (source == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("source"), STR("Cannot be null."));
  }

  Bool result = false;
  if (ref->getNext() == 0) {
    if (ref->isSingleValued()) {
      TiObject *tempResult;
      if (ref->getValue(this->dataProvider, source, tempResult)) {
        if (ref->getResultValidator() == 0 || ref->getResultValidator()->validate(tempResult) == true) {
          retVal = tempResult;
          result = true;
        }
      }
    } else {
      ref->forEachValue(this->dataProvider, source,
        [=,&result,&retVal](Int i, TiObject *obj)->RefOp {
          if (ref->getResultValidator() != 0 && ref->getResultValidator()->validate(obj) == false) {
            return RefOp::MOVE;
          }
          retVal = obj;
          result = true;
          return RefOp::STOP;
        });
    }
    if (result && parentTypeInfo != 0) {
      if (source->isDerivedFrom(parentTypeInfo)) {
        *retParent = source;
      } else {
        *retParent = 0;
      }
    }
    return result;
  } else {
    if (ref->isSingleValued()) {
      TiObject *innerSrc;
      if (!ref->getValue(this->dataProvider, source, innerSrc)) return false;
      if (innerSrc == 0) return false;
      // Prepare innerSrc
      TiObject *innerParent;
      if (innerSrc->isA<SharedPairedPtr>()) {
        SharedPairedPtr *pairedPtr = static_cast<SharedPairedPtr*>(innerSrc);
        innerSrc = pairedPtr->object.get();
        innerParent = pairedPtr->parent.get();
      } else if (innerSrc->isA<PlainPairedPtr>()) {
        PlainPairedPtr *pairedPtr = static_cast<PlainPairedPtr*>(innerSrc);
        innerSrc = pairedPtr->object;
        innerParent = pairedPtr->parent;
      } else {
        innerParent = innerSrc;
      }
      if (ref->getResultValidator() != 0 &&
          ref->getResultValidator()->validate(innerSrc) == false) {
        return false;
      }
      // Recurse into next level.
      result = this->tryGet(ref->getNext().get(), innerSrc, retVal, parentTypeInfo, retParent);
      if (result && parentTypeInfo != 0 && *retParent == 0) {
        if (innerParent->isDerivedFrom(parentTypeInfo)) {
          *retParent = innerParent;
        }
      }
    } else {
      ref->forEachValue(this->dataProvider, source,
        [=,&retVal,&result](Int i, TiObject *innerSrc)->RefOp {
          if (innerSrc == 0) return RefOp::MOVE;
          // Prepare innerSrc
          TiObject *innerParent;
          if (innerSrc->isA<SharedPairedPtr>()) {
            SharedPairedPtr *pairedPtr = static_cast<SharedPairedPtr*>(innerSrc);
            innerSrc = pairedPtr->object.get();
            innerParent = pairedPtr->parent.get();
          } else if (innerSrc->isA<PlainPairedPtr>()) {
            PlainPairedPtr *pairedPtr = static_cast<PlainPairedPtr*>(innerSrc);
            innerSrc = pairedPtr->object;
            innerParent = pairedPtr->parent;
          } else {
            innerParent = innerSrc;
          }
          if (ref->getResultValidator() != 0 &&
              ref->getResultValidator()->validate(innerSrc) == false) {
            return RefOp::MOVE;
          }
          // Recurse into next level.
          result = this->tryGet(ref->getNext().get(), innerSrc, retVal, parentTypeInfo, retParent);
          if (result) {
            if (parentTypeInfo != 0 && *retParent == 0) {
              if (innerParent->isDerivedFrom(parentTypeInfo)) {
                *retParent = innerParent;
              }
            }
            return RefOp::STOP;
          } else {
            return RefOp::MOVE;
          }
        });
    }
  }
  return result;
}


RefOp ReferenceSeeker::forEach(Reference const *ref, TiObject *source, SeekerForeachLambda handler,
                               TypeInfo const *parentTypeInfo, TiObject *parent, Int *index) const
{
  if (ref == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Cannot be null."));
  }

  if (source == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("source"), STR("Cannot be null."));
  }

  // Prepare parent and index.
  Int _index = 0;
  if (index == 0) index = &_index;
  if (parentTypeInfo != 0) {
    if (source->isDerivedFrom(parentTypeInfo)) {
      parent = source;
    }
  }

  RefOp result = RefOp::MOVE;
  if (ref->getNext() == 0) {
    ref->forEachValue(this->dataProvider, source,
      [=,&result,&index](Int i, TiObject *obj)->RefOp {
        if (ref->getResultValidator() != 0 && ref->getResultValidator()->validate(obj) == false) {
          return RefOp::MOVE;
        }
        RefOp ret = handler(*index, obj, parent);
        *index++;
        if (!isMove(ret)) result = RefOp::STOP;
        return ret;
      });
  } else {
    ref->forEachValue(this->dataProvider, source,
      [=,&result](Int i, TiObject *innerSrc)->RefOp {
        // Prepare innerSrc and innerParent
        TiObject *innerParent = parent;
        if (innerSrc->isA<SharedPairedPtr>()) {
          SharedPairedPtr *pairedPtr = static_cast<SharedPairedPtr*>(innerSrc);
          if (ref->getResultValidator() != 0 &&
              ref->getResultValidator()->validate(pairedPtr->object.get()) == false) {
            return RefOp::MOVE;
          }
          innerSrc = pairedPtr->object.get();
          if (parentTypeInfo != 0) {
            if (pairedPtr->parent->isDerivedFrom(parentTypeInfo)) {
              innerParent = pairedPtr->parent.get();
            } else {
              innerParent = 0;
            }
          }
        } else if (innerSrc->isA<PlainPairedPtr>()) {
          PlainPairedPtr *pairedPtr = static_cast<PlainPairedPtr*>(innerSrc);
          if (ref->getResultValidator() != 0 &&
              ref->getResultValidator()->validate(pairedPtr->object) == false) {
            return RefOp::MOVE;
          }
          innerSrc = pairedPtr->object;
          if (parentTypeInfo != 0) {
            if (pairedPtr->parent->isDerivedFrom(parentTypeInfo)) {
              innerParent = pairedPtr->parent;
            } else {
              innerParent = 0;
            }
          }
        } else {
          if (ref->getResultValidator() != 0 &&
              ref->getResultValidator()->validate(innerSrc) == false) {
            return RefOp::MOVE;
          }
        }
        // Recurse into next level.
        RefOp ret = this->forEach(ref->getNext().get(), innerSrc, handler,
                                  parentTypeInfo, innerParent, index);
        result = ret;
        return ret;
      });
  }
  return result;
}

} } // namespace
