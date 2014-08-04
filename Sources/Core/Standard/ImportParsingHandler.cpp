/**
 * @file Core/Standard/ImportParsingHandler.cpp
 * Contains the implementation of class Core::Standard::ImportParsingHandler.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <regex>

namespace Core { namespace Standard
{

//==============================================================================
// Overloaded Abstract Functions

void ImportParsingHandler::onProdEnd(Processing::Parser *machine, Processing::ParserState *state)
{
  SharedPtr<ParsedItem> item = state->getData().io_cast<ParsedItem>();
  ParsedDataBrowser browser;
  // Find a literal token in the subject.
  SharedPtr<ParsedToken> token =
      browser.getValue<ParsedToken>(STR("Subject.Subject1>Subject.Literal"), item);
  if (token != 0) {
    // Is it a string token?
    if (ID_GENERATOR->getDesc(token->getId()) == STR("STRING_LITERAL_TOKEN")) {
      if (!this->import(token->getText().c_str(), state)) {
        // Create a build msg.
        state->addBuildMsg(
              SharedPtr<ImportLoadFailedMsg>(
                new ImportLoadFailedMsg(item->getLine(), item->getColumn())));
      }
    }
  }
  // Reset parsed data because we are done with the command.
  state->setData(SharedPtr<IdentifiableObject>(0));
}


Bool ImportParsingHandler::import(Char const *filename, Processing::ParserState *state)
{
  // Check the file type.
  static std::regex r(STR(".*\\.(source|alusus)"));

  if (std::regex_match(filename, r)) {
    try {
      this->rootManager->processFile(filename);
    } catch (...) {
      return false;
    }
  } else {
    // If so, import library and return.
    PtrWord id = this->rootManager->getLibraryManager()->load(filename);
    // Did we fail the loading?
    if (id == 0) {
      return false;
    } else {
      // TODO: Log the loaded library in the parent statement list in order to unload it when
      //       the statement list is complete.
    }
  }
  return true;
}

} } // namespace
