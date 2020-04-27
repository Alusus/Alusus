/**
 * @file Spp/RootScopeHandlerExtension.cpp
 * Contains the implementation of class Spp::RootScopeHandlerExtension.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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
  auto extension = std::make_shared<RootScopeHandlerExtension>(handler);
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
  auto end = root->getCount() - 1;

  Bool execute = false;
  for (Int i = start; i <= end; ++i) {
    if (root->get(i) != 0 && !root->get(i)->isDerivedFrom<Core::Data::Ast::Definition>()) execute = true;
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
    auto astProcessor = rootManagerExt->jitBuildManager->getAstProcessor();
    astProcessor->preparePass(state->getNoticeStore());
    if (!astProcessor->process(root)) return;

    auto building = ti_cast<Building>(rootManagerExt->jitBuildManager.get());

    BuildSession buildSession;

    building->prepareExecution(
      state->getNoticeStore(), rootManager->getRootScope().get(), buildSession
    );

    Bool execute = true;

    // First, let's run all the modules initializations.
    for (Int i = 0; i < start; ++i) {
      auto def = ti_cast<Core::Data::Ast::Definition>(root->get(i));
      if (def != 0) {
        auto module = def->getTarget().ti_cast_get<Spp::Ast::Module>();
        if (module != 0) {
          if (!building->addElementToBuild(def.get(), buildSession)) execute = false;
        }
      }
    }

    // Now run all new statements.
    for (Int i = start; i <= end; ++i) {
      auto childData = root->get(i);
      if (!building->addElementToBuild(childData.get(), buildSession)) execute = false;
    }

    building->finalizeBuild(state->getNoticeStore(), rootManager->getRootScope().get(), buildSession);
    if (execute) {
      building->execute(state->getNoticeStore(), buildSession);
    }

    building->deleteTempFunctions(buildSession);
  }
}

} // namespace
