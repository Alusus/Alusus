/**
 * @file Core/Main/RootScopeHandler.cpp
 * Contains the implementation of class Core::Main::RootScopeHandler.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Main
{

//==============================================================================
// Initialization Functions

void RootScopeHandler::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->addNewElement
  });
}


void RootScopeHandler::initBindings()
{
  this->addNewElement = &RootScopeHandler::_addNewElement;
}


//==============================================================================
// Main Functions

void RootScopeHandler::_addNewElement(TiObject *self, TioSharedPtr const &data,
  Processing::Parser *parser, Processing::ParserState *state
) {
  PREPARE_SELF(rootScopeHandler, RootScopeHandler);
  if (data != 0) {
    Core::Data::Ast::addPossiblyMergeableElement(
      data.get(), rootScopeHandler->rootScope.get(), rootScopeHandler->seeker, state->getNoticeStore()
    );
  }
}

} // namespace
