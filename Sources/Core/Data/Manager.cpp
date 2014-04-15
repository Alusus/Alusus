/**
 * @file Core/Data/Manager.cpp
 * Contains the implementation of class Core::Data::Manager.
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

void Manager::setValue(Reference *ref, const SharedPtr<IdentifiableObject> &obj)
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::Manager::setValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   ref->getScope().val);
  }
  this->server.set(ref->getSegment().get(), this->root.get(), obj);

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


void Manager::setValue(const Char *qualifier, const SharedPtr<IdentifiableObject> &obj)
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::Manager::setValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   scope.val);
  }
  this->rawServer.set(qualifier, this->root.get(), obj);

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


void Manager::removeValue(Reference *ref)
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::Manager::removeValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   ref->getScope().val);
  }
  this->server.remove(ref->getSegment().get(), this->root.get());
}


void Manager::removeValue(const Char *qualifier)
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::Manager::removeValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   scope.val);
  }
  this->rawServer.remove(qualifier, this->root.get());
}


const SharedPtr<IdentifiableObject>& Manager::getValue(Reference *ref)
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::Manager::getValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   ref->getScope().val);
  }
  return this->server.get(ref->getSegment().get(), this->root.get());
}


const SharedPtr<IdentifiableObject>& Manager::getValue(const Char *qualifier)
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::Manager::getValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   scope.val);
  }
  return this->rawServer.get(qualifier, this->root.get());
}


void Manager::getValue(Reference *ref, SharedPtr<IdentifiableObject> &retVal,
                       SharedPtr<Module> &retModule)
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::Manager::getValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   ref->getScope().val);
  }
  this->server.get(ref->getSegment().get(), this->root.get(), retVal, retModule);
}


void Manager::getValue(const Char *qualifier, SharedPtr<IdentifiableObject> &retVal,
                       SharedPtr<Module> &retModule)
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::Manager::getValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   scope.val);
  }
  this->rawServer.get(qualifier, this->root.get(), retVal, retModule);
}


void Manager::setId(IdentifiableObject *obj, const Char *id)
{
  IdOwner *ido = ii_cast<IdOwner>(obj);
  if (ido != 0) ido->setId(ID_GENERATOR->getId(id));
  this->setChildIds(obj, id);
}


void Manager::setChildIds(IdentifiableObject *obj, const Char *id)
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

IdentifiableObject* Manager::getPlainValue(Reference *ref) const
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::Manager::getPlainValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   ref->getScope().val);
  }
  return this->server.getPlain(ref->getSegment().get(), this->root.get());
}


void Manager::getPlainValue(Reference *ref, IdentifiableObject *&retVal, Module *&retModule) const
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::Manager::getPlainValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   ref->getScope().val);
  }
  return this->server.getPlain(ref->getSegment().get(), this->root.get(), retVal, retModule);
}


IdentifiableObject* Manager::getPlainValue(const Char *qualifier) const
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::Manager::getPlainValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   scope.val);
  }
  return this->rawServer.getPlain(qualifier, this->root.get());
}


void Manager::getPlainValue(const Char *qualifier, IdentifiableObject *&retVal, Module *&retModule) const
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::Manager::getPlainValue"),
                                   STR("Reference must be of type ROOT or UNKNOWN."),
                                   scope.val);
  }
  return this->rawServer.getPlain(qualifier, this->root.get(), retVal, retModule);
}


Bool Manager::tryGetPlainValue(Reference *ref, IdentifiableObject *&retVal) const
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) return false;
  return this->server.tryGetPlain(ref->getSegment().get(), this->root.get(), retVal);
}


Bool Manager::tryGetPlainValue(Reference *ref, IdentifiableObject *&retVal, Module *&retModule) const
{
  if (ref->getScope() != ReferenceScope::ROOT && ref->getScope() != ReferenceScope::UNKNOWN) return false;
  return this->server.tryGetPlain(ref->getSegment().get(), this->root.get(), retVal, retModule);
}


Bool Manager::tryGetPlainValue(const Char *qualifier, IdentifiableObject *&retVal) const
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) return false;
  return this->rawServer.tryGetPlain(qualifier, this->root.get(), retVal);
}


Bool Manager::tryGetPlainValue(const Char *qualifier, IdentifiableObject *&retVal, Module *&retModule) const
{
  ReferenceScope scope = ReferenceParser::parseQualifierScope(qualifier);
  if (scope != ReferenceScope::ROOT && scope != ReferenceScope::UNKNOWN) return false;
  return this->rawServer.tryGetPlain(qualifier, this->root.get(), retVal, retModule);
}

} } // namespace
