/**
 * @file Spp/Rt/BuildMgr.cpp
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

void BuildMgr::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->dumpLlvmIrForElement,
    &this->buildObjectFileForElement
  });
}


void BuildMgr::initBindings()
{
  this->dumpLlvmIrForElement = &BuildMgr::_dumpLlvmIrForElement;
  this->buildObjectFileForElement = &BuildMgr::_buildObjectFileForElement;
}


void BuildMgr::initializeRuntimePointers(CodeGen::GlobalItemRepo *globalItemRepo, BuildMgr *buildMgr)
{
  globalItemRepo->addItem(S("!Spp.buildMgr"), sizeof(void*), &buildMgr);
  globalItemRepo->addItem(S("Spp_BuildMgr_dumpLlvmIrForElement"), (void*)&BuildMgr::_dumpLlvmIrForElement);
  globalItemRepo->addItem(S("Spp_BuildMgr_buildObjectFileForElement"), (void*)&BuildMgr::_buildObjectFileForElement);
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

} // namespace
