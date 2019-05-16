/**
 * @file Core/Processing/Handlers/ImportParsingHandler.cpp
 * Contains the implementation of class Core::Processing::Handlers::ImportParsingHandler.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include "string.h"

namespace Core::Processing::Handlers
{

using namespace Data;

//==============================================================================
// Overloaded Abstract Functions

void ImportParsingHandler::onProdEnd(Parser *parser, ParserState *state)
{
  auto metadata = state->getData().ti_cast<Ast::MetaHaving>();
  auto stringLiteral = ti_cast<Ast::StringLiteral>(
    state->getData().ti_cast_get<Containing<TiObject>>()->getElement(1)
  );
  if (stringLiteral) {
    auto filename = stringLiteral->getValue().get();
    Str errorDetails;
    if (!this->rootManager->tryImportFile(filename, errorDetails)) {
      // Create a build msg.
      state->addNotice(std::make_shared<Notices::ImportLoadFailedNotice>(
        filename, errorDetails.c_str(), metadata->findSourceLocation()
      ));
    } else {
      // TODO: Log the loaded library in the parent statement list in order to unload it when
      //       the statement list is complete.
    }
  } else {
    throw EXCEPTION(GenericException, S("Invalid data format."));
  }
  // Reset parsed data because we are done with the command.
  state->setData(SharedPtr<TiObject>(0));
}

} // namespace
