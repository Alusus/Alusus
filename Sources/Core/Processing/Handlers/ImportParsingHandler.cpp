/**
 * @file Core/Processing/Handlers/ImportParsingHandler.cpp
 * Contains the implementation of class Core::Processing::Handlers::ImportParsingHandler.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include "string.h"

namespace Core::Processing::Handlers
{

using namespace Data;

//==============================================================================
// Overloaded Abstract Functions

void ImportParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  auto metadata = state->getData().ti_cast<Ast::Metadata>();
  auto stringLiteral = ti_cast<Ast::StringLiteral>(
    state->getData().ti_cast_get<Basic::Containing<TiObject>>()->getElement(1)
  );
  if (stringLiteral) {
    auto filename = stringLiteral->getValue().get();
    Str errorDetails;
    if (!this->import(filename, state, errorDetails)) {
      // Create a build msg.
      state->addNotice(std::make_shared<Notices::ImportLoadFailedNotice>(
        filename, errorDetails.c_str(), metadata->findSourceLocation()
      ));
    }
  } else {
    throw EXCEPTION(GenericException, STR("Invalid data format."));
  }
  // Reset parsed data because we are done with the command.
  state->setData(SharedPtr<TiObject>(0));
}


Bool compareStringEnd(Str const &str, Char const *end)
{
  Int len = getStrLen(end);
  if (len >= str.size()) return false;
  return compareStr(str.c_str()+str.size()-len, end) == 0;
}


Bool ImportParsingHandler::import(Char const *filename, Processing::ParserState *state, Str &errorDetails)
{
  // Check the file type.
  if (compareStringEnd(filename, STR(".source")) || compareStringEnd(filename, STR(".alusus")) ||
      compareStringEnd(filename, STR(".مصدر")) || compareStringEnd(filename, STR(".الأسس")) ||
      compareStringEnd(filename, STR(".أسس"))) {
    try {
      LOG(LogLevel::PARSER_MAJOR, STR("Importing source file: ") << filename);

      this->rootManager->processFile(filename);
    } catch (...) {
      return false;
    }
  } else {
    LOG(LogLevel::PARSER_MAJOR, STR("Importing library: ") << filename);

    // If so, import library and return.
    PtrWord id = 0;

    #ifndef RELEASE
      // We are running in debug mode, so we will look first for a debug verion.
      // Find the first '.' after the last '/'.
      thread_local static std::array<Char,FILENAME_MAX> dbgFilename;
      Char const *dotPos = strrchr(filename, CHR('/'));
      if (dotPos == 0) dotPos = filename;
      dotPos = strchr(dotPos, CHR('.'));
      if (dotPos == 0) {
        // The filename has no extension, so we'll attach .dbg to the end.
        copyStr(filename, dbgFilename.data());
        copyStr(STR(".dbg"), dbgFilename.data()+getStrLen(filename));
        id = this->rootManager->getLibraryManager()->load(dbgFilename.data(), errorDetails);
      } else if (compareStr(dotPos, STR(".dbg."), 5) != 0) {
        // The position of the file's extension is found, and it doesn't already have
        // the .dbg extension, so we'll attach it.
        Int dotIndex = dotPos - filename;
        copyStr(filename, dbgFilename.data(), dotIndex);
        copyStr(STR(".dbg"), dbgFilename.data()+dotIndex);
        copyStr(dotPos, dbgFilename.data()+dotIndex+4);
        id = this->rootManager->getLibraryManager()->load(dbgFilename.data(), errorDetails);
      }
    #endif
    if (id == 0) id = this->rootManager->getLibraryManager()->load(filename, errorDetails);
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

} // namespace
