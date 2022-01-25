/**
 * @file Spp/Rt/GrammarMgr.cpp
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
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

void GrammarMgr::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->addCustomCommand,
    &this->addCustomGrammar
  });
}


void GrammarMgr::initBindings()
{
  this->addCustomCommand = &GrammarMgr::_addCustomCommand;
  this->addCustomGrammar = &GrammarMgr::_addCustomGrammar;
}


void GrammarMgr::initializeRuntimePointers(CodeGen::GlobalItemRepo *globalItemRepo, GrammarMgr *grammarMgr)
{
  globalItemRepo->addItem(S("!Spp.grammarMgr"), sizeof(void*), &grammarMgr);
  globalItemRepo->addItem(S("Spp_GrammarMgr_addCustomCommand"), (void*)&GrammarMgr::_addCustomCommand);
  globalItemRepo->addItem(S("Spp_GrammarMgr_addCustomGrammar"), (void*)&GrammarMgr::_addCustomGrammar);
}


//==============================================================================
// Operations

void GrammarMgr::_addCustomCommand(
  TiObject *self, Char const *qualifier, TiObject *ast, ParsingHandlerFunc func
) {
  PREPARE_SELF(grammarMgr, GrammarMgr);
  grammarMgr->grammarFactory->createCustomCommand(qualifier, ast, func, grammarMgr->rootManager->getNoticeStore());
  grammarMgr->rootManager->flushNotices();
}


Bool GrammarMgr::_addCustomGrammar(
  TiObject *self, Char const *qualifier, Char const *baseQualifier, TiObject *ast
) {
  PREPARE_SELF(grammarMgr, GrammarMgr);
  auto result = grammarMgr->grammarFactory->createCustomGrammar(
    qualifier, baseQualifier, ast, grammarMgr->rootManager->getNoticeStore()
  );
  grammarMgr->rootManager->flushNotices();
  return result;
}

} // namespace
