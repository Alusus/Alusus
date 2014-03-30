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
using Core::Data::IdGenerator;
using Standard::ParsedItem;
using Standard::ParsedList;
using Standard::ParsedToken;
using Standard::ParsedDataBrowser;

////////////////////////////////////////////////////////////////////////////////
// Overloaded Abstract Functions

void ModuleParsingHandler::onProdEnd(Parser::State *state)
{
    static Word defId = IdGenerator::getSingleton()->getId(STR("SubMain.Def"));
    static Word linkId = IdGenerator::getSingleton()->getId(STR("Main.Link"));
    static ParsedDataBrowser statementListBrowser(STR("0:Main.StatementList"));

    auto item = state->refTopTermLevel().getData().io_cast<ParsedItem>();

    // Find the statement list in the subject.
    auto statementList = statementListBrowser.getValue<ParsedList>(item);
    if (statementList == 0) {
        // Create a build error msg.
        state->addBuildMsg(SharedPtr<Common::UnrecognizedErrorMsg>(
            new Common::UnrecognizedErrorMsg(item->getLine(), item->getColumn())));
        return;
    }

    // Create a Module out of the given statement list.
    auto module = Core::Data::Module::create({});
    for (auto i = 0; i < statementList.s_cast_get<ParsedList>()->getElementCount(); i++) {
        auto element = statementList.s_cast_get<ParsedList>()->getElement(i).io_cast<ParsedItem>();
        if (element == 0) {
            // TODO: Generate a build message instead of throwing an exception.
            THROW_EXCEPTION(SyntaxErrorException, "Invalid statement received in module body.");
        }
        auto id = element->getProdId();
        if (id == defId) {
            // Add the definition to the module.
            this->add_definition_to_module(element, module.get());
        } else if (id == linkId) {
            // Add the link expression to the module.
            this->add_link_to_module(element, module.get());
        } else {
            // Raise a build error.
            state->addBuildMsg(std::make_shared<Common::CustomBuildMsg>(STR("Invalid statement inside module body."),
                element->getLine(), element->getColumn()));
        }
    }

    // Return the create module.
    state->refTopTermLevel().setData(module);
}


void ModuleParsingHandler::add_definition_to_module(const SharedPtr<ParsedItem> &def,
                                                    Core::Data::Module *module)
{
    static Word identifierTokenId = Core::Data::IdGenerator::getSingleton()->getId(STR("IDENTIFIER_TOKEN"));
    static ParsedDataBrowser nameBrowser(
        STR("0:Expression.Exp>"
            "0:Expression.LowerLinkExp>"
            "0:Subject.Subject1>"
            "0:Subject.Parameter"));
    auto nameToken = nameBrowser.getChildValue<ParsedToken>(def);
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


void ModuleParsingHandler::add_link_to_module(const SharedPtr<ParsedItem> &link,
                                              Core::Data::Module *module)
{
    static ParsedDataBrowser listBrowser(
        STR("0:Expression.Exp>"
            "0:Subject.Subject1>"
            "0:Main.StatementList"));
    static ParsedDataBrowser exprBrowser(
        STR("0:Expression.Exp"));

    auto list = listBrowser.getChildValue<ParsedList>(link);
    if (list != 0)
    {
      for (auto i = 0; i < list->getElementCount(); i++)
      {
        auto name = this->get_link_name(list->getElement(i).s_cast<ParsedItem>());
        auto l = std::make_shared<ParsedList>(link->getProdId());
        l->setLine(link->getLine());
        l->setColumn(link->getColumn());
        l->pushElement(list->getElement(i));
        module->add(name, l);
      }
    }
    else
    {
      auto expr = exprBrowser.getChildValue<ParsedList>(link);
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


const Char* ModuleParsingHandler::get_link_name(const SharedPtr<ParsedItem> &link)
{
    static ParsedDataBrowser funcExpNoRetBrowser(
        STR("0:Expression.FunctionalExp"));
    static ParsedDataBrowser funcExpBrowser(
        STR("0:Expression.LowLinkExp>"
            "0:Expression.FunctionalExp"));
    static ParsedDataBrowser nameBrowser(STR("0:Subject.Subject1>0:Subject.Parameter"));
    static Word identifierTokenId = Core::Data::IdGenerator::getSingleton()->getId(STR("IDENTIFIER_TOKEN"));

    auto funcExp = funcExpNoRetBrowser.getChildValue<ParsedList>(link);
    if (funcExp == 0)
    {
      funcExp = funcExpBrowser.getChildValue<ParsedList>(link);
      if (funcExp == 0)
        THROW_EXCEPTION(InvalidArgumentException, "Invalid function link expression.");
    }
    auto nameToken = nameBrowser.getChildValue<ParsedToken>(funcExp);
    if (nameToken == 0 || nameToken->getId() != identifierTokenId)
      THROW_EXCEPTION(InvalidArgumentException,
          "Functional expressions should have the name of the function "
          "under the path 0:Subject.Subject1>0:Subject.Parameter.");
    return nameToken->getText().c_str();
}

} // namespace
