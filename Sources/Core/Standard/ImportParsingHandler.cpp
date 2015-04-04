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
#include "string.h"

namespace Core { namespace Standard
{

using namespace Data;

//==============================================================================
// Overloaded Abstract Functions

void ImportParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  IdentifiableObject *item = state->getData().get();
  static Word stringLiteralId = ID_GENERATOR->getId(STR("LexerDefs.StringLiteral"));
  QualifierSeeker seeker;
  // Find a literal token in the subject.
  IdentifiableObject *token;
  if (seeker.tryGet(STR("1~where(prodId=Subject.Subject1).{find prodId=Subject.Literal}"), item, token)) {
    ParsedToken *parsedToken = io_cast<ParsedToken>(token);
    // Is it a string token?
    if (parsedToken != 0 && parsedToken->getId() == stringLiteralId) {
      auto fileName = parsedToken->getText();
      if (!this->import(parsedToken->getText().c_str(), state)) {
        // Create a build msg.
        ParsingMetadataHolder *itemMeta = item->getInterface<ParsingMetadataHolder>();
        ASSERT(itemMeta != 0);
        state->addBuildMsg(
              SharedPtr<ImportLoadFailedMsg>(
                new ImportLoadFailedMsg(fileName.c_str(), itemMeta->getSourceLocation())));
      }
    }
  } else {
    throw EXCEPTION(GenericException, STR("Invalid data format."));
  }
  // Reset parsed data because we are done with the command.
  state->setData(SharedPtr<IdentifiableObject>(0));
}


Bool compareStringEnd(Str const &str, Char const *end)
{
  Int len = getStrLen(end);
  if (len >= str.size()) return false;
  return compareStr(str.c_str()+str.size()-len, end) == 0;
}


Bool ImportParsingHandler::import(Char const *filename, Processing::ParserState *state)
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
        id = this->rootManager->getLibraryManager()->load(dbgFilename.data());
      } else if (compareStr(dotPos, STR(".dbg."), 5) != 0) {
        // The position of the file's extension is found, and it doesn't already have
        // the .dbg extension, so we'll attach it.
        Int dotIndex = dotPos - filename;
        copyStr(filename, dbgFilename.data(), dotIndex);
        copyStr(STR(".dbg"), dbgFilename.data()+dotIndex);
        copyStr(dotPos, dbgFilename.data()+dotIndex+4);
        id = this->rootManager->getLibraryManager()->load(dbgFilename.data());
      }
    #endif
    if (id == 0) id = this->rootManager->getLibraryManager()->load(filename);
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
