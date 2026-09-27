/**
 * @file Core/Processing/Handlers/ImportParsingHandler.cpp
 * Contains the implementation of class Core::Processing::Handlers::ImportParsingHandler.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"
#include "string.h"

namespace Core::Processing::Handlers
{

//==============================================================================
// Overloaded Abstract Functions

void ImportParsingHandler::onProdEnd(Parser *parser, ParserState *state)
{
  Str filenames;
  Str errorDetails;
  auto result = this->tryImport(
    state->getData().ti_cast_get<Containing<Ast::Node>>()->getElement(1), filenames, errorDetails, state
  );
  if (result == 0) {
    // TODO: Log the loaded library in the parent statement list in order to unload it when
    //       the statement list is complete.
  } else if (result == 1) {
    auto node = state->getData().get();
    state->addNotice(newSrdObj<Notices::ImportLoadFailedNotice>(
      filenames, errorDetails, node->findSourceLocation()
    ));
  }
  // Reset parsed data because we are done with the command.
  state->setData(SharedPtr<Core::Ast::Node>(0));
}


Int ImportParsingHandler::tryImport(Core::Ast::Node *astNode, Str &filenames, Str &errorDetails, ParserState *state)
{
  auto stringLiteral = ti_cast<Ast::StringLiteral>(astNode);
  if (stringLiteral != 0) {
    auto filename = stringLiteral->getValue().get();
    if (this->rootManager->tryImportFile(filename, errorDetails)) {
      return 0;
    } else {
      if (filenames.getLength() > 0) filenames += S(" || ");
      filenames += filename;
      return 1;
    }
  } else {
    auto logOperator = ti_cast<Ast::LogOperator>(astNode);
    if (logOperator != 0) {
      if (logOperator->getType() != "||" && logOperator->getType() != "or") {
        state->addNotice(newSrdObj<Notices::InvalidImportNotice>(
          Ast::findSourceLocation(astNode)
        ));
        return 2;
      }
      auto result = this->tryImport(logOperator->getFirst().get(), filenames, errorDetails, state);
      if (result == 1) {
        result = this->tryImport(logOperator->getSecond().get(), filenames, errorDetails, state);
      }
      return result;
    } else {
      state->addNotice(newSrdObj<Notices::InvalidImportNotice>(
        Ast::findSourceLocation(astNode)
      ));
      return 2;
    }
  }
}

} // namespace
