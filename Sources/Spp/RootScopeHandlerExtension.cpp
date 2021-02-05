/**
 * @file Spp/RootScopeHandlerExtension.cpp
 * Contains the implementation of class Spp::RootScopeHandlerExtension.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp
{

//==============================================================================
// Initialization Functions

RootScopeHandlerExtension::Overrides* RootScopeHandlerExtension::extend(
  Core::Main::RootScopeHandler *handler, Core::Main::RootManager *rootManager
) {
  auto extension = newSrdObj<RootScopeHandlerExtension>(handler);
  handler->addDynamicInterface(extension);

  auto overrides = new Overrides();
  extension->rootManagerBox = Box<Core::Main::RootManager*>::create(rootManager);
  overrides->addNewElementRef = handler->addNewElement.set(&RootScopeHandlerExtension::_addNewElement).get();

  return overrides;
}


void RootScopeHandlerExtension::unextend(Core::Main::RootScopeHandler *handler, Overrides *overrides)
{
  auto extension = ti_cast<RootScopeHandlerExtension>(handler);
  handler->addNewElement.reset(overrides->addNewElementRef);
  extension->rootManagerBox.remove();
  handler->removeDynamicInterface<RootScopeHandlerExtension>();
  delete overrides;
}


//==============================================================================
// Main Functions

void RootScopeHandlerExtension::_addNewElement(
  TiFunctionBase *base, TiObject *self, TioSharedPtr const &data,
  Core::Processing::Parser *parser, Core::Processing::ParserState *state
) {
  PREPARE_SELF(rootScopeHandler, Core::Main::RootScopeHandler);
  auto root = rootScopeHandler->getRootScope().get();

  auto start = root->getCount();
  rootScopeHandler->addNewElement.useCallee(base)(data, parser, state);

  Bool execute = false;
  for (Int i = start; i < root->getCount(); ++i) {
    if (
      root->get(i) != 0 &&
      !root->get(i)->isDerivedFrom<Core::Data::Ast::Definition>() &&
      !root->get(i)->isDerivedFrom<Core::Data::Ast::Bridge>()
    ) execute = true;
  }

  if (execute) {
    PREPARE_SELF(extension, RootScopeHandlerExtension);
    auto rootManager = extension->rootManagerBox->get();
    auto rootManagerExt = ti_cast<RootManagerExtension>(rootManager);

    // Prepare rtAstMgr & rtBuildMgr
    rootManagerExt->rtAstMgr->setParser(parser);
    rootManagerExt->rtAstMgr->setNoticeStore(state->getNoticeStore());
    rootManagerExt->rtBuildMgr->setParser(parser);
    rootManagerExt->rtBuildMgr->setNoticeStore(state->getNoticeStore());

    // Process macros.
    auto astProcessor = rootManagerExt->astProcessor.get();
    astProcessor->preparePass(state->getNoticeStore());
    if (!astProcessor->process(root)) return;

    auto executing = ti_cast<Executing>(rootManagerExt->buildManager.get());

    SharedPtr<BuildSession> buildSession = executing->prepareBuild(
      state->getNoticeStore(), BuildManager::BuildType::JIT, rootManager->getRootScope().get()
    );

    Bool execute = true;

    // First, let's run all the modules initializations.
    for (Int i = 0; i < start; ++i) {
      auto def = ti_cast<Core::Data::Ast::Definition>(root->get(i));
      if (def != 0) {
        auto module = def->getTarget().ti_cast_get<Spp::Ast::Module>();
        if (module != 0) {
          if (!executing->addElementToBuild(def.get(), buildSession.get())) execute = false;
        }
      }
    }

    // Now run all new statements.
    for (Int i = start; i < root->getCount(); ++i) {
      auto childData = root->getElement(i);
      if (CodeGen::isExecuted(childData)) continue;
      CodeGen::setExecuted(childData, true);
      if (!executing->addElementToBuild(childData, buildSession.get())) execute = false;
    }

    executing->finalizeBuild(state->getNoticeStore(), rootManager->getRootScope().get(), buildSession.get());
    if (execute) {
      executing->execute(state->getNoticeStore(), buildSession.get());
    }
  }
}

} // namespace
