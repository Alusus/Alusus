/**
 * @file Core/Data/DataStore.cpp
 * Contains the implementation of class Core::Data::DataStore.
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

//==============================================================================
// Member Functions

void DataStore::setValue(Reference *ref, const SharedPtr<IdentifiableObject> &obj)
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::DataStore::setValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   ref->getScope().val);
  }
  this->seeker.set(ref->getSegment().get(), this->root.get(), obj);

  // TODO: We can remove this part if/when we start handling IDs in definition signals.
  // Set the id for the new object.
  Str qualifierStr = ReferenceParser::getQualifier(ref);
  const Char *qualifier = qualifierStr.c_str();
  if (compareStr(qualifier, STR("root."), 5) == 0) qualifier += 5;
  this->setId(obj.get(), qualifier);

  Initializable *initializable = obj->getInterface<Initializable>();
  if (initializable != 0) {
    SharedPtr<IdentifiableObject> dummy;
    SharedPtr<Module> module;
    this->getValue(ref, dummy, module);
    initializable->initialize(this, module);
  }
}


void DataStore::setValue(const Char *qualifier, const SharedPtr<IdentifiableObject> &obj)
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::DataStore::setValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   scope.val);
  }
  this->rawSeeker.set(qualifier, this->root.get(), obj);

  // TODO: We can remove this part if/when we start handling IDs in definition signals.
  // Set the id for the new object.
  if (compareStr(qualifier, STR("root."), 5) == 0) qualifier += 5;
  this->setId(obj.get(), qualifier);

  Initializable *initializable = obj->getInterface<Initializable>();
  if (initializable != 0) {
    SharedPtr<IdentifiableObject> dummy;
    SharedPtr<Module> module;
    this->getValue(qualifier, dummy, module);
    initializable->initialize(this, module);
  }
}


void DataStore::removeValue(Reference *ref)
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::DataStore::removeValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   ref->getScope().val);
  }
  this->seeker.remove(ref->getSegment().get(), this->root.get());
}


void DataStore::removeValue(const Char *qualifier)
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::DataStore::removeValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   scope.val);
  }
  this->rawSeeker.remove(qualifier, this->root.get());
}


const SharedPtr<IdentifiableObject>& DataStore::getValue(Reference *ref)
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::DataStore::getValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   ref->getScope().val);
  }
  return this->seeker.get(ref->getSegment().get(), this->root.get());
}


const SharedPtr<IdentifiableObject>& DataStore::getValue(const Char *qualifier)
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::DataStore::getValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   scope.val);
  }
  return this->rawSeeker.get(qualifier, this->root.get());
}


void DataStore::getValue(Reference *ref, SharedPtr<IdentifiableObject> &retVal,
                       SharedPtr<Module> &retModule)
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::DataStore::getValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   ref->getScope().val);
  }
  this->seeker.get(ref->getSegment().get(), this->root.get(), retVal, retModule);
}


void DataStore::getValue(const Char *qualifier, SharedPtr<IdentifiableObject> &retVal,
                       SharedPtr<Module> &retModule)
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::DataStore::getValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   scope.val);
  }
  this->rawSeeker.get(qualifier, this->root.get(), retVal, retModule);
}


void DataStore::setId(IdentifiableObject *obj, const Char *id)
{
  IdOwner *ido = ii_cast<IdOwner>(obj);
  if (ido != 0) ido->setId(ID_GENERATOR->getId(id));
  this->setChildIds(obj, id);
}


void DataStore::setChildIds(IdentifiableObject *obj, const Char *id)
{
  MapContainer *map; Container *list;
  if ((map = ii_cast<MapContainer>(obj)) != 0) {
    Str childId;
    for (Int i = 0; static_cast<Word>(i) < map->getCount(); ++i) {
      childId = id;
      childId += CHR('.');
      childId += map->getKey(i);
      this->setId(map->get(i).get(), childId.c_str());
    }
  } else if ((list = ii_cast<Container>(obj)) != 0) {
    for (Int i = 0; static_cast<Word>(i) < list->getCount(); ++i) {
      StrStream childId;
      childId << id << CHR('.') << i;
      this->setId(list->get(i).get(), childId.str().c_str());
    }
  }
}


//==============================================================================
// Provider Implementation

IdentifiableObject* DataStore::getPlainValue(Reference *ref) const
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::DataStore::getPlainValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   ref->getScope().val);
  }
  return this->seeker.getPlain(ref->getSegment().get(), this->root.get());
}


void DataStore::getPlainValue(Reference *ref, IdentifiableObject *&retVal, Module *&retModule) const
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::DataStore::getPlainValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   ref->getScope().val);
  }
  return this->seeker.getPlain(ref->getSegment().get(), this->root.get(), retVal, retModule);
}


IdentifiableObject* DataStore::getPlainValue(const Char *qualifier) const
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::DataStore::getPlainValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   scope.val);
  }
  return this->rawSeeker.getPlain(qualifier, this->root.get());
}


void DataStore::getPlainValue(const Char *qualifier, IdentifiableObject *&retVal, Module *&retModule) const
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::DataStore::getPlainValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   scope.val);
  }
  return this->rawSeeker.getPlain(qualifier, this->root.get(), retVal, retModule);
}


Bool DataStore::tryGetPlainValue(Reference *ref, IdentifiableObject *&retVal) const
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) return false;
  return this->seeker.tryGetPlain(ref->getSegment().get(), this->root.get(), retVal);
}


Bool DataStore::tryGetPlainValue(Reference *ref, IdentifiableObject *&retVal, Module *&retModule) const
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) return false;
  return this->seeker.tryGetPlain(ref->getSegment().get(), this->root.get(), retVal, retModule);
}


Bool DataStore::tryGetPlainValue(const Char *qualifier, IdentifiableObject *&retVal) const
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) return false;
  return this->rawSeeker.tryGetPlain(qualifier, this->root.get(), retVal);
}


Bool DataStore::tryGetPlainValue(const Char *qualifier, IdentifiableObject *&retVal, Module *&retModule) const
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) return false;
  return this->rawSeeker.tryGetPlain(qualifier, this->root.get(), retVal, retModule);
}

} } // namespace
