/**
 * @file Spp/RootScopeHandlerExtension.cpp
 * Contains the implementation of class Spp::RootScopeHandlerExtension.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
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
  rootScopeHandler->addNewElement.useCallee(base)(data, parser, state);
  if (data != 0 && !data->isDerivedFrom<Core::Data::Ast::Definition>()) {
    PREPARE_SELF(extension, RootScopeHandlerExtension);
    auto rootManager = extension->rootManagerBox->get();
    auto rootManagerExt = ti_cast<RootManagerExtension>(rootManager);

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

    if (data->isDerivedFrom<Core::Data::Ast::MergeList>()) {
      auto mergeList = data.s_cast_get<Core::Data::Ast::MergeList>();
      for (Int i = 0; i < mergeList->getCount(); ++i) {
        auto childData = mergeList->get(i);
        if (!childData->isDerivedFrom<Core::Data::Ast::Definition>()) {
          rootManagerExt->executeRootElement(childData, state->getNoticeStore());
        }
      }
    } else {
      rootManagerExt->executeRootElement(data, state->getNoticeStore());
    }
  }
}

} // namespace
