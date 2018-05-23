/**
 * @file Core/Data/Grammar/grammar.cpp
 * Contains the global implementations of Data::Grammar namespace's declarations.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Grammar
{

SharedPtr<Reference> createReference(Char const *qualifier, std::vector<SharedPtr<Reference>> *referenceCache)
{
  VALIDATE_NOT_NULL(qualifier);

  SharedPtr<Reference> ref, prevRef;

  Int cacheIndex = 0;
  auto addSegment = [=, &ref, &prevRef, &cacheIndex](Int begin, Word size)
  {
    SharedPtr<Reference> seg;
    if (referenceCache != 0) {
      if (cacheIndex >= referenceCache->size()) {
        referenceCache->push_back(std::make_shared<Reference>());
      }
      seg = referenceCache->at(cacheIndex);
      ++cacheIndex;
    } else {
      seg = std::make_shared<Reference>(qualifier + begin, size);
    }
    seg->setKey(qualifier + begin, size);
    seg->setNext(SharedPtr<Reference>::null);
    if (prevRef != 0) {
      prevRef->setNext(seg);
    }
    prevRef = seg;
    if (ref == 0) {
      ref = seg;
    }
  };

  Int start = 0, count = 0;
  while ((count = SBSTR(qualifier + start).findPos('.')) >= 0) {
    addSegment(start, count);
    start += count + 1;
  }
  addSegment(start, SBSTR(qualifier).size() - start);
  ASSERT(ref != 0);

  return ref;
}


Module* getGrammarRoot(ListContaining<TiObject> *rootScope, Bool createIfMissing)
{
  VALIDATE_NOT_NULL(rootScope);

  // Find grammar root.
  Module *grammarRoot = 0;
  for (Int i = 0; i < rootScope->getElementCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(rootScope->getElement(i));
    if (def != 0 && def->getName() == STR("grammar")) {
      grammarRoot = def->getTarget().ti_cast_get<Module>();
      break;
    }
  }

  // Create a new grammar root if no one is found.
  if (grammarRoot == 0 && createIfMissing) {
    auto grammarDef = Data::Ast::Definition::create();
    grammarDef->setName(STR("grammar"));
    grammarDef->setTarget(Module::create({}));
    rootScope->addElement(grammarDef.get());
    grammarRoot = grammarDef->getTarget().s_cast_get<Module>();
  }

  return grammarRoot;
}


Bool matchCharGroup(WChar ch, CharGroupUnit *unit)
{
  ASSERT(unit);

  if (unit->isA<SequenceCharGroupUnit>()) {
    SequenceCharGroupUnit *u = static_cast<SequenceCharGroupUnit*>(unit);
    if (u->getStartCode() == 0 && u->getEndCode() == 0) {
      throw EXCEPTION(GenericException, STR("Sequence char group unit is not configured yet."));
    }
    if (ch >= u->getStartCode() && ch <= u->getEndCode()) return true;
    else return false;
  } else if (unit->isA<RandomCharGroupUnit>()) {
    RandomCharGroupUnit *u = static_cast<RandomCharGroupUnit*>(unit);
    if (u->getCharList() == 0) {
      throw EXCEPTION(GenericException, STR("Random char group unit is not configured yet."));
    }
    for (Int i = 0; i < u->getCharListSize(); i++) {
      if (u->getCharList()[i] == ch) return true;
    }
    return false;
  } else if (unit->isA<UnionCharGroupUnit>()) {
    UnionCharGroupUnit *u = static_cast<UnionCharGroupUnit*>(unit);
    if (u->getCharGroupUnits()->size() == 0) {
      throw EXCEPTION(GenericException, STR("Union char group unit is not configured yet."));
    }
    for (Int i = 0; i < static_cast<Int>(u->getCharGroupUnits()->size()); i++) {
      if (matchCharGroup(ch, u->getCharGroupUnits()->at(i).get()) == true) {
        return true;
      }
    }
    return false;
  } else if (unit->isA<InvertCharGroupUnit>()) {
    InvertCharGroupUnit *u = static_cast<InvertCharGroupUnit*>(unit);
    if (u->getChildCharGroupUnit() == 0) {
      throw EXCEPTION(GenericException, STR("Invert char group unit is not configured yet."));
    }
    if (matchCharGroup(ch, u->getChildCharGroupUnit().get()) == true) {
      return false;
    } else {
      return true;
    }
  } else {
    throw EXCEPTION(GenericException, STR("Invalid char group type."));
  }
  return false; // just to prevent warnings
}

} // namespace
