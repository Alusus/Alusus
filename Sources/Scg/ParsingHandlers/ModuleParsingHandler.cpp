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
  static SharedPtr<Reference> statementListReference = REF_PARSER->parseQualifier(
        STR("1~where(prodId=Main.StatementList)"),
        ReferenceUsageCriteria::MULTI_DATA);

  auto item = state->getData();
  auto metadata = item->getInterface<Ast::MetadataHolder>();
  ASSERT(metadata != 0);

  // Find the statement list in the subject.
  auto statementList = ti_cast<Ast::List>(seeker.tryGet(statementListReference.get(), item.get()));

  if (statementList == 0) {
    // Create a build error msg.
    state->addBuildMsg(std::make_shared<Processing::UnrecognizedErrorNotice>(metadata->getSourceLocation()));
    return;
  }

  // Create a Module out of the given statement list.
  auto module = std::make_shared<Scg::Module>();

  static CodeGenerator generator;
  generator.setBuildMsgStore(state->getBuildMsgStore());
  generator.setAliasDictionary(static_cast<SharedMap*>(state->getDataStack()->tryGet(this->aliasDictionaryRef.get())));

  for (auto i = 0; i < statementList->getCount(); i++) {
    auto element = statementList->getShared(i);
    auto elementMetadata = element->getInterface<Ast::MetadataHolder>();

    if (element == 0 || elementMetadata == 0) {
      state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(
                           STR("Invalid statement received in module body."),
                           STR("SCG1025"), 1, metadata->getSourceLocation()));
    }

    auto id = elementMetadata->getProdId();

    if (id == defId) {
      // Add the definition to the module.
      this->addDefinitionToModule(state, element, module.get(), &generator);
    } else if (id == linkId) {
      // Add the link expression to the module.
      this->addLinkToModule(state, element, module.get(), &generator);
    } else {
      // Raise a build error.
      state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(STR("Invalid statement inside module body."),
                         elementMetadata->getSourceLocation()));
    }
  }

  // Return the create module.
  state->setData(module);
}


void ModuleParsingHandler::addDefinitionToModule(Processing::ParserState *state,
    const SharedPtr<TiObject> &def,
    Module *module, CodeGenerator *generator)
{
  static Word identifierTokenId = Core::Data::IdGenerator::getSingleton()->getId(STR("LexerDefs.Identifier"));
  static ReferenceSeeker seeker;
  static SharedPtr<Reference> nameReference = REF_PARSER->parseQualifier(
        STR("1~where(prodId=Expression.Exp)."
            "0~where(prodId=Expression.LowerLinkExp)."
            "0~where(prodId=Subject.Subject1)."
            "0~where(prodId=Subject.Parameter)"),
        ReferenceUsageCriteria::MULTI_DATA);
  auto metadata = def.ti_cast_get<Ast::MetadataHolder>();
  ASSERT(metadata != 0);
  auto nameToken = ti_cast<Ast::Token>(seeker.tryGet(nameReference.get(), def.get()));

  if (nameToken == nullptr || nameToken->getId() != identifierTokenId) {
    state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(
                         STR("A 'def' command needs a definition name."),
                         STR("SCG1002"), 1, metadata->getSourceLocation()));
    return;
  }

  auto name = nameToken->getText();

  //module->add(name.c_str(), def);
  for (auto stat : generator->generateStatement(def)) {
    module->appendNode(stat);
  }
}


void ModuleParsingHandler::addLinkToModule(Processing::ParserState *state,
    const SharedPtr<TiObject> &link,
    Module *module, CodeGenerator *generator)
{
  auto linkMetadata = link->getInterface<Ast::MetadataHolder>();

  static ReferenceSeeker seeker;
  static SharedPtr<Reference> listReference = REF_PARSER->parseQualifier(
        STR("1~where(prodId=Expression.Exp)."
            "0~where(prodId=Subject.Subject1)."
            "0~where(prodId=Main.StatementList)"),
        ReferenceUsageCriteria::MULTI_DATA);
  static SharedPtr<Reference> exprReference = REF_PARSER->parseQualifier(
        STR("0~where(prodId=Expression.Exp)"),
        ReferenceUsageCriteria::MULTI_DATA);

  auto list = ti_cast<Ast::List>(seeker.tryGet(listReference.get(), link.get()));

  if (list != 0) {
    for (auto i = 0; i < list->getCount(); i++) {
      // auto name = this->getLinkName(list->get(i));
      auto l = std::make_shared<Ast::List>(linkMetadata->getProdId(), linkMetadata->getSourceLocation());
      l->add(list->get(i));
      for (auto stat : generator->generateStatement(l)) {
        module->appendNode(stat);
      }
    }
  } else {
    auto expr = ti_cast<Ast::List>(seeker.tryGet(exprReference.get(), link.get()));

    if (expr != 0) {
      // auto name = this->getLinkName(expr);
      for (auto stat : generator->generateStatement(link)) {
        module->appendNode(stat);
      }
    } else {
      state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(
                           STR("Invalid link statement."),
                           STR("SCG1026"), 1, linkMetadata->getSourceLocation()));
    }
  }
}


Char const* ModuleParsingHandler::getLinkName(TiObject *link)
{
  static ReferenceSeeker seeker;
  static SharedPtr<Reference> funcExpNoRetReference = REF_PARSER->parseQualifier(
        STR("0~where(prodId=Expression.FunctionalExp)"),
        ReferenceUsageCriteria::MULTI_DATA);
  static SharedPtr<Reference> funcExpReference = REF_PARSER->parseQualifier(
        STR("0~where(prodId=Expression.LowLinkExp)."
            "0~where(prodId=Expression.FunctionalExp)"),
        ReferenceUsageCriteria::MULTI_DATA);
  static SharedPtr<Reference> nameReference = REF_PARSER->parseQualifier(
        STR("0~where(prodId=Subject.Subject1)."
            "0~where(prodId=Subject.Parameter)"),
        ReferenceUsageCriteria::MULTI_DATA);
  static Word identifierTokenId = Core::Data::IdGenerator::getSingleton()->getId(STR("LexerDefs.Identifier"));

  auto funcExp = ti_cast<Ast::List>(seeker.tryGet(funcExpNoRetReference.get(), link));

  if (funcExp == 0) {
    funcExp = ti_cast<Ast::List>(seeker.tryGet(funcExpReference.get(), link));

    if (funcExp == 0)
      throw EXCEPTION(InvalidArgumentException, "Invalid function link expression.");
  }

  auto nameToken = ti_cast<Ast::Token>(seeker.tryGet(nameReference.get(), funcExp));

  if (nameToken == 0 || nameToken->getId() != identifierTokenId)
    throw EXCEPTION(InvalidArgumentException,
                    "Functional expressions should have the name of the function "
                    "under the path 0:Subject.Subject1>0:Subject.Parameter.");

  return nameToken->getText().c_str();
}

} // namespace
