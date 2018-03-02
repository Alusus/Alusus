/**
 * @file Spp/LibraryGateway.cpp
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
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
  // Create AST helpers.
  this->nodePathResolver = std::make_shared<Ast::NodePathResolver>();
  this->astHelper = std::make_shared<Ast::Helper>(manager, this->nodePathResolver.get());

  // Extend Seeker.
  this->seekerExtensionOverrides = SeekerExtension::extend(manager->getSeeker(), this->astHelper);

  // Create the generator.
  this->noOpTargetGenerator = std::make_shared<CodeGen::NoOpTargetGenerator>();
  this->typeGenerator = std::make_shared<CodeGen::TypeGenerator>(this->astHelper.get());
  this->expressionGenerator = std::make_shared<CodeGen::ExpressionGenerator>(
    this->astHelper.get(), this->noOpTargetGenerator.get()
  );
  this->commandGenerator = std::make_shared<CodeGen::CommandGenerator>(this->astHelper.get());
  this->generator = std::make_shared<CodeGen::Generator>(
    manager,
    this->astHelper.get(),
    this->typeGenerator.get(),
    this->commandGenerator.get(),
    this->expressionGenerator.get()
  );
  this->targetGenerator = std::make_shared<LlvmCodeGen::TargetGenerator>();

  // Create leading commands.

  auto grammarRepository = manager->getGrammarRepository();
  auto leadingCmdList = this->getLeadingCommandsList(grammarRepository);
  auto innerCmdList = this->getInnerCommandsList(grammarRepository);
  auto tildeCmdList = this->getTildeCommandsList(grammarRepository);

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
    {SymbolDefElement::HANDLER, std::make_shared<Handlers::CodeGenParsingHandler>(
      manager, this->astHelper.get(), this->generator.get(), this->targetGenerator.get(), false
    )}
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
    {SymbolDefElement::HANDLER, std::make_shared<Handlers::CodeGenParsingHandler>(
      manager, this->astHelper.get(), this->generator.get(), this->targetGenerator.get(), true
    )}
  }).get());
  this->addReferenceToCommandList(leadingCmdList, STR("module:Run"));

  //// if = "if" + Expression + Statement + ("else" + Statement)*(0, 1)
  grammarRepository->set(STR("root:Main.If"), SymbolDefinition::create({
    {SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:MultiCmd"))},
    {SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
      {STR("sections"), Core::Data::SharedList::create({
        Core::Data::SharedMap::create(false, {
          {STR("min"), std::make_shared<Integer>(1)},
          {STR("max"), std::make_shared<Integer>(1)},
          {STR("pty"), std::make_shared<Integer>(1)},
          {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)},
          {STR("kwd"), Core::Data::SharedMap::create(false, { { STR("if"), 0 }, { STR("إذا"), 0 } })},
          {STR("args"), Core::Data::SharedList::create({
            Core::Data::SharedMap::create(false, {
              {STR("min"), std::make_shared<Integer>(1)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)},
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression"))}
            }),
            Core::Data::SharedMap::create(false, {
              {STR("min"), std::make_shared<Integer>(1)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)},
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:BlockMain.Statement"))}
            })
          })}
        }),
        Core::Data::SharedMap::create(false, {
          {STR("min"), std::make_shared<Integer>(0)},
          {STR("max"), std::make_shared<Integer>(1)},
          {STR("pty"), std::make_shared<Integer>(1)},
          {STR("flags"), Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {STR("kwd"), Core::Data::SharedMap::create(false, { { STR("else"), 0 }, { STR("وإلا"), 0 } })},
          {STR("args"), Core::Data::SharedList::create({
            Core::Data::SharedMap::create(false, {
              {STR("min"), std::make_shared<Integer>(1)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)},
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:BlockMain.Statement"))}
            })
          })}
        })
      })}
    })},
    {SymbolDefElement::HANDLER,
      std::make_shared<Handlers::IfParsingHandler>()}
  }).get());
  this->addReferenceToCommandList(leadingCmdList, STR("module:If"));

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
        { "sourceLocation", metadata->findSourceLocation() }
      });
      if (currentList != 0) {
        returnStatement->setOperand(getSharedPtr(currentList->get(1)));
      }
      state->setData(returnStatement);
    })}
  }).get());
  this->addReferenceToCommandList(leadingCmdList, STR("module:Return"));

  // Create inner commands.

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
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:Block"))},
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
      auto currentList = state->getData().ti_cast_get<Data::Container>();
      auto metadata = ti_cast<Data::Ast::Metadata>(currentList);
      auto type = Ast::UserType::create({
        { "prodId", metadata->getProdId() },
        { "sourceLocation", metadata->findSourceLocation() }
      }, {
        { "body", currentList->get(1) }
      });
      state->setData(type);
    })}
  }).get());
  this->addReferenceToCommandList(innerCmdList, STR("module:Type"));

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
      })},
      {STR("sbj2"), Core::Data::SharedList::create({REF_PARSER->parseQualifier(STR("root:Expression"))})},
      {STR("sbj3"), Core::Data::SharedList::create({REF_PARSER->parseQualifier(STR("root:Expression"))})},
      {STR("frc2"), 0},
      {STR("frc3"), 0}
    })}
  }).get());
  grammarRepository->set(STR("root:BlockExpression"), GrammarModule::create({
    { STR("@parent"), REF_PARSER->parseQualifier(STR("root:Expression")) }
  }).get());
  grammarRepository->set(STR("root:BlockExpression.FunctionalExp"), SymbolDefinition::create({
    {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:FunctionalExp")) },
    {SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
      {STR("flags"), Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
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

  // Create tilde commands.

  // ~ptr
  grammarRepository->set(STR("root:Expression.PointerTilde"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
        { STR("kwd"), Core::Data::SharedMap::create(false, { { STR("ptr"), 0 }, { STR("مؤشر"), 0 } }) },
        { STR("prms"), Core::Data::SharedList::create({}) }
      })
    },
    { SymbolDefElement::HANDLER, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::PointerOp>::create() }
  }).get());
  this->addReferenceToCommandList(tildeCmdList, STR("module:PointerTilde"));
  // ~cnt
  grammarRepository->set(STR("root:Expression.ContentTilde"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
        { STR("kwd"), Core::Data::SharedMap::create(false, { { STR("cnt"), 0 }, { STR("محتوى"), 0 } }) },
        { STR("prms"), Core::Data::SharedList::create({}) }
      })
    },
    { SymbolDefElement::HANDLER, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::ContentOp>::create() }
  }).get());
  this->addReferenceToCommandList(tildeCmdList, STR("module:ContentTilde"));
  // ~cast
  grammarRepository->set(STR("root:Expression.CastTilde"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
        { STR("kwd"), Core::Data::SharedMap::create(false, {{STR("cast"), 0}, {STR("مثّل"), 0}, {STR("مثل"), 0}}) },
        {
          STR("prms"), Core::Data::SharedList::create({
            Core::Data::SharedMap::create(false, {
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:CastSubject"))},
              {STR("min"), std::make_shared<Integer>(1)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)}
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::CastOp>::create() }
  }).get());
  this->addReferenceToCommandList(tildeCmdList, STR("module:CastTilde"));
  grammarRepository->set(STR("root:CastSubject"), GrammarModule::create({
    { STR("@parent"), REF_PARSER->parseQualifier(STR("root:Subject")) },
    { STR("@start"), REF_PARSER->parseQualifier(STR("module:Subject2")) }
  }).get());
  grammarRepository->set(STR("root:CastSubject.Subject2"), SymbolDefinition::create({
   {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:Subject2")) },
   {SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
      {STR("sbj"), REF_PARSER->parseQualifier(STR("root:Expression"))},
      {STR("fltr"), std::make_shared<Integer>(2)},
      {STR("frc"), std::make_shared<Integer>(1)}
    })}
  }).get());
  // ~size
  grammarRepository->set(STR("root:Expression.SizeTilde"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
        { STR("kwd"), Core::Data::SharedMap::create(false, { { STR("size"), 0 }, { STR("حجم"), 0 } }) },
        { STR("prms"), Core::Data::SharedList::create({}) }
      })
    },
    { SymbolDefElement::HANDLER, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::SizeOp>::create() }
  }).get());
  this->addReferenceToCommandList(tildeCmdList, STR("module:SizeTilde"));

  this->createBuiltInTypes(manager);
  this->createBuiltInFunctions(manager);
}


void LibraryGateway::uninitialize(Standard::RootManager *manager)
{
  // Unextend Seeker.
  SeekerExtension::unextend(manager->getSeeker(), this->seekerExtensionOverrides);
  this->seekerExtensionOverrides = 0;

  this->targetGenerator.reset();
  this->generator.reset();
  this->commandGenerator.reset();
  this->expressionGenerator.reset();
  this->typeGenerator.reset();
  this->noOpTargetGenerator.reset();
  this->nodePathResolver.reset();
  this->astHelper.reset();

  auto grammarRepository = manager->getGrammarRepository();
  auto leadingCmdList = this->getLeadingCommandsList(grammarRepository);
  auto innerCmdList = this->getInnerCommandsList(grammarRepository);
  auto tildeCmdList = this->getTildeCommandsList(grammarRepository);

  // Remove commands from tilde commands list.
  this->removeReferenceFromCommandList(tildeCmdList, STR("module:SizeTilde"));
  this->removeReferenceFromCommandList(tildeCmdList, STR("module:CastTilde"));
  this->removeReferenceFromCommandList(tildeCmdList, STR("module:ContentTilde"));
  this->removeReferenceFromCommandList(tildeCmdList, STR("module:PointerTilde"));

  // Remove commands from leading commands list.
  this->removeReferenceFromCommandList(leadingCmdList, STR("module:Build"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module:Run"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module:If"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module:While"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module:Return"));

  // Remove command from inner commands list.
  this->removeReferenceFromCommandList(innerCmdList, STR("module:Module"));
  this->removeReferenceFromCommandList(innerCmdList, STR("module:Type"));
  this->removeReferenceFromCommandList(innerCmdList, STR("module:Function"));

  // Delete definitions.
  grammarRepository->remove(STR("root:Main.Build"));
  grammarRepository->remove(STR("root:Main.Run"));
  grammarRepository->remove(STR("root:Main.If"));
  grammarRepository->remove(STR("root:Main.While"));
  grammarRepository->remove(STR("root:Main.Return"));
  grammarRepository->remove(STR("root:Main.ModuleStatementList"));
  grammarRepository->remove(STR("root:Main.BlockStatementList"));
  grammarRepository->remove(STR("root:Subject.Module"));
  grammarRepository->remove(STR("root:ModuleBody"));
  grammarRepository->remove(STR("root:Subject.Type"));
  grammarRepository->remove(STR("root:Subject.Function"));
  grammarRepository->remove(STR("root:FuncSigExpression"));
  grammarRepository->remove(STR("root:Block"));
  grammarRepository->remove(STR("root:BlockSubject"));
  grammarRepository->remove(STR("root:BlockExpression"));
  grammarRepository->remove(STR("root:BlockMain"));
  grammarRepository->remove(STR("root:Expression.PointerTilde"));
  grammarRepository->remove(STR("root:Expression.ContentTilde"));
  grammarRepository->remove(STR("root:Expression.SizeTilde"));
  grammarRepository->remove(STR("root:Expression.CastTilde"));
  grammarRepository->remove(STR("root:CastSubject"));

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

  // ref
  tmplt = Ast::Template::create();
  tmplt->setVarDefs({{ STR("type"), Ast::Template::VarType::TYPE }});
  tmplt->setTemplateBody(Ast::ReferenceType::create());
  identifier.setValue(STR("ref"));
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

  identifier.setValue(STR("ref"));
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
  Char const *intBinaryOps[] = {
    STR("rem"), STR("shr"), STR("shl"), STR("and"), STR("or"), STR("xor")
  };
  Char const *comparisonOps[] = {
    STR("equal"), STR("notEqual"),
    STR("greaterThan"), STR("greaterThanOrEqual"),
    STR("lessThan"), STR("lessThanOrEqual")
  };
  Char const *assignmentOps[] = {
    STR("addAssign"), STR("subAssign"), STR("mulAssign"), STR("divAssign")
  };
  Char const *intAssignmentOps[] = {
    STR("remAssign"), STR("shrAssign"), STR("shlAssign"), STR("andAssign"), STR("orAssign"), STR("xorAssign")
  };
  Char const *unaryValOps[] = {
    STR("neg"), STR("pos")
  };
  Char const *intUnaryValOps[] = {
    STR("not")
  };
  Char const *unaryVarOps[] = {
    STR("earlyInc"), STR("earlyDec"), STR("lateInc"), STR("lateDec")
  };

  Char const *types[] = {
    STR("Int[8]"), STR("Int[16]"), STR("Int[32]"), STR("Int[64]"), STR("Float[32]"), STR("Float[64]")
  };
  Char const *refTypes[] = {
    STR("ref[Int[8]]"), STR("ref[Int[16]]"), STR("ref[Int[32]]"), STR("ref[Int[64]]"),
    STR("ref[Float[32]]"), STR("ref[Float[64]]")
  };
  Char const *intTypes[] = {
    STR("Int[8]"), STR("Int[16]"), STR("Int[32]"), STR("Int[64]")
  };
  Char const *intRefTypes[] = {
    STR("ref[Int[8]]"), STR("ref[Int[16]]"), STR("ref[Int[32]]"), STR("ref[Int[64]]")
  };

  // Add binary math operators.
  for (Int i = 0; i < sizeof(binaryOps) / sizeof(binaryOps[0]); ++i) {
    Str path = STR("__");
    path += binaryOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(types) / sizeof(types[0]); ++j) {
      Str funcName = STR("#");
      funcName += binaryOps[i];

      manager->getSeeker()->set(&identifier, root, [=,&hook](TiObject *&obj, Notice*)->Core::Data::Seeker::Verb {
        if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
        hook = this->createBinaryFunction(manager, funcName.c_str(), types[j], types[j], types[j]);
        obj = hook.get();
        return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
      }, 0);
    }
  }

  // Add int-only binary math operators.
  for (Int i = 0; i < sizeof(intBinaryOps) / sizeof(intBinaryOps[0]); ++i) {
    Str path = STR("__");
    path += intBinaryOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(intTypes) / sizeof(intTypes[0]); ++j) {
      Str funcName = STR("#");
      funcName += intBinaryOps[i];

      manager->getSeeker()->set(&identifier, root, [=,&hook](TiObject *&obj, Notice*)->Core::Data::Seeker::Verb {
        if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
        hook = this->createBinaryFunction(manager, funcName.c_str(), intTypes[j], intTypes[j], intTypes[j]);
        obj = hook.get();
        return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
      }, 0);
    }
  }

  // Add comparison operators.
  for (Int i = 0; i < sizeof(comparisonOps) / sizeof(comparisonOps[0]); ++i) {
    Str path = STR("__");
    path += comparisonOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(types) / sizeof(types[0]); ++j) {
      Str funcName = STR("#");
      funcName += comparisonOps[i];

      manager->getSeeker()->set(&identifier, root, [=,&hook](TiObject *&obj, Notice*)->Core::Data::Seeker::Verb {
        if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
        hook = this->createBinaryFunction(manager, funcName.c_str(), types[j], types[j], STR("Int[1]"));
        obj = hook.get();
        return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
      }, 0);
    }
  }

  // Add assignment operators.
  for (Int i = 0; i < sizeof(assignmentOps) / sizeof(assignmentOps[0]); ++i) {
    Str path = STR("__");
    path += assignmentOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(types) / sizeof(types[0]); ++j) {
      Str funcName = STR("#");
      funcName += assignmentOps[i];

      manager->getSeeker()->set(&identifier, root, [=,&hook](TiObject *&obj, Notice*)->Core::Data::Seeker::Verb {
        if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
        hook = this->createBinaryFunction(manager, funcName.c_str(), refTypes[j], types[j], refTypes[j]);
        obj = hook.get();
        return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
      }, 0);
    }
  }

  // Add int-only assignment operators.
  for (Int i = 0; i < sizeof(intAssignmentOps) / sizeof(intAssignmentOps[0]); ++i) {
    Str path = STR("__");
    path += intAssignmentOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(intTypes) / sizeof(intTypes[0]); ++j) {
      Str funcName = STR("#");
      funcName += intAssignmentOps[i];

      manager->getSeeker()->set(&identifier, root, [=,&hook](TiObject *&obj, Notice*)->Core::Data::Seeker::Verb {
        if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
        hook = this->createBinaryFunction(manager, funcName.c_str(), intRefTypes[j], intTypes[j], intRefTypes[j]);
        obj = hook.get();
        return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
      }, 0);
    }
  }

  // Add unary val operators.
  for (Int i = 0; i < sizeof(unaryValOps) / sizeof(unaryValOps[0]); ++i) {
    Str path = STR("__");
    path += unaryValOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(types) / sizeof(types[0]); ++j) {
      Str funcName = STR("#");
      funcName += unaryValOps[i];

      manager->getSeeker()->set(&identifier, root, [=,&hook](TiObject *&obj, Notice*)->Core::Data::Seeker::Verb {
        if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
        hook = this->createUnaryFunction(manager, funcName.c_str(), types[j], types[j]);
        obj = hook.get();
        return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
      }, 0);
    }
  }

  // Add int-only unary val operators.
  for (Int i = 0; i < sizeof(intUnaryValOps) / sizeof(intUnaryValOps[0]); ++i) {
    Str path = STR("__");
    path += intUnaryValOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(intTypes) / sizeof(intTypes[0]); ++j) {
      Str funcName = STR("#");
      funcName += intUnaryValOps[i];

      manager->getSeeker()->set(&identifier, root, [=,&hook](TiObject *&obj, Notice*)->Core::Data::Seeker::Verb {
        if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
        hook = this->createUnaryFunction(manager, funcName.c_str(), intTypes[j], intTypes[j]);
        obj = hook.get();
        return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
      }, 0);
    }
  }

  // Add unary var operators.
  for (Int i = 0; i < sizeof(unaryVarOps) / sizeof(unaryVarOps[0]); ++i) {
    Str path = STR("__");
    path += unaryVarOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(types) / sizeof(types[0]); ++j) {
      Str funcName = STR("#");
      funcName += unaryVarOps[i];

      manager->getSeeker()->set(&identifier, root, [=,&hook](TiObject *&obj, Notice*)->Core::Data::Seeker::Verb {
        if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
        hook = this->createUnaryFunction(manager, funcName.c_str(), refTypes[j], types[j]);
        obj = hook.get();
        return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
      }, 0);
    }
  }
}


void LibraryGateway::removeBuiltInFunctions(Core::Standard::RootManager *manager)
{
  Core::Data::Ast::Identifier identifier;
  auto root = manager->getRootScope().get();

  Char const *operations[] = {
    STR("add"), STR("sub"), STR("mul"), STR("div"),
    STR("rem"), STR("shr"), STR("shl"), STR("and"), STR("or"), STR("xor"),
    STR("equal"), STR("notEqual"),
    STR("greaterThan"), STR("greaterThanOrEqual"),
    STR("lessThan"), STR("lessThanOrEqual"),
    STR("addAssign"), STR("subAssign"), STR("mulAssign"), STR("divAssign"),
    STR("remAssign"), STR("shrAssign"), STR("shlAssign"), STR("andAssign"), STR("orAssign"), STR("xorAssign"),
    STR("neg"), STR("pos"),
    STR("not"),
    STR("earlyInc"), STR("earlyDec"), STR("lateInc"), STR("lateDec")
  };

  for (Int i = 0; i < sizeof(operations) / sizeof(operations[0]); ++i) {
    Str name = STR("__");
    name += operations[i];
    identifier.setValue(name.c_str());
    manager->getSeeker()->doRemove(&identifier, root);
  }
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
