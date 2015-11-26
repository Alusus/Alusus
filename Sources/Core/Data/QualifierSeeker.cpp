/**
 * @file Core/Data/QualifierSeeker.cpp
 * Contains the implementation of class Core::Data::QualifierSeeker.
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

RefOp QualifierSeeker::set(Char const *qualifier, IdentifiableObject *target, SeekerSetLambda handler,
                           Int *index) const
{
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }

  if (target == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("target"), STR("Cannot be null."));
  }

  // Prepare parent and index.
  Int _index = 0;
  if (index == 0) index = &_index;

  RefOp result;
  Reference const *ref = &this->parser.parseQualifierSegmentToTemp(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    ref->setValue(this->dataProvider, target, [=,&result,&index](Int i, IdentifiableObject *&obj)->RefOp {
                    if (ref->getResultValidator() != 0 && ref->getResultValidator()->validate(obj) == false) {
                      return RefOp::MOVE;
                    }
                    RefOp ret = handler(*index, obj);
                    *index++;
                    if (isMove(ret)) result = RefOp::MOVE;
                    else result = RefOp::STOP;
                    return ret;
                  });
  } else {
    ++qualifier;
    ref->forEachValue(this->dataProvider, target, [=,&result](Int i, IdentifiableObject *innerTarget)->RefOp {
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
                        RefOp ret = this->set(qualifier, innerTarget, handler, index);
                        result = ret;
                        return ret;
                      });
  }
  return result;
}


RefOp QualifierSeeker::remove(Char const *qualifier, IdentifiableObject *target, SeekerRemoveLambda handler,
                              Int *index) const
{
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }

  if (target == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("target"), STR("Cannot be null."));
  }

  // Prepare parent and index.
  Int _index = 0;
  if (index == 0) index = &_index;

  RefOp result;
  Reference const *ref = &this->parser.parseQualifierSegmentToTemp(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    ref->removeValue(this->dataProvider, target, [=,&result,&index](Int i, IdentifiableObject *obj)->RefOp {
                       if (ref->getResultValidator() != 0 && ref->getResultValidator()->validate(obj) == false) {
                         return RefOp::MOVE;
                       }
                       RefOp ret = handler(*index, obj);
                       *index++;
                       if (isMove(ret)) result = RefOp::MOVE;
                       else result = RefOp::STOP;
                       return ret;
                     });
  } else {
    ++qualifier;
    ref->forEachValue(this->dataProvider, target, [=,&result](Int i, IdentifiableObject *innerTarget)->RefOp {
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
                        RefOp ret = this->remove(qualifier, innerTarget, handler, index);
                        result = ret;
                        return ret;
                      });
  }
  return result;
}


RefOp QualifierSeeker::forEach(Char const *qualifier, IdentifiableObject *source, SeekerForeachLambda handler,
                               TypeInfo const *parentTypeInfo, IdentifiableObject *parent, Int *index) const
{
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
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

  RefOp result;
  Reference const *ref = &this->parser.parseQualifierSegmentToTemp(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    ref->forEachValue(this->dataProvider, source, [=,&result,&index](Int i, IdentifiableObject *obj)->RefOp {
                        if (ref->getResultValidator() != 0 && ref->getResultValidator()->validate(obj) == false) {
                          return RefOp::MOVE;
                        }
                        RefOp ret = handler(*index, obj, parent);
                        *index++;
                        if (isMove(ret)) result = RefOp::MOVE;
                        else result = RefOp::STOP;
                        return ret;
                      });
  } else {
    ++qualifier;
    ref->forEachValue(this->dataProvider, source, [=,&result](Int i, IdentifiableObject *innerSrc)->RefOp {
                        // Prepare innerSrc and innerParent
                        IdentifiableObject *innerParent = parent;
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
                        RefOp ret = this->forEach(qualifier, innerSrc, handler,
                                                  parentTypeInfo, innerParent, index);
                        result = ret;
                        return ret;
                      });
  }
  return result;
}

} } // namespace
