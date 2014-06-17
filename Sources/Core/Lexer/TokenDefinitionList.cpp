/**
 * @file Core/Lexer/TokenDefinitionList.cpp
 * Contains the implementation of class Core::Lexer::TokenDefinitionList.
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
 * The token definitions are sorted by token id. This function will
 * insert the new token at the appropriate location within the list. If the
 * token id already exists in the list an exception will be thrown.
 *
 * @return The index at which the definition is inserted.
 */
Int TokenDefinitionList::add(const SharedPtr<TokenDefinition> &ptr)
{
  // Validate given token definition.
  if (ptr->getId() == UNKNOWN_ID) {
    throw InvalidArgumentException(STR("ptr->getId()"), STR("Core::Lexer::TokenDefinitionList::add"),
                                   STR("Definition ID is NULL."));
  }
  if (ptr->getId() & FLAGS_BITS_MASK) {
    throw InvalidArgumentException(STR("ptr->getId()"), STR("Core::Lexer::TokenDefinitionList::add"),
                                   STR("Definition ID is using a reserved flag."));
  }
  if (ptr->getFormulaHead() == 0) {
    throw InvalidArgumentException(STR("ptr->getFormulaHead()"), STR("Core::Lexer::TokenDefinitionList::add"));
  }
  // Add the object at the appropriate location in the list.
  if (this->list.empty()) {
    this->list.push_back(ptr);
    return 0;
  } else if (ptr->getId() == this->list.front()->getId() || ptr->getId() == this->list.back()->getId()) {
    throw GeneralException(STR("A token with the same token id already exists."),
                           STR("Core::Lexer::TokenDefinitionList::add"));
  } else if (ptr->getId() < this->list.front()->getId()) {
    this->list.insert(this->list.begin(), ptr);
    return 0;
  } else if (ptr->getId() > this->list.back()->getId()) {
    this->list.push_back(ptr);
    return this->list.size()-1;
  } else {
    Int pos = this->findIndex(ptr->getId(), 0, this->list.size()-1);
    if (ptr->getId() == this->list[pos]->getId()) {
      throw GeneralException(STR("A token with the same token id already exists."),
                             STR("Core::Lexer::TokenDefinitionList::add"));
    } else {
      this->list.insert(this->list.begin()+pos, ptr);
      return pos;
    }
  }
}


/**
 * The token definitions are sorted by token id. This function will
 * insert the new token at the appropriate location within the list. If the
 * token id already exists in the list an exception will be thrown.
 *
 * @param id The id to use for the new definition.
 * @param formulaHead A pointer to the definition's term tree head.
 * @param internal Specifies whether this definition is for an internal token.
 *                 Internal tokens are tokens that come part of other tokens and
 *                 never represent complete tokens by themselves.
 * @return The index at which the new definition is inserted.
 */
Int TokenDefinitionList::add(Word id, const SharedPtr<Term> &formulaHead, Bool internal)
{
  if (id == UNKNOWN_ID) {
    throw InvalidArgumentException(STR("id"), STR("Core::Lexer::TokenDefinitionList::add"),
                                   STR("Definition ID is NULL."));
  }
  if (id & FLAGS_BITS_MASK) {
    throw InvalidArgumentException(STR("id"), STR("Core::Lexer::TokenDefinitionList::add"),
                                   STR("Definition ID is using a reserved flag."));
  }
  if (formulaHead == 0) {
    throw InvalidArgumentException(STR("formulaHead"), STR("Core::Lexer::TokenDefinitionList::add"));
  }
  return this->add(SharedPtr<TokenDefinition>(new TokenDefinition(id, formulaHead, internal)));
}


/**
 * The function first searches the list for a definition of a const token with
 * the provided string. If a definition is found, the ID of that definition is
 * returned. Otherwise, a new definition is created with an ID obtained from
 * Data::IdGenerator and the new ID is returned.
 */
Word TokenDefinitionList::addConstToken(Char const *str)
{
  // First search for an already available const token.
  for (Int i = 0; i < static_cast<Int>(this->list.size()); i++) {
    Term *term = this->list[i]->getFormulaHead().get();
    if (term->getType() == TermType::CONST) {
      if (static_cast<ConstTerm*>(term)->getMatchString().compare(str) == 0) {
        return this->list[i]->getId();
      }
    }
  }
  // Not found, so create a new token definition.
  static Int *counter=0;
  if (counter == 0) {
    counter = reinterpret_cast<Int*>
              (GLOBAL_STORAGE->getObject(STR("Core::Lexer::TokenDefinitionList::addConstToken")));
    if (counter == 0) {
      counter = new Int(0);
      GLOBAL_STORAGE->setObject(STR("Core::Lexer::TokenDefinitionList::addConstToken"),
                                reinterpret_cast<void*>(counter));
    }
  }
  ++*counter;
  StrStream idDesc;
  idDesc << STR("CONSTANT_") << *counter;
  Word id = Data::IdGenerator::getSingleton()->getId(idDesc.str().c_str());
  this->add(id, SharedPtr<Term>(new ConstTerm(str)));
  return id;
}


/**
 * This function will remove the token from the list only if the id is
 * not the root id and the token with that id is not in use by anyone.
 * Checking whether the token is in use is done by emitting the
 * tokenInUseInquirer signal. The check also guarantees that the
 * token is not in use by any reference term in this list itself.
 *
 * @sa tokenInUseInquirer
 */
void TokenDefinitionList::remove(Int index)
{
  // Validation.
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Lexer::TokenDefinitionList::remove"),
                                   STR("Index out of range."));
  }
  if (this->tokenInUseInquirer.emitAny(this->list[index]->getId())) {
    throw InvalidArgumentException(STR("index"), STR("Core::Lexer::TokenDefinitionList::remove"),
                                   STR("Token is still in use (by somebody else)."));
  }
  // Clear references.
  TokenDefinitionList::clearTreeReferences(this->list[index]->getFormulaHead().get());
  this->clearAllReferences(this->list[index]->getId());
  // Remove the item from the list.
  this->list.erase(this->list.begin()+index);
}


/**
 * If the given id is not found in the list nothing will happen and no
 * exception will be thrown. If the token is still in use by anyone, an
 * exception will be thrown. Checking whether it's in use is done by emitting
 * the tokenInUseInquirer signal. The check also guarantees that the
 * token is not in use by any reference term in this list itself.
 *
 * @sa tokenInUseInquirer
 */
void TokenDefinitionList::removeById(Word id)
{
  // Validation.
  if (this->tokenInUseInquirer.emitAny(id)) {
    throw InvalidArgumentException(STR("id"), STR("Core::Lexer::TokenDefinitionList::removeById"),
                                   STR("Token is still in use (by somebody else)."));
  }
  // Find the item to remove.
  Int index = this->findIndex(id);
  // Clear references.
  TokenDefinitionList::clearTreeReferences(this->list[index]->getFormulaHead().get());
  this->clearAllReferences(this->list[index]->getId());
  // Remove the item.
  if (index != -1) this->remove(index);
}


/**
 * Before removing any token this method loops through all tokens to
 * make sure none of them is in use by anyone. This is done by emitting the
 * tokenInUseInquirer signal. If any token is still in use an exception
 * will be thrown.
 *
 * @sa tokenInUseInquirer
 */
void TokenDefinitionList::release()
{
  for (Int i = 0; i < static_cast<Int>(this->list.size()); ++i) {
    if (this->tokenInUseInquirer.emitAny(this->list[i]->getId())) {
      throw InvalidArgumentException(STR("Some token is still in use."),
                                     STR("Core::Lexer::TokenDefinitionList::release"));
    }
  }
  this->list.clear();
}


/**
 * Loop through all tokens to clear any references for them to the given
 * token id. Skip the token with the same token id. Notice that clearing the
 * reference only involves reseting the reference pointer (which is actually
 * used for caching purposes) and doesn't involve reseting the id itself.
 */
void TokenDefinitionList::clearAllReferences(Word tokenId)
{
  for (Int i = 0; i < static_cast<Int>(this->list.size()); ++i) {
    // Skip the token with the same token id.
    if (this->list[i]->getId() == tokenId) continue;
    // Now check this token tree.
    TokenDefinitionList::clearTreeReferences(this->list[i]->getFormulaHead().get(), tokenId);
  }
}


/**
 * Recursively search the given tree for any reference terms and reset their
 * referenced pointer. Search is filtered by the given tokenId. Notice
 * that only the reference's pointer will be reset, not the ID.
 *
 * @param tokenId If this param is equal to UNKNOWN_ID, all references to
 *                 any id will be reset. If this is not equal to UNKNOWN_ID
 *                 only references to that specific id will be reset.
 */
void TokenDefinitionList::clearTreeReferences(Term *term, Word tokenId)
{
  switch (term->getType().val) {
    case TermType::REFERENCE: {
      ReferenceTerm *t = static_cast<ReferenceTerm*>(term);
      if (tokenId == UNKNOWN_ID || tokenId == t->getReferenceId()) t->setReferenceHead(0);
      break;
    }

    case TermType::CONST:
    case TermType::CHAR_GROUP:
      break;

    case TermType::OPTIONAL:
      TokenDefinitionList::clearTreeReferences(
            static_cast<OptionalTerm*>(term)->getChildTerm().get(), tokenId);
      break;

    case TermType::DUPLICATE:
      TokenDefinitionList::clearTreeReferences(
            static_cast<DuplicateTerm*>(term)->getChildTerm().get(), tokenId);
      break;

    case TermType::ALTERNATIVE:
    case TermType::CONCAT: {
      Term *childTerm = static_cast<ListTerm*>(term)->getChildTermsHead().get();
      while (childTerm != 0) {
        TokenDefinitionList::clearTreeReferences(childTerm, tokenId);
        childTerm = childTerm->getNextTerm().get();
      }
      break;
    }

    default:
      // This should not be reachable.
      ASSERT(false);
  }
}


/**
 * Searches the entire list for the given id using binary search algorithm.
 *
 * @return The index of the given id, or -1 if that id couldn't be found.
 */
Int TokenDefinitionList::findIndex(Word id)
{
  if (id == UNKNOWN_ID) {
    throw InvalidArgumentException(STR("id"), STR("Core::Lexer::TokenDefinitionList::findIndex"));
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
Int TokenDefinitionList::findIndex(Word id, Int startPos, Int endPos)
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


/**
 * Loop through all char groups to check if any of them has a unit that
 * references the given char group id.
 *
 * @return true if the char group is in use by anyone.
 */
Bool TokenDefinitionList::isCharGroupInUse(Word charGroupId)
{
  for (Int i = 0; i < static_cast<Int>(this->list.size()); ++i) {
    if (TokenDefinitionList::_isCharGroupInUse(this->list[i]->getFormulaHead().get(),
                                                   charGroupId)) {
      return true;
    }
  }
  return false;
}


/**
 * Recursively searches the entire term tree to see if it contains a
 * CharGroupTerm that refers to the same char group id as the given one.
 *
 * @return true if there is any reference to that given char group,
 *              false otherwise.
 */
Bool TokenDefinitionList::_isCharGroupInUse(const Term *term, Word charGroupId)
{
  switch (term->getType().val) {
    case TermType::CHAR_GROUP:
      if (static_cast<const CharGroupTerm*>(term)->getCharGroupId() == charGroupId) return true;
      else return false;

    case TermType::CONST:
    case TermType::REFERENCE:
      return false;

    case TermType::OPTIONAL:
      return TokenDefinitionList::_isCharGroupInUse(
            static_cast<const OptionalTerm*>(term)->getChildTerm().get(), charGroupId);

    case TermType::DUPLICATE:
      return TokenDefinitionList::_isCharGroupInUse(
            static_cast<const DuplicateTerm*>(term)->getChildTerm().get(), charGroupId);

    case TermType::ALTERNATIVE:
    case TermType::CONCAT: {
      Term *childTerm = static_cast<const ListTerm*>(term)->getChildTermsHead().get();
      while (childTerm != 0) {
        if (TokenDefinitionList::_isCharGroupInUse(childTerm, charGroupId)) return true;
        childTerm = childTerm->getNextTerm().get();
      }
      return false;
    }

    default:
      // This should not be reachable.
      ASSERT(false);
  }
  return false;
}

} } // namespace
