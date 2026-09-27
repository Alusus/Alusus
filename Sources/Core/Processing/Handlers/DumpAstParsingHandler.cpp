/**
 * @file Core/Processing/Handlers/DumpAstParsingHandler.cpp
 * Contains the implementation of Core::Processing::Handlers::DumpAstParsingHandler.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Processing::Handlers
{

//==============================================================================
// Overloaded Abstract Functions

void DumpAstParsingHandler::onProdEnd(Parser *parser, ParserState *state)
{
  using SeekVerb = Ast::Seeker::Verb;

  auto data = state->getData().ti_cast_get<Containing<Ast::Node>>()->getElement(1);
  ASSERT(data != 0);

  try {
    Bool found = false;
    data->setOwner(parser->getRootScope().get());
    this->rootManager->getSeeker()->foreach(data, state->getDataStack(),
      [=, &found](TiInt action, Ast::Node *obj, Core::Ast::Seeker::NoticePtr const &notice)->SeekVerb
      {
        if (action == Core::Ast::Seeker::Action::TARGET_MATCH && obj != 0) {
          outStream << S("------------------ Parsed Data Dump ------------------\n");
          Ast::dumpAst(outStream, obj, 0);
          outStream << S("\n------------------------------------------------------\n");
          found = true;
        }
        return SeekVerb::MOVE;
      }, 0
    );
    if (!found) {
      state->addNotice(newSrdObj<Notices::InvalidDumpArgNotice>(data->findSourceLocation()));
    }
  } catch (InvalidArgumentException) {
    state->addNotice(newSrdObj<Notices::InvalidDumpArgNotice>(data->findSourceLocation()));
  }

  state->setData(SharedPtr<Ast::Node>(0));
}

} // namespace
