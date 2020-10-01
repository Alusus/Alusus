/**
 * @file Spp/Rt/AstMgr.cpp
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Rt
{

//==============================================================================
// Initialization Functions

void AstMgr::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->getModifierStrings
  });
}


void AstMgr::initBindings()
{
  this->getModifierStrings = &AstMgr::_getModifierStrings;
}


void AstMgr::initializeRuntimePointers(CodeGen::GlobalItemRepo *globalItemRepo, AstMgr *astMgr)
{
  globalItemRepo->addItem(S("!Spp.astMgr"), sizeof(void*), &astMgr);
  globalItemRepo->addItem(S("Spp_AstMgr_getModifierStrings"), (void*)&AstMgr::_getModifierStrings);
}


//==============================================================================
// Operations

Bool AstMgr::_getModifierStrings(
  TiObject *self, TiObject *element, Char const *modifierKwd, Char const **resultStrs[], Word *resultCount
) {
  PREPARE_SELF(astMgr, AstMgr);

  auto node = ti_cast<Core::Data::Node>(element);
  auto def = Core::Data::findOwner<Core::Data::Ast::Definition>(node);
  if (def == 0 || def->getModifiers() == 0 || def->getModifiers()->getCount() == 0) {
    *resultCount = 0;
    *resultStrs = 0;
    return true;
  }
  // Look for the modifier.
  for (Int i = 0; i < def->getModifiers()->getElementCount(); ++i) {
    auto modifier = ti_cast<Core::Data::Ast::ParamPass>(def->getModifiers()->getElement(i));
    if (modifier == 0) continue;
    auto identifier = modifier->getOperand().ti_cast_get<Core::Data::Ast::Identifier>();
    if (identifier != 0 && identifier->getValue() == modifierKwd && modifier->getParam() != 0) {
      // Found the requested modifier.
      Core::Basic::PlainList<TiObject> strList;
      auto strs = modifier->getParam().ti_cast_get<Core::Basic::Containing<TiObject>>();
      if (strs == 0) {
        strList.add(modifier->getParam().get());
        strs = &strList;
      }
      *resultCount = strs->getElementCount();
      *resultStrs = reinterpret_cast<Char const**>(malloc(sizeof(Char*) * (*resultCount)));
      for (Int j = 0; j < *resultCount; ++j) {
        auto str = ti_cast<Core::Data::Ast::StringLiteral>(strs->getElement(j));
        if (str == 0) {
          astMgr->noticeStore->add(newSrdObj<Spp::Notices::InvalidModifierDataNotice>(
            Core::Data::Ast::findSourceLocation(strs->getElement(j))
          ));
          astMgr->parser->flushApprovedNotices();
          free(*resultStrs);
          *resultStrs = 0;
          *resultCount = 0;
          return false;
        }
        (*resultStrs)[j] = str->getValue().get();
      }
      return true;
    }
  }
  *resultCount = 0;
  *resultStrs = 0;
  return true;
}

} // namespace
