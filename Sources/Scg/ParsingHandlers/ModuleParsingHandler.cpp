/**
 * @file Scg/ParsingHandlers/ModuleParsingHandler.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

// Alusus core header files.
#include <core.h>

// Boost header files
#include <boost/lexical_cast.hpp>

// SCG header files
#include <scg.h>
#include <CodeGeneration/CodeGenerator.h>
#include <ParsingHandlers/ModuleParsingHandler.h>

namespace Scg
{

using namespace Core;
using namespace Core::Data;

//==============================================================================
// Overloaded Abstract Functions

void ModuleParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
    static Word defId = IdGenerator::getSingleton()->getId(STR("SubMain.Def"));
    static Word linkId = IdGenerator::getSingleton()->getId(STR("Main.Link"));
    static ReferenceSeeker seeker;
    static SharedPtr<Reference> statementListReference = ReferenceParser::parseQualifier(
      STR("self~where(prodId=Main.StatementList)"),
      ReferenceUsageCriteria::MULTI_DATA);

    auto item = state->getData();

    // Find the statement list in the subject.
    auto statementList = seeker.tryGetPlain<ParsedList>(statementListReference.get(), item.get());
    if (statementList == 0) {
      // Create a build error msg.
      auto metadata = item->getInterface<ParsingMetadataHolder>();
      if (metadata != 0) {
        state->addBuildMsg(std::make_shared<Processing::UnrecognizedErrorMsg>(
          metadata->getLine(), metadata->getColumn()));
      } else {
        state->addBuildMsg(std::make_shared<Processing::UnrecognizedErrorMsg>(
          -1, -1));
      }
      return;
    }

    // Create a Module out of the given statement list.
    auto module = Core::Data::Module::create({});
    for (auto i = 0; i < static_cast<ParsedList*>(statementList)->getCount(); i++) {
        auto element = static_cast<ParsedList*>(statementList)->get(i);
        auto elementMetadata = element->getInterface<ParsingMetadataHolder>();
        if (element == 0 || elementMetadata == 0) {
            // TODO: Generate a build message instead of throwing an exception.
            THROW_EXCEPTION(SyntaxErrorException, "Invalid statement received in module body.");
        }
        auto id = elementMetadata->getProdId();
        if (id == defId) {
            // Add the definition to the module.
            this->add_definition_to_module(element, module.get());
        } else if (id == linkId) {
            // Add the link expression to the module.
            this->add_link_to_module(element, module.get());
        } else {
            // Raise a build error.
            state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(STR("Invalid statement inside module body."),
                elementMetadata->getLine(), elementMetadata->getColumn()));
        }
    }

    // Return the create module.
    state->setData(module);
}


void ModuleParsingHandler::add_definition_to_module(const SharedPtr<IdentifiableObject> &def,
                                                    Core::Data::Module *module)
{
  static Word identifierTokenId = Core::Data::IdGenerator::getSingleton()->getId(STR("LexerDefs.Identifier"));
  static ReferenceSeeker seeker;
  static SharedPtr<Reference> nameReference = ReferenceParser::parseQualifier(
    STR("0~where(prodId=Expression.Exp)."
        "0~where(prodId=Expression.LowerLinkExp)."
        "0~where(prodId=Subject.Subject1)."
        "0~where(prodId=Subject.Parameter)"),
    ReferenceUsageCriteria::MULTI_DATA);
  auto nameToken = seeker.tryGetShared<ParsedToken>(nameReference.get(), def.get());
  if (nameToken == nullptr || nameToken->getId() != identifierTokenId) {
      // TODO: Generate a build message instead of throwing an exception.
      THROW_EXCEPTION(SyntaxErrorException, "A 'def' command needs a definition name.");
  }
  auto name = nameToken->getText();

  static int i = 0;
  i++;
  //module->add(name.c_str(), def);
  module->add(boost::lexical_cast<std::string>(i).c_str(), def);
}


void ModuleParsingHandler::add_link_to_module(const SharedPtr<IdentifiableObject> &link,
                                              Core::Data::Module *module)
{
  auto linkMetadata = link->getInterface<ParsingMetadataHolder>();

  static ReferenceSeeker seeker;
  static SharedPtr<Reference> listReference = ReferenceParser::parseQualifier(
    STR("0~where(prodId=Expression.Exp)."
        "0~where(prodId=Subject.Subject1)."
        "0~where(prodId=Main.StatementList)"),
    ReferenceUsageCriteria::MULTI_DATA);
  static SharedPtr<Reference> exprReference = ReferenceParser::parseQualifier(
    STR("0~where(prodId=Expression.Exp)"),
    ReferenceUsageCriteria::MULTI_DATA);

  auto list = seeker.tryGetShared<ParsedList>(listReference.get(), link.get());
  if (list != 0)
  {
    for (auto i = 0; i < list->getCount(); i++)
    {
      auto name = this->get_link_name(list->get(i));
      auto l = std::make_shared<ParsedList>(linkMetadata->getProdId());
      l->setLine(linkMetadata->getLine());
      l->setColumn(linkMetadata->getColumn());
      l->add(list->get(i));
      module->add(name, l);
    }
  }
  else
  {
    auto expr = seeker.tryGetShared<ParsedList>(exprReference.get(), link.get());
    if (expr != 0)
    {
      auto name = this->get_link_name(expr);
      module->add(name, link);
    }
    else
      // TODO: Improve exception message.
      THROW_EXCEPTION(SyntaxErrorException, "Invalid link statement.");
  }
}


Char const* ModuleParsingHandler::get_link_name(const SharedPtr<IdentifiableObject> &link)
{
  static ReferenceSeeker seeker;
  static SharedPtr<Reference> funcExpNoRetReference = ReferenceParser::parseQualifier(
    STR("0~where(prodId=Expression.FunctionalExp)"),
    ReferenceUsageCriteria::MULTI_DATA);
  static SharedPtr<Reference> funcExpReference = ReferenceParser::parseQualifier(
    STR("0~where(prodId=Expression.LowLinkExp)."
        "0~where(prodId=Expression.FunctionalExp)"),
    ReferenceUsageCriteria::MULTI_DATA);
  static SharedPtr<Reference> nameReference = ReferenceParser::parseQualifier(
    STR("0~where(prodId=Subject.Subject1)."
        "0~where(prodId=Subject.Parameter)"),
    ReferenceUsageCriteria::MULTI_DATA);
  static Word identifierTokenId = Core::Data::IdGenerator::getSingleton()->getId(STR("LexerDefs.Identifier"));

  auto funcExp = seeker.tryGetShared<ParsedList>(funcExpNoRetReference.get(), link.get());
  if (funcExp == 0)
  {
    funcExp = seeker.tryGetShared<ParsedList>(funcExpReference.get(), link.get());
    if (funcExp == 0)
      THROW_EXCEPTION(InvalidArgumentException, "Invalid function link expression.");
  }
  auto nameToken = seeker.tryGetShared<ParsedToken>(nameReference.get(), funcExp.get());
  if (nameToken == 0 || nameToken->getId() != identifierTokenId)
    THROW_EXCEPTION(InvalidArgumentException,
        "Functional expressions should have the name of the function "
        "under the path 0:Subject.Subject1>0:Subject.Parameter.");
  return nameToken->getText().c_str();
}

} // namespace
