/**
 * @file Tests/CoreTests/helpers.cpp
 * Contains the definition of different helper functions and classes that are
 * used by unit tests.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core_tests.h"

namespace Tests { namespace CoreTests
{

using namespace std;

/**
 * Search the given tree recursively for the data of the given production id.
 *
 * @param prodId The id of the production to search for.
 * @param ptr The root of the tree.
 * @param result A reference to an array to contain all found results.
 * @param index If ptr is a list object, this index will specify at what element
 *              of that list to search. If this is -1 the entire list will be
 *              searched.
 */
void findProdData(Word prodId, SharedPtr<TiObject> ptr,
                  vector<SharedPtr<TiObject> > &result, Int index)
{
  if (ptr == 0) return;

  Data::Ast::MetadataHolder *metadata = ptr->getInterface<Data::Ast::MetadataHolder>();
  if (metadata == 0) return;

  if (metadata->getProdId() == prodId) {
    result.push_back(ptr);
  }

  // Print the data itself.
  if (ptr->isDerivedFrom<Data::Ast::List>()) {
    if (index != -1) {
      findProdData(prodId, ptr.s_cast_get<Data::Ast::List>()->getShared(index), result);
    } else {
      for (Word i = 0; i < ptr.s_cast_get<Data::Ast::List>()->getCount(); ++i) {
        findProdData(prodId, ptr.s_cast_get<Data::Ast::List>()->getShared(i), result);
      }
    }
  } else if (ptr->isDerivedFrom<Data::Ast::Route>()) {
    findProdData(prodId, ptr.s_cast_get<Data::Ast::Route>()->getData(), result);
  }
}


/**
 * Search the given tree recursively for the token with the given id and/or
 * text.
 *
 * @param tokenId The id of the token to search for, or UNKNOWN_ID to ignore
 *                 the token id.
 * @param text The text of the token, or 0 to ignore the token text.
 * @param ptr The root of the tree.
 * @param result A reference to an array to contain all found results.
 * @param index If ptr is a list object, this index will specify at what element
 *              of that list to search. If this is -1 the entire list will be
 *              searched.
 */
void findToken(Word tokenId, Char const *text, SharedPtr<TiObject> ptr,
               vector<SharedPtr<TiObject> > &result, Int index)
{
  if (ptr == 0) return;

  // Print the data itself.
  if (ptr->isDerivedFrom<Data::Ast::List>()) {
    if (index != -1) {
      findToken(tokenId, text, ptr.s_cast_get<Data::Ast::List>()->getShared(index), result);
    } else {
      for (Word i = 0; i < ptr.s_cast_get<Data::Ast::List>()->getCount(); ++i) {
        findToken(tokenId, text, ptr.s_cast_get<Data::Ast::List>()->getShared(i), result);
      }
    }
  } else if (ptr->isDerivedFrom<Data::Ast::Route>()) {
    findToken(tokenId, text, ptr.s_cast_get<Data::Ast::Route>()->getData(), result);
  } else if (ptr->isDerivedFrom<Data::Ast::Token>()) {
    // Print the token type.
    SharedPtr<Data::Ast::Token> token = ptr.s_cast<Data::Ast::Token>();
    if ((tokenId == 0 || tokenId == token->getId()) &&
        (text == 0 || token->getText().compare(text) == 0)) {
      result.push_back(token);
    }
  }
}


/**
 * Check whether the provided pointer has empty data. Data is considered empty
 * if:<br>
 * - ptr is a list that has 0 elements.<br>
 * - ptr is a route that has no child data.<br>
 * or<br>
 * - ptr is a token that has UNKNOWN_ID.
 */
Bool isEmpty(SharedPtr<TiObject> ptr)
{
  if (ptr == 0) {
    return false;
  } else if (ptr->isDerivedFrom<Data::Ast::List>()) {
    return ptr.s_cast_get<Data::Ast::List>()->getCount() == 0;
  } else if (ptr->isDerivedFrom<Data::Ast::Route>()) {
    return ptr.s_cast_get<Data::Ast::Route>()->getData() == 0;
  } else if (ptr->isDerivedFrom<Data::Ast::Token>()) {
    return ptr.s_cast_get<Data::Ast::Token>()->getId() == UNKNOWN_ID;
  }
  return false;
}

} } // namespace
