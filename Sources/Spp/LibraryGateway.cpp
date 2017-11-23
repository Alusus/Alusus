/**
 * @file Spp/LibraryGateway.cpp
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
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
  // Extend Seeker.
  this->seekerExtensionOverrides = SeekerExtension::extend(manager->getSeeker());

  // Create the generator.
  this->nodePathResolver = new NodePathResolver(manager->getSeeker());
  this->llvmTypeGenerator = new LlvmCodeGen::TypeGenerator();
  this->llvmLiteralGenerator = new LlvmCodeGen::LiteralGenerator();
  this->llvmExpressionGenerator = new LlvmCodeGen::ExpressionGenerator();
  this->llvmVariableGenerator = new LlvmCodeGen::VariableGenerator();
  this->llvmCommandGenerator = new LlvmCodeGen::CommandGenerator();
  this->llvmGenerator = new LlvmCodeGen::Generator(
    manager,
    this->nodePathResolver,
    this->llvmTypeGenerator,
    this->llvmLiteralGenerator,
    this->llvmExpressionGenerator,
    this->llvmVariableGenerator,
    this->llvmCommandGenerator
  );
  this->llvmTypeGenerator->setGenerator(this->llvmGenerator);
  this->llvmLiteralGenerator->setGenerator(this->llvmGenerator);
  this->llvmExpressionGenerator->setGenerator(this->llvmGenerator);
  this->llvmVariableGenerator->setGenerator(this->llvmGenerator);
  this->llvmCommandGenerator->setGenerator(this->llvmGenerator);

  // Create leading commands.

  auto grammarRepository = manager->getGrammarRepository();
  auto leadingCmdList = this->getLeadingCommandsList(grammarRepository);
  auto innerCmdList = this->getInnerCommandsList(grammarRepository);

  //// build = "build" + Subject
  grammarRepository->set(STR("root:Main.Build"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
        { STR("kwd"), Core::Data::SharedMap::create(false, { { STR("build"), 0 }, { STR("أنشئ"), 0 } }) },
        {
          STR("prms"), Core::Data::SharedList::create({
            Core::Data::SharedMap::create(false, {
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression"))},
              {STR("min"), std::make_shared<Integer>(1)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)}
            })
          })
        }
      })
    },
    {SymbolDefElement::HANDLER, std::make_shared<Handlers::CodeGenParsingHandler>(manager, this->llvmGenerator, false)}
  }).get());
  this->addReferenceToCommandList(leadingCmdList, STR("module:Build"));

  //// run = "run" + Subject
  grammarRepository->set(STR("root:Main.Run"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
        { STR("kwd"), Core::Data::SharedMap::create(false, { { STR("run"), 0 }, { STR("نفّذ"), 0 } }) },
        {
          STR("prms"), Core::Data::SharedList::create({
            Core::Data::SharedMap::create(false, {
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression"))},
              {STR("min"), std::make_shared<Integer>(1)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)}
            })
          })
        }
      })
    },
    {SymbolDefElement::HANDLER, std::make_shared<Handlers::CodeGenParsingHandler>(manager, this->llvmGenerator, true)}
  }).get());
  this->addReferenceToCommandList(leadingCmdList, STR("module:Run"));

  //// while = "while" + Expression + Statement
  grammarRepository->set(STR("root:Main.While"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
        { STR("kwd"), Core::Data::SharedMap::create(false, { { STR("while"), 0 }, { STR("بينما"), 0 } }) },
        {
          STR("prms"), Core::Data::SharedList::create({
            Core::Data::SharedMap::create(false, {
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression"))},
              {STR("min"), std::make_shared<Integer>(1)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)}
            }),
            Core::Data::SharedMap::create(false, {
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:BlockMain.Statement"))},
              {STR("min"), std::make_shared<Integer>(1)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)}
            })
          })
        }
      })
    },
    {SymbolDefElement::HANDLER,
      std::make_shared<Handlers::WhileParsingHandler>()}
  }).get());
  this->addReferenceToCommandList(leadingCmdList, STR("module:While"));

  //// return = "return" + Expression
  grammarRepository->set(STR("root:Main.Return"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    { SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
        { STR("kwd"), Core::Data::SharedMap::create(false, { { STR("return"), 0 }, { STR("أرجع"), 0 } }) },
        {
          STR("prms"), Core::Data::SharedList::create({
            Core::Data::SharedMap::create(false, {
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression"))},
              {STR("min"), std::make_shared<Integer>(0)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)}
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto metadata = state->getData().ti_cast_get<Data::Ast::Metadata>();
      auto currentList = state->getData().ti_cast_get<Data::Container>();
      auto returnStatement = Ast::ReturnStatement::create({
        { "prodId", metadata->getProdId() },
        { "sourceLocation", metadata->getSourceLocation() }
      });
      if (currentList != 0) {
        returnStatement->setOperand(getSharedPtr(currentList->get(1)));
      }
      state->setData(returnStatement);
    })}
  }).get());
  this->addReferenceToCommandList(leadingCmdList, STR("module:Return"));

  // Create inner command.

  //// module = "module" + Set
  grammarRepository->set(STR("root:Subject.Module"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    { SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
        { STR("kwd"), Core::Data::SharedMap::create(false, { { STR("module"), 0 }, { STR("حزمة"), 0 } }) },
        {
          STR("prms"), Core::Data::SharedList::create({
            Core::Data::SharedMap::create(false, {
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:ModuleBody"))},
              {STR("min"), std::make_shared<Integer>(1)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)}
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
    { SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
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
    { SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
        { STR("kwd"), Core::Data::SharedMap::create(false, { { STR("type"), 0 }, { STR("صنف"), 0 } }) },
        {
          STR("prms"), Core::Data::SharedList::create({
            Core::Data::SharedMap::create(false, {
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:TypeBody"))},
              {STR("min"), std::make_shared<Integer>(1)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)}
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
    { SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
      {STR("stmt"), REF_PARSER->parseQualifier(STR("root:Main.TypeStatementList"))}
    })}
  }).get());
  grammarRepository->set(STR("root:Main.TypeStatementList"), SymbolDefinition::create({
    { SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("module:StatementList")) },
    { SymbolDefElement::HANDLER, ScopeParsingHandler<Spp::Ast::UserType>::create(-1) }
  }).get());

  // Function
  grammarRepository->set(STR("root:Subject.Function"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
        { STR("kwd"), Core::Data::SharedMap::create(false, { { STR("function"), 0 }, { STR("دالّة"), 0 } }) },
        {
          STR("prms"), Core::Data::SharedList::create({
            Core::Data::SharedMap::create(false, {
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:FuncSigExpression"))},
              {STR("min"), std::make_shared<Integer>(1)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)}
            }),
            Core::Data::SharedMap::create(false, {
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:Block"))},
              {STR("min"), std::make_shared<Integer>(0)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)}
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, std::make_shared<Handlers::FunctionParsingHandler>() }
  }).get());
  this->addReferenceToCommandList(innerCmdList, STR("module:Function"));

  // FuncSigExpression
  grammarRepository->set(STR("root:FuncSigExpression"), GrammarModule::create({
    { STR("@start"), REF_PARSER->parseQualifier(STR("module:LowLinkExp")) },
    { STR("@parent"), REF_PARSER->parseQualifier(STR("root:Expression")) }
  }).get());
  grammarRepository->set(STR("root:FuncSigExpression.LowLinkExp"), SymbolDefinition::create({
    {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:LowLinkExp"))},
    {SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {{STR("enable"), std::make_shared<Integer>(1)}})},
  }).get());
  grammarRepository->set(STR("root:FuncSigExpression.AddExp"), SymbolDefinition::create({
    {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:AddExp"))},
    {SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {{STR("enable"), std::make_shared<Integer>(0)}})},
  }).get());
  grammarRepository->set(STR("root:FuncSigExpression.MulExp"), SymbolDefinition::create({
    {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:MulExp"))},
    {SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {{STR("enable"), std::make_shared<Integer>(0)}})},
  }).get());
  grammarRepository->set(STR("root:FuncSigExpression.BitwiseExp"), SymbolDefinition::create({
    {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:BitwiseExp"))},
    {SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {{STR("enable"), std::make_shared<Integer>(0)}})},
  }).get());
  grammarRepository->set(STR("root:FuncSigExpression.UnaryExp"), SymbolDefinition::create({
    {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:UnaryExp"))},
    {SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
      {STR("enable1"), std::make_shared<Integer>(0)},
      {STR("enable2"), std::make_shared<Integer>(0)}
    })},
  }).get());
  grammarRepository->set(STR("root:FuncSigExpression.FunctionalExp"), SymbolDefinition::create({
    {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:FunctionalExp")) },
    {SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
      {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP | TermFlags::ONE_ROUTE_TERM)},
      {STR("operand"), REF_PARSER->parseQualifier(STR("root:Subject"))},
      {STR("pty2"), std::make_shared<Integer>(1)},
      {STR("dup"), 0},
      {STR("fltr2"), 0}
    })}
  }).get());

  // Block
  grammarRepository->set(STR("root:Block"), SymbolDefinition::create({
    { SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("root:Set")) },
    { SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
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
   {SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
      {STR("sbj1"), Core::Data::SharedList::create({
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
    {SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
      {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)},
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
   {SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
      {STR("subjects"), Core::Data::SharedList::create({
         Core::Data::SharedMap::create(false, {
           {STR("prd"), REF_PARSER->parseQualifier(STR("root:BlockExpression"))},
           {STR("min"), std::make_shared<Integer>(1)},
           {STR("max"), std::make_shared<Integer>(1)},
           {STR("pty"), std::make_shared<Integer>(1)}
         })
       })}
    })}
  }).get());

  this->createBuiltInTypes(manager);
  this->createBuiltInFunctions(manager);
}


void LibraryGateway::uninitialize(Standard::RootManager *manager)
{
  // Unextend Seeker.
  SeekerExtension::unextend(manager->getSeeker(), this->seekerExtensionOverrides);
  this->seekerExtensionOverrides = 0;

  delete this->llvmGenerator;
  this->llvmGenerator = 0;
  delete this->llvmLiteralGenerator;
  this->llvmLiteralGenerator = 0;
  delete this->llvmTypeGenerator;
  this->llvmTypeGenerator = 0;
  delete this->nodePathResolver;
  this->nodePathResolver = 0;

  auto grammarRepository = manager->getGrammarRepository();
  auto leadingCmdList = this->getLeadingCommandsList(grammarRepository);
  auto innerCmdList = this->getInnerCommandsList(grammarRepository);

  // Remove commands from leading commands list.
  this->removeReferenceFromCommandList(leadingCmdList, STR("module:Build"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module:Run"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module:While"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module:Return"));

  // Remove command from inner commands list.
  this->removeReferenceFromCommandList(innerCmdList, STR("module:Module"));
  this->removeReferenceFromCommandList(innerCmdList, STR("module:Type"));
  this->removeReferenceFromCommandList(innerCmdList, STR("module:Function"));

  // Delete definitions.
  grammarRepository->remove(STR("root:Main.Build"));
  grammarRepository->remove(STR("root:Main.Run"));
  grammarRepository->remove(STR("root:Main.While"));
  grammarRepository->remove(STR("root:Main.Return"));
  grammarRepository->remove(STR("root:Main.ModuleStatementList"));
  grammarRepository->remove(STR("root:Main.TypeStatementList"));
  grammarRepository->remove(STR("root:Main.BlockStatementList"));
  grammarRepository->remove(STR("root:Subject.Module"));
  grammarRepository->remove(STR("root:ModuleBody"));
  grammarRepository->remove(STR("root:Subject.Type"));
  grammarRepository->remove(STR("root:TypeBody"));
  grammarRepository->remove(STR("root:Subject.Function"));
  grammarRepository->remove(STR("root:FuncSigExpression"));
  grammarRepository->remove(STR("root:Block"));
  grammarRepository->remove(STR("root:BlockSubject"));
  grammarRepository->remove(STR("root:BlockExpression"));
  grammarRepository->remove(STR("root:BlockMain"));

  this->removeBuiltInFunctions(manager);
  this->removeBuiltInTypes(manager);
}


Core::Data::SharedList* LibraryGateway::getLeadingCommandsList(GrammarRepository *grammarRepository)
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
  Core::Data::SharedMap *vars = context.getSymbolVars(def, static_cast<Core::Data::Module*>(retVal.parent));
  Core::Data::SharedList *cmd_list = ti_cast<Core::Data::SharedList>(vars->get(STR("cmds")));

  if (cmd_list == 0) {
    throw EXCEPTION(GenericException, STR("Could not find leading command group's command list."));
  }

  return cmd_list;
}


Core::Data::SharedList* LibraryGateway::getInnerCommandsList(GrammarRepository *grammarRepository)
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
  Core::Data::SharedMap *vars = context.getSymbolVars(def, static_cast<Core::Data::Module*>(retVal.parent));
  Core::Data::SharedList *cmd_list = ti_cast<Core::Data::SharedList>(vars->get(STR("cmds")));

  if (cmd_list == 0) {
    throw EXCEPTION(GenericException, STR("Could not find inner command group's command list."));
  }

  return cmd_list;
}


Core::Data::SharedList* LibraryGateway::getTildeCommandsList(GrammarRepository *grammarRepository)
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
  Core::Data::SharedMap *vars = context.getSymbolVars(def, static_cast<Core::Data::Module*>(retVal.parent));
  Core::Data::SharedList *cmd_list = tio_cast<Core::Data::SharedList>(vars->get(STR("cmds")));

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


void LibraryGateway::createBuiltInTypes(Core::Standard::RootManager *manager)
{
  Core::Data::Ast::Identifier identifier;
  auto root = manager->getRootScope().get();
  SharedPtr<Ast::Template> tmplt;

  // Void
  identifier.setValue(STR("Void"));
  manager->getSeeker()->doSet(&identifier, root, Ast::VoidType::create().get());

  // Int
  tmplt = Ast::Template::create();
  tmplt->setVarDefs({{ STR("bitCount"), Ast::Template::VarType::INTEGER }});
  tmplt->setTemplateBody(Ast::IntegerType::create());
  identifier.setValue(STR("Int"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // Float
  tmplt = Ast::Template::create();
  tmplt->setVarDefs({{ STR("bitCount"), Ast::Template::VarType::INTEGER }});
  tmplt->setTemplateBody(Ast::FloatType::create());
  identifier.setValue(STR("Float"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // ptr
  tmplt = Ast::Template::create();
  tmplt->setVarDefs({{ STR("type"), Ast::Template::VarType::TYPE }});
  tmplt->setTemplateBody(Ast::PointerType::create());
  identifier.setValue(STR("ptr"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // array
  tmplt = Ast::Template::create();
  tmplt->setVarDefs({
    { STR("type"), Ast::Template::VarType::TYPE },
    { STR("size"), Ast::Template::VarType::INTEGER }
  });
  tmplt->setTemplateBody(Ast::ArrayType::create());
  identifier.setValue(STR("array"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());
}


void LibraryGateway::removeBuiltInTypes(Core::Standard::RootManager *manager)
{
  Core::Data::Ast::Identifier identifier;
  auto root = manager->getRootScope().get();

  identifier.setValue(STR("Void"));
  manager->getSeeker()->doRemove(&identifier, root);

  identifier.setValue(STR("Int"));
  manager->getSeeker()->doRemove(&identifier, root);

  identifier.setValue(STR("Float"));
  manager->getSeeker()->doRemove(&identifier, root);

  identifier.setValue(STR("ptr"));
  manager->getSeeker()->doRemove(&identifier, root);

  identifier.setValue(STR("array"));
  manager->getSeeker()->doRemove(&identifier, root);
}


void LibraryGateway::createBuiltInFunctions(Core::Standard::RootManager *manager)
{
  Core::Data::Ast::Identifier identifier;
  auto root = manager->getRootScope().get();
  TioSharedPtr hook;

  Char const *binaryOps[] = { STR("add"), STR("sub"), STR("mul"), STR("div") };
  Char const *types[] = {
    STR("Int[8]"), STR("Int[16]"), STR("Int[32]"), STR("Int[64]"), STR("Float[32]"), STR("Float[64]")
  };

  for (Int i = 0; i < sizeof(binaryOps) / sizeof(binaryOps[0]); ++i) {
    Str path = STR("__");
    path += binaryOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(types) / sizeof(types[0]); ++j) {
      Int length = SBSTR(types[j]).findPos(CHR('['));
      Str funcName = STR("#");
      funcName += binaryOps[i];
      funcName.append(types[j], length);

      manager->getSeeker()->set(&identifier, root, [=,&hook](TiObject *&obj, Notice*)->Core::Data::Seeker::Verb {
        if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
        hook = this->createBinaryFunction(manager, funcName.c_str(), types[j], types[j], types[j]);
        obj = hook.get();
        return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
      });
    }
  }

  identifier.setValue(STR("__neg"));
  for (Int j = 0; j < sizeof(types) / sizeof(types[0]); ++j) {
    Int length = SBSTR(types[j]).findPos(CHR('['));
    Str funcName = STR("#");
    funcName += STR("neg");
    funcName.append(types[j], length);

    manager->getSeeker()->set(&identifier, root, [=,&hook](TiObject *&obj, Notice*)->Core::Data::Seeker::Verb {
      if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
      hook = this->createUnaryFunction(manager, funcName.c_str(), types[j], types[j]);
      obj = hook.get();
      return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
    });
  }
}


void LibraryGateway::removeBuiltInFunctions(Core::Standard::RootManager *manager)
{
  Core::Data::Ast::Identifier identifier;
  auto root = manager->getRootScope().get();

  identifier.setValue(STR("__add"));
  manager->getSeeker()->doRemove(&identifier, root);

  identifier.setValue(STR("__sub"));
  manager->getSeeker()->doRemove(&identifier, root);

  identifier.setValue(STR("__mul"));
  manager->getSeeker()->doRemove(&identifier, root);

  identifier.setValue(STR("__div"));
  manager->getSeeker()->doRemove(&identifier, root);

  identifier.setValue(STR("__neg"));
  manager->getSeeker()->doRemove(&identifier, root);
}


SharedPtr<Ast::Function> LibraryGateway::createBinaryFunction(
  Core::Standard::RootManager *manager, Char const *name, Char const *in1, Char const *in2, Char const *out
) {
  auto retType = manager->parseExpression(out);
  auto argTypes = Core::Data::SharedMap::create(false, {
    { STR("in1"), manager->parseExpression(in1) },
    { STR("in2"), manager->parseExpression(in2) }
  });
  return Ast::Function::create({
    { STR("name"), TiStr(name) }
  }, {
    { STR("argTypes"), argTypes },
    { STR("retType"), retType }
  });
}


SharedPtr<Ast::Function> LibraryGateway::createUnaryFunction(
  Core::Standard::RootManager *manager, Char const *name, Char const *in, Char const *out
) {
  auto retType = manager->parseExpression(out);
  auto argTypes = Core::Data::SharedMap::create(false, {
    { STR("in"), manager->parseExpression(in) }
  });
  return Ast::Function::create({
    { STR("name"), TiStr(name) }
  }, {
    { STR("argTypes"), argTypes },
    { STR("retType"), retType }
  });
}

} // namespace
