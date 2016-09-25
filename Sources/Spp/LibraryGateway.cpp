/**
 * @file Spp/LibraryGateway.cpp
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include "spp.h"

namespace Spp
{

using namespace Core;
using namespace Core::Data;
using namespace Core::Processing;
using namespace Core::Processing::Handlers;

//==============================================================================
// Overloaded Abstract Functions

void LibraryGateway::initialize(Standard::RootManager *manager)
{
  auto grammarRepository = manager->getGrammarRepository();
  auto leadingCmdList = this->getLeadingCommandsList(grammarRepository);
  auto innerCmdList = this->getInnerCommandsList(grammarRepository);

  // Create leading commands.

  //// dump = "dump" + Subject
  grammarRepository->set(STR("root:Main.Dump"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), std::make_shared<String>(STR("dump")) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:Subject")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, std::make_shared<Handlers::DumpParsingHandler>(manager) }
  }).get());
  this->addReferenceToCommandList(leadingCmdList, STR("module:Dump"));

  //// while = "while" + Expression + Statement
  grammarRepository->set(STR("root:Main.While"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("while"), 0 }, { STR("بينما"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            }),
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:BlockMain.Statement")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    {SymbolDefElement::HANDLER,
      std::make_shared<Handlers::WhileParsingHandler>()}
  }).get());
  this->addReferenceToCommandList(leadingCmdList, STR("module:While"));

  // Create inner command.

  //// module = "module" + Set
  grammarRepository->set(STR("root:Subject.Module"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    { SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("module"), 0 }, { STR("حزمة"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:ModuleBody")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
       auto currentList = state->getData().tii_cast_get<Data::Container>();
       state->setData(getSharedPtr(currentList->get(1)));
    })}
  }).get());
  this->addReferenceToCommandList(innerCmdList, STR("module:Module"));
  grammarRepository->set(STR("root:ModuleBody"), SymbolDefinition::create({
    { SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("root:Set")) },
    { SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("stmt"), REF_PARSER->parseQualifier(STR("root:Main.ModuleStatementList"))}
    })}
  }).get());
  grammarRepository->set(STR("root:Main.ModuleStatementList"), SymbolDefinition::create({
    { SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("module:StatementList")) },
    { SymbolDefElement::HANDLER, ScopeParsingHandler<Spp::Ast::Module>::create(-1) }
  }).get());

  //// type = "type" + Set
  grammarRepository->set(STR("root:Subject.Type"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    { SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("type"), 0 }, { STR("صنف"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:TypeBody")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
       auto currentList = state->getData().tii_cast_get<Data::Container>();
       state->setData(getSharedPtr(currentList->get(1)));
    })}
  }).get());
  this->addReferenceToCommandList(innerCmdList, STR("module:Type"));
  grammarRepository->set(STR("root:TypeBody"), SymbolDefinition::create({
    { SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("root:Set")) },
    { SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("stmt"), REF_PARSER->parseQualifier(STR("root:Main.TypeStatementList"))}
    })}
  }).get());
  grammarRepository->set(STR("root:Main.TypeStatementList"), SymbolDefinition::create({
    { SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("module:StatementList")) },
    { SymbolDefElement::HANDLER, ScopeParsingHandler<Spp::Ast::Type>::create(-1) }
  }).get());

  // Function
  grammarRepository->set(STR("root:Subject.Function"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("function"), 0 }, { STR("دالّة"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            }),
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:Block")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, std::make_shared<Handlers::FunctionParsingHandler>() }
  }).get());
  this->addReferenceToCommandList(innerCmdList, STR("module:Function"));

  // Block
  grammarRepository->set(STR("root:Block"), SymbolDefinition::create({
    { SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("root:Set")) },
    { SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("stmt"), REF_PARSER->parseQualifier(STR("root:Main.BlockStatementList"))}
    })}
  }).get());
  grammarRepository->set(STR("root:Main.BlockStatementList"), SymbolDefinition::create({
    { SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("module:StatementList")) },
    { SymbolDefElement::HANDLER, ScopeParsingHandler<Spp::Ast::Block>::create(-1) }
  }).get());

  // Block based statement.
  grammarRepository->set(STR("root:BlockSubject"), GrammarModule::create({
    { STR("@parent"), REF_PARSER->parseQualifier(STR("root:Subject")) }
  }).get());
  grammarRepository->set(STR("root:BlockSubject.Subject1"), SymbolDefinition::create({
   {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:Subject1")) },
   {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("sbj1"), SharedList::create({
         REF_PARSER->parseQualifier(STR("module:SubjectCmdGrp")),
         REF_PARSER->parseQualifier(STR("module:Parameter")),
         REF_PARSER->parseQualifier(STR("root:Block"))
       })}
    })}
  }).get());
  grammarRepository->set(STR("root:BlockExpression"), GrammarModule::create({
    { STR("@parent"), REF_PARSER->parseQualifier(STR("root:Expression")) }
  }).get());
  grammarRepository->set(STR("root:BlockExpression.FunctionalExp"), SymbolDefinition::create({
    {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:FunctionalExp")) },
    {SymbolDefElement::VARS, SharedMap::create(false, {
       {STR("operand"), REF_PARSER->parseQualifier(STR("root:BlockSubject"))},
       {STR("pty2"), std::make_shared<Integer>(1)},
       {STR("dup"), 0},
       {STR("fltr2"), 0}
     })}
  }).get());
  grammarRepository->set(STR("root:BlockMain"), GrammarModule::create({
    { STR("@parent"), REF_PARSER->parseQualifier(STR("root:Main")) }
  }).get());
  grammarRepository->set(STR("root:BlockMain.ExpPhrase"), SymbolDefinition::create({
   {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:ExpPhrase")) },
   {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("subjects"), SharedList::create({
         SharedMap::create(false, {
           {STR("prd"), REF_PARSER->parseQualifier(STR("root:BlockExpression"))},
           {STR("min"), std::make_shared<Integer>(1)},
           {STR("max"), std::make_shared<Integer>(1)},
           {STR("pty"), std::make_shared<Integer>(1)}
         })
       })}
    })}
  }).get());
}


void LibraryGateway::uninitialize(Standard::RootManager *manager)
{
  auto grammarRepository = manager->getGrammarRepository();
  auto leadingCmdList = this->getLeadingCommandsList(grammarRepository);
  auto innerCmdList = this->getInnerCommandsList(grammarRepository);

  // Remove commands from leading commands list.
  this->removeReferenceFromCommandList(leadingCmdList, STR("module:Dump"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module:While"));

  // Remove command from inner commands list.
  this->removeReferenceFromCommandList(innerCmdList, STR("module:Module"));
  this->removeReferenceFromCommandList(innerCmdList, STR("module:Type"));
  this->removeReferenceFromCommandList(innerCmdList, STR("module:Function"));

  // Delete definitions.
  grammarRepository->remove(STR("root:Main.Dump"));
  grammarRepository->remove(STR("root:Main.While"));
  grammarRepository->remove(STR("root:Subject.Module"));
  grammarRepository->remove(STR("root:ModuleBody"));
  grammarRepository->remove(STR("root:Subject.Type"));
  grammarRepository->remove(STR("root:TypeBody"));
  grammarRepository->remove(STR("root:Subject.Function"));
  grammarRepository->remove(STR("root:Block"));
  grammarRepository->remove(STR("root:BlockSubject"));
  grammarRepository->remove(STR("root:BlockExpression"));
  grammarRepository->remove(STR("root:BlockMain"));
}


SharedList* LibraryGateway::getLeadingCommandsList(GrammarRepository *grammarRepository)
{
  PlainPairedPtr retVal;
  grammarRepository->get(STR("root:Main.LeadingCmdGrp"), retVal.object,
                         Core::Data::Module::getTypeInfo(), &retVal.parent);
  SymbolDefinition *def = tio_cast<SymbolDefinition>(retVal.object);

  if (def == 0) {
    throw EXCEPTION(GenericException, STR("Could not find leading command group."));
  }

  GrammarContext context;
  context.setRoot(grammarRepository->getRoot().get());
  SharedMap *vars = context.getSymbolVars(def, static_cast<Core::Data::Module*>(retVal.parent));
  SharedList *cmd_list = tio_cast<SharedList>(vars->get(STR("cmds")));

  if (cmd_list == 0) {
    throw EXCEPTION(GenericException, STR("Could not find leading command group's command list."));
  }

  return cmd_list;
}


SharedList* LibraryGateway::getInnerCommandsList(GrammarRepository *grammarRepository)
{
  PlainPairedPtr retVal;
  grammarRepository->get(STR("root:Subject.SubjectCmdGrp"), retVal.object,
                         Core::Data::Module::getTypeInfo(), &retVal.parent);
  SymbolDefinition *def = tio_cast<SymbolDefinition>(retVal.object);

  if (def == 0) {
    throw EXCEPTION(GenericException, STR("Could not find inner command group."));
  }

  GrammarContext context;
  context.setRoot(grammarRepository->getRoot().get());
  SharedMap *vars = context.getSymbolVars(def, static_cast<Core::Data::Module*>(retVal.parent));
  SharedList *cmd_list = tio_cast<SharedList>(vars->get(STR("cmds")));

  if (cmd_list == 0) {
    throw EXCEPTION(GenericException, STR("Could not find inner command group's command list."));
  }

  return cmd_list;
}


SharedList* LibraryGateway::getTildeCommandsList(GrammarRepository *grammarRepository)
{
  PlainPairedPtr retVal;
  grammarRepository->get(STR("root:Expression.DefaultPostfixTildeCmd"), retVal.object,
                         Core::Data::Module::getTypeInfo(), &retVal.parent);
  SymbolDefinition *def = tio_cast<SymbolDefinition>(retVal.object);

  if (def == 0) {
    throw EXCEPTION(GenericException, STR("Could not find tilde command group."));
  }

  GrammarContext context;
  context.setRoot(grammarRepository->getRoot().get());
  SharedMap *vars = context.getSymbolVars(def, static_cast<Core::Data::Module*>(retVal.parent));
  SharedList *cmd_list = tio_cast<SharedList>(vars->get(STR("cmds")));

  if (cmd_list == 0) {
    throw EXCEPTION(GenericException, STR("Could not find inner command group's command list."));
  }

  return cmd_list;
}


void LibraryGateway::addReferenceToCommandList(Core::Data::SharedList *cmdList, Char const *qualifier)
{
  cmdList->add(REF_PARSER->parseQualifier(qualifier));
}


void LibraryGateway::removeReferenceFromCommandList(Core::Data::SharedList *cmdList, Char const *qualifier)
{
  for (Int i = 0; i < cmdList->getCount(); ++i) {
    auto reference = ti_cast<Core::Data::Reference>(cmdList->get(i));
    if (REF_PARSER->getQualifier(reference) == qualifier) {
      cmdList->remove(i);
      return;
    }
  }
}

} // namespace
