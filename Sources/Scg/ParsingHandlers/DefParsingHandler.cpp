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
using namespace Core::Data;

//==============================================================================
// Overloaded Abstract Functions

void DefParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
    auto expr = state->getData();
    assert(expr != 0);

    // Get the name of the definition.
    static Int identifierTokenId = ID_GENERATOR->getId("LexerDefs.Identifier");
    static ReferenceSeeker seeker;
    static SharedPtr<Reference> nameReference = ReferenceParser::parseQualifier(
      STR("0~where(prodId=Expression.LowerLinkExp).0~where(prodId=Subject.Subject1).0"),
      ReferenceUsageCriteria::MULTI_DATA);
    auto nameToken = seeker.tryGetShared<Data::ParsedToken>(nameReference.get(), expr.get());
    if (nameToken == 0 || nameToken->getId() != identifierTokenId) {
      // TODO: Generate a build message instead of throwing an exception.
      THROW_EXCEPTION(SyntaxErrorException, "A 'def' command needs a definition name.");
    }
    auto name = nameToken->getText();

    // Get the definee (after the colon).
    static SharedPtr<Reference> defReference = ReferenceParser::parseQualifier(
      STR("0~where(prodId=Expression.LowerLinkExp).2~where(prodId=Subject.Subject1).0"),
      ReferenceUsageCriteria::MULTI_DATA);
    auto def = seeker.tryGetShared(defReference.get(), expr.get());
    if (def == 0) {
      // TODO: Generate a build message instead of throwing an exception.
      // TODO: We need to choose terms for the parts of a define command, e.g.
      // definition name, definition, etc.
      THROW_EXCEPTION(SyntaxErrorException, "A 'def' has an invalid definition.");
    }

    // Store the definition in the definitions manager.
    Core::Standard::mergeDefinition(name.c_str(), def, state);

    // Reset parsed data because we are done with the command.
    state->setData(SharedPtr<IdentifiableObject>(0));
}

} // namespace

