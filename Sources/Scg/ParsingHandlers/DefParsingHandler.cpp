/**
 * @file Scg/ParsingHandlers/DefParsingHandler.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <core.h>   // Alusus core header files.
#include <llvm/IR/IRBuilder.h>
#include <scg.h>
#include <CodeGeneration/CodeGenerator.h>
#include <ParsingHandlers/DefParsingHandler.h>

namespace Scg
{

using namespace Core;

////////////////////////////////////////////////////////////////////////////////
// Overloaded Abstract Functions

void DefParsingHandler::onProdEnd(Parser::State *state)
{
    auto expr = state->refTopTermLevel().getData().io_cast<Standard::ParsedItem>();
    assert(expr != 0);

    // Get the name of the definition.
    static Int identifierTokenId = ID_GENERATOR->getId("IDENTIFIER_TOKEN");
    static Standard::ParsedDataBrowser nameBrowser;
    if (!nameBrowser.isInitialized()) {
      nameBrowser.initialize(STR("0:Expression.LowerLinkExp>0:Subject.Subject1>0"));
    }
    auto nameToken = nameBrowser.getChildValue<Standard::ParsedToken>(expr);
    if (nameToken == 0 || nameToken->getId() != identifierTokenId) {
      // TODO: Generate a build message instead of throwing an exception.
      THROW_EXCEPTION(SyntaxErrorException, "A 'def' command needs a definition name.");
    }
    auto name = nameToken->getText();

    // Get the definee (after the colon).
    static Standard::ParsedDataBrowser defBrowser;
    if (!defBrowser.isInitialized()) {
      defBrowser.initialize(STR("0:Expression.LowerLinkExp>2:Subject.Subject1>0"));
    }
    auto def = defBrowser.getChildValue<IdentifiableObject>(expr);
    if (def == 0) {
      // TODO: Generate a build message instead of throwing an exception.
      // TODO: We need to choose terms for the parts of a define command, e.g.
      // definition name, definition, etc.
      THROW_EXCEPTION(SyntaxErrorException, "A 'def' has an invalid definition.");
    }

    // Store the definition in the definitions manager.
    Core::Standard::mergeDefinition(this->rootManager->getDefinitionsManager(), name.c_str(), def, state);

    // Reset parsed data because we are done with the command.
    state->refTopTermLevel().setData(SharedPtr<IdentifiableObject>(0));
}

} // namespace

