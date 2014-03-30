/**
 * @file Core/Lexer/CharGroupDefinitionList.cpp
 * Contains the implementation of class Lexer::CharGroupDefinitionList.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Lexer
{

//==============================================================================
// Member Functions

/**
 * The char group definitions are sorted by char group id. This function will
 * insert the new char group at the appropriate location within the list. If the
 * char group id already exists in the list an exception will be thrown.
 *
 * @return The index at which the definition is inserted.
 */
Int CharGroupDefinitionList::add(const SharedPtr<CharGroupDefinition> &ptr)
{
  // Validate given char group definition.
  if (ptr->getId() == UNKNOWN_ID) {
    throw InvalidArgumentException(STR("ptr->getId()"), STR("Core::Lexer::CharGroupDefinitionList::add"),
                                   STR("Definition ID is NULL."));
  }
  if (ptr->getId() & FLAGS_BITS_MASK) {
    throw InvalidArgumentException(STR("ptr->getId()"), STR("Core::Lexer::CharGroupDefinitionList::add"),
                                   STR("Definition ID is using a reserved flag."));
  }
  if (ptr->getCharGroupUnit() == 0) {
    throw InvalidArgumentException(STR("ptr->getCharGroupUnit()"),
                                   STR("Core::Lexer::CharGroupDefinitionList::add"));
  }
  // Add the object at the appropriate location in the list.
  if (this->list.empty()) {
    this->list.push_back(ptr);
    return 0;
  } else if (ptr->getId() == this->list.front()->getId() || ptr->getId() == this->list.back()->getId()) {
    throw GeneralException(STR("A char group with the same char group id already exists."),
                           STR("Core::Lexer::CharGroupDefinitionList::add"));
  } else if (ptr->getId() < this->list.front()->getId()) {
    this->list.insert(this->list.begin(), ptr);
    return 0;
  } else if (ptr->getId() > this->list.back()->getId()) {
    this->list.push_back(ptr);
    return this->list.size()-1;
  } else {
    Int pos = this->findIndex(ptr->getId(), 0, this->list.size()-1);
    if (ptr->getId() == this->list[pos]->getId()) {
      throw GeneralException(STR("A char group with the same char group id already exists."),
                             STR("Core::Lexer::CharGroupDefinitionList::add"));
    } else {
      this->list.insert(this->list.begin()+pos, ptr);
      return pos;
    }
  }
}


/**
 * The char group definitions are sorted by char group id. This function will
 * insert the new char group at the appropriate location within the list. If the
 * char group id already exists in the list an exception will be thrown.
 *
 * @return The index at which the new definition is inserted.
 */
Int CharGroupDefinitionList::add(Word id, const SharedPtr<CharGroupUnit> &unit)
{
  if (id == UNKNOWN_ID) {
    throw InvalidArgumentException(STR("id"), STR("Core::Lexer::CharGroupDefinitionList::add"),
                                   STR("Definition ID is NULL."));
  }
  if (id & FLAGS_BITS_MASK) {
    throw InvalidArgumentException(STR("id"), STR("Core::Lexer::CharGroupDefinitionList::add"),
                                   STR("Definition ID is using a reserved flag."));
  }
  if (unit == 0) {
    throw InvalidArgumentException(STR("unit"), STR("Core::Lexer::CharGroupDefinitionList::add"));
  }
  return this->add(SharedPtr<CharGroupDefinition>(new CharGroupDefinition(id, unit)));
}


/**
 * This function will remove the char group from the list only if the id is
 * not the root id and the char group with that id is not in use by anyone.
 * Checking whether the char group is in use is done by emitting the
 * charGroupInUseInquirer signal. The check also guarantees that the
 * char group is not in use by any other char group unit in this list itself.
 *
 * @sa charGroupInUseInquirer
 */
void CharGroupDefinitionList::remove(Int index)
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Lexer::CharGroupDefinitionList::remove"),
                                   STR("Index out of range."));
  }
  if (this->charGroupInUseInquirer.emitAny(this->list[index]->getId())) {
    throw InvalidArgumentException(STR("index"), STR("Core::Lexer::CharGroupDefinitionList::remove"),
                                   STR("Char group is still in use (by somebody else)."));
  }
  this->list.erase(this->list.begin()+index);
}


/**
 * If the given id is not found in the list nothing will happen and no
 * exception will be thrown. If the char group is still in use by anyone, an
 * exception will be thrown. Checking whether it's in use is done by emitting
 * the charGroupInUseInquirer signal. The check also guarantees that the
 * char group is not in use by any other char group unit in this list itself.
 *
 * @sa charGroupInUseInquirer
 */
void CharGroupDefinitionList::removeById(Word id)
{
  if (this->charGroupInUseInquirer.emitAny(id)) {
    throw InvalidArgumentException(STR("id"), STR("Core::Lexer::CharGroupDefinitionList::removeById"),
                                   STR("Char group is still in use (by somebody else)."));
  }
  Int index = this->findIndex(id);
  if (index != -1) this->remove(index);
}


/**
 * Before removing any char group this method loops through all char groups to
 * make sure none of them is in use by anyone. This is done by emitting the
 * charGroupInUseInquirer signal. If any char group is still in use an exception
 * will be thrown.
 *
 * @sa charGroupInUseInquirer
 */
void CharGroupDefinitionList::release()
{
  for (Int i = 0; i < static_cast<Int>(this->list.size()); ++i) {
    if (this->charGroupInUseInquirer.emitAny(this->list[i]->getId())) {
      throw InvalidArgumentException(STR("Some char group is still in use."),
                                     STR("Core::Lexer::CharGroupDefinitionList::release"));
    }
  }
  this->list.clear();
}


/**
 * Searches the entire list for the given id using binary search algorithm.
 * @return The index of the given id, or -1 if that id couldn't be found.
 */
Int CharGroupDefinitionList::findIndex(Word id)
{
  if (id == UNKNOWN_ID) {
    throw InvalidArgumentException(STR("id"), STR("Core::Lexer::CharGroupDefinitionList::findIndex"));
  }
  if (this->list.empty() || id < this->list.front()->getId() || id > this->list.back()->getId()) return -1;
  else if (id == this->list.front()->getId()) return 0;
  else if (id == this->list.back()->getId()) return this->list.size()-1;
  else {
    Int pos = this->findIndex(id, 0, this->list.size()-1);
    if (id == this->list[pos]->getId()) return pos;
    else return -1;
  }
}


/**
 * A recursive function that uses binary search algorithm to search a specific
 * portion of the list for the given id.
 *
 * @return The index at which the id exists. If the id couldn't be found the
 *         index at which it should be inserted will be returned instead.
 */
Int CharGroupDefinitionList::findIndex(Word id, Int startPos, Int endPos)
{
  ASSERT(id != UNKNOWN_ID);
  ASSERT(endPos >= startPos && startPos >= 0);
  if (startPos == endPos) return startPos;
  else {
    Int midPos = (startPos + endPos) / 2;
    Word midId = this->list[midPos]->getId();
    if (midId == id) return midPos;
    else if (midId > id) return this->findIndex(id, startPos, midPos);
    else return this->findIndex(id, midPos+1, endPos);
  }
}

} } // namespace
