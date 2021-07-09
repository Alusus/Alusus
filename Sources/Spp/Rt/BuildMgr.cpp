/**
 * @file Spp/Rt/BuildMgr.cpp
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
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

void BuildMgr::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->dumpLlvmIrForElement,
    &this->buildObjectFileForElement,
    &this->raiseBuildNotice
  });
}


void BuildMgr::initBindings()
{
  this->dumpLlvmIrForElement = &BuildMgr::_dumpLlvmIrForElement;
  this->buildObjectFileForElement = &BuildMgr::_buildObjectFileForElement;
  this->raiseBuildNotice = &BuildMgr::_raiseBuildNotice;
}


void BuildMgr::initializeRuntimePointers(CodeGen::GlobalItemRepo *globalItemRepo, BuildMgr *buildMgr)
{
  globalItemRepo->addItem(S("!Spp.buildMgr"), sizeof(void*), &buildMgr);
  globalItemRepo->addItem(S("Spp_BuildMgr_dumpLlvmIrForElement"), (void*)&BuildMgr::_dumpLlvmIrForElement);
  globalItemRepo->addItem(S("Spp_BuildMgr_buildObjectFileForElement"), (void*)&BuildMgr::_buildObjectFileForElement);
  globalItemRepo->addItem(S("Spp_BuildMgr_raiseBuildNotice"), (void*)&BuildMgr::_raiseBuildNotice);
}


//==============================================================================
// Operations

void BuildMgr::_dumpLlvmIrForElement(TiObject *self, TiObject *element)
{
  PREPARE_SELF(buildMgr, BuildMgr);
  buildMgr->buildManager->dumpLlvmIrForElement(element, buildMgr->noticeStore, buildMgr->parser);
}


Bool BuildMgr::_buildObjectFileForElement(
  TiObject *self, TiObject *element, Char const *objectFilename, Char const *targetTriple
) {
  PREPARE_SELF(buildMgr, BuildMgr);
  return buildMgr->buildManager->buildObjectFileForElement(
    element, objectFilename, targetTriple, buildMgr->noticeStore, buildMgr->parser
  );
}


void BuildMgr::_raiseBuildNotice(
  TiObject *self, Char const *code, Int severity, TiObject *astNode
) {
  PREPARE_SELF(buildMgr, BuildMgr);
  buildMgr->noticeStore->add(newSrdObj<Core::Notices::GenericNotice>(
    code, severity, Core::Data::Ast::findSourceLocation(astNode)
  ));
  buildMgr->parser->flushApprovedNotices();
}

} // namespace
