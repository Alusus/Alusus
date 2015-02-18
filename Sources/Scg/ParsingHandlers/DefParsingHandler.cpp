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
      STR("1.0~where(prodId=Expression.LowerLinkExp).0~where(prodId=Subject.Subject1).0"),
      ReferenceUsageCriteria::MULTI_DATA);
    auto nameToken = io_cast<Data::ParsedToken>(seeker.tryGet(nameReference.get(), expr.get()));
    if (nameToken == 0 || nameToken->getId() != identifierTokenId) {
      // TODO: Generate a build message instead of throwing an exception.
      THROW_EXCEPTION(SyntaxErrorException, "A 'def' command needs a definition name.");
    }
    auto name = nameToken->getText();

    // Get the definee (after the colon).
    static SharedPtr<Reference> defReference = ReferenceParser::parseQualifier(
      STR("1.0~where(prodId=Expression.LowerLinkExp).2~where(prodId=Subject.Subject1).0"),
      ReferenceUsageCriteria::MULTI_DATA);
    auto def = seeker.tryGet(defReference.get(), expr.get());
    if (def == 0) {
      // TODO: Generate a build message instead of throwing an exception.
      // TODO: We need to choose terms for the parts of a define command, e.g.
      // definition name, definition, etc.
      THROW_EXCEPTION(SyntaxErrorException, "A 'def' has an invalid definition.");
    }

    // Check if the definee is an alias.
    // TODO: Replace the following hack with a proper alias structure after the reference
    //       system is utilized in the SCG.
    // The proper way to implement the alias is by defining an alias, which is a reference to the
    // actual element. But since the references are not utilized in SCG we'll replace that with
    // a dictionary that will be looked up later on by the Variable and FunctionCall structures.
    static SharedPtr<Reference> aliasReference = ReferenceParser::parseQualifier(
      STR("self~where(prodId=Subject.Alias).1~where(prodId=Subject.Subject1).{find prodId=Subject.Parameter, 0}"),
      ReferenceUsageCriteria::MULTI_DATA);
    auto alias = io_cast<ParsedToken>(seeker.tryGet(aliasReference.get(), def));
    if (alias != 0) {
      // Add the alias to the dictionary.
      SharedMap *aliasDictionary =
          static_cast<SharedMap*>(state->getDataStack()->tryGet(this->aliasDictionaryRef.get()));
      // Create the dictionary if it's not created yet.
      if (aliasDictionary == 0) {
        SharedPtr<SharedMap> _aliasDictionary = SharedMap::create(true, {});
        state->getDataStack()->set(this->aliasDictionaryRef.get(), _aliasDictionary.get());
        aliasDictionary = _aliasDictionary.get();
      }
      aliasDictionary->set(name.c_str(), String::create(alias->getText().c_str()), true);
    } else {
      // Store the definition in the definitions manager.
      Core::Standard::mergeDefinition(name.c_str(), def, state);
    }

    // Reset parsed data because we are done with the command.
    state->setData(SharedPtr<IdentifiableObject>(0));
}

} // namespace

