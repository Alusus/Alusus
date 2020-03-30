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

    // Process macros.
    rootManagerExt->astProcessor->preparePass(state->getNoticeStore());
    if (!rootManagerExt->astProcessor->runPass(root)) return;

    // Set global noticeStore var.
    auto globalNoticeStoreIndex = rootManagerExt->generator->getGlobalItemRepo()->findItem(S("Core.noticeStore"));
    if (globalNoticeStoreIndex == -1) {
      throw EXCEPTION(GenericException, S("Core.noticeStore global variable is missing from the global repo."));
    }
    auto globalNoticeStore = rootManagerExt->generator->getGlobalItemRepo()->getItemPtr(globalNoticeStoreIndex);
    *((Core::Notices::Store**)globalNoticeStore) = state->getNoticeStore();
    // Set global parser var.
    auto globalParserIndex = rootManagerExt->generator->getGlobalItemRepo()->findItem(S("Core.parser"));
    if (globalParserIndex == -1) {
      throw EXCEPTION(GenericException, S("Core.parser global variable is missing from the global repo."));
    }
    auto globalParser = rootManagerExt->generator->getGlobalItemRepo()->getItemPtr(globalParserIndex);
    *((Core::Processing::Parser**)globalParser) = parser;

    auto rootFuncName = S("__rootstatement__");

    rootManagerExt->buildManager->prepareExecution(
      state->getNoticeStore(), rootManager->getRootScope().get(), rootFuncName
    );

    Bool execute = true;

    // First, let's run all the modules initializations.
    for (Int i = 0; i < start; ++i) {
      auto def = ti_cast<Core::Data::Ast::Definition>(root->get(i));
      if (def != 0) {
        auto module = def->getTarget().ti_cast_get<Spp::Ast::Module>();
        if (module != 0) {
          if (!rootManagerExt->buildManager->addElementToBuild(def.get())) execute = false;
        }
      }
    }

    // Now run all new statements.
    for (Int i = start; i <= end; ++i) {
      auto childData = root->get(i);
      if (!rootManagerExt->buildManager->addElementToBuild(childData.get())) execute = false;
    }

    rootManagerExt->buildManager->finalizeBuild(state->getNoticeStore(), rootManager->getRootScope().get());
    if (execute) {
      rootManagerExt->buildManager->execute(state->getNoticeStore(), rootFuncName);
    }
  }
}

} // namespace
