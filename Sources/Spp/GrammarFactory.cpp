/**
 * @file Spp/GrammarFactory.cpp
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
using namespace Core::Data::Grammar;
using namespace Core::Processing;
using namespace Core::Processing::Handlers;

//==============================================================================
// Overloaded Abstract Functions

void GrammarFactory::createGrammar(
  Core::Data::Ast::Scope *rootScope, Standard::RootManager *manager, Ast::Helper *astHelper,
  CodeGen::Generator *generator, LlvmCodeGen::TargetGenerator *targetGenerator
) {
  this->setRootScope(rootScope);

  // Create leading commands.

  auto leadingCmdList = this->getLeadingCommandsList();
  auto innerCmdList = this->getInnerCommandsList();
  auto tildeCmdList = this->getTildeCommandsList();

  //// DumpIr = "dump_ir" + Subject
  this->set(STR("root.Main.DumpLlvmIr"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, { { STR("dump_llvm_ir"), 0 }, { STR("أدرج_ت_و"), 0 } })},
      {STR("prms"), List::create({}, {
        Map::create(false, {}, {
          {STR("prd"), PARSE_REF(STR("root.Expression"))},
          {STR("min"), std::make_shared<TiInt>(1)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {STR("handler"), std::make_shared<Handlers::CodeGenParsingHandler>(
      manager, astHelper, generator, targetGenerator, false
    )}
  }).get());
  leadingCmdList->add(PARSE_REF(STR("module.DumpLlvmIr")));

  //// run = "run" + Subject
  this->set(STR("root.Main.Run"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, { { STR("run"), 0 }, { STR("نفّذ"), 0 } })},
      {STR("prms"), List::create({}, {
        Map::create(false, {}, {
          {STR("prd"), PARSE_REF(STR("root.Expression"))},
          {STR("min"), std::make_shared<TiInt>(1)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {STR("handler"), std::make_shared<Handlers::CodeGenParsingHandler>(
      manager, astHelper, generator, targetGenerator, true
    )}
  }).get());
  leadingCmdList->add(PARSE_REF(STR("module.Run")));

  //// if = "if" + Expression + Statement + ("else" + Statement)*(0, 1)
  this->set(STR("root.Main.If"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.MultiCmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("sections"), List::create({}, {
        Map::create(false, {}, {
          {STR("min"), std::make_shared<TiInt>(1)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
          {STR("kwd"), Map::create(false, {}, { { STR("if"), 0 }, { STR("إذا"), 0 } })},
          {STR("args"), List::create({}, {
            Map::create(false, {}, {
              {STR("min"), std::make_shared<TiInt>(1)},
              {STR("max"), std::make_shared<TiInt>(1)},
              {STR("pty"), std::make_shared<TiInt>(1)},
              {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
              {STR("prd"), PARSE_REF(STR("root.Expression"))}
            }),
            Map::create(false, {}, {
              {STR("min"), std::make_shared<TiInt>(1)},
              {STR("max"), std::make_shared<TiInt>(1)},
              {STR("pty"), std::make_shared<TiInt>(1)},
              {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
              {STR("prd"), PARSE_REF(STR("root.BlockMain.Statement"))}
            })
          })}
        }),
        Map::create(false, {}, {
          {STR("min"), std::make_shared<TiInt>(0)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {STR("kwd"), Map::create(false, {}, { { STR("else"), 0 }, { STR("وإلا"), 0 } })},
          {STR("args"), List::create({}, {
            Map::create(false, {}, {
              {STR("min"), std::make_shared<TiInt>(1)},
              {STR("max"), std::make_shared<TiInt>(1)},
              {STR("pty"), std::make_shared<TiInt>(1)},
              {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
              {STR("prd"), PARSE_REF(STR("root.BlockMain.Statement"))}
            })
          })}
        })
      })}
    })},
    {STR("handler"), std::make_shared<Handlers::IfParsingHandler>()}
  }).get());
  leadingCmdList->add(PARSE_REF(STR("module.If")));

  //// while = "while" + Expression + Statement
  this->set(STR("root.Main.While"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, { { STR("while"), 0 }, { STR("بينما"), 0 } })},
      {STR("prms"), List::create({}, {
        Map::create(false, {}, {
          {STR("prd"), PARSE_REF(STR("root.Expression"))},
          {STR("min"), std::make_shared<TiInt>(1)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        }),
        Map::create(false, {}, {
          {STR("prd"), PARSE_REF(STR("root.BlockMain.Statement"))},
          {STR("min"), std::make_shared<TiInt>(1)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {STR("handler"), std::make_shared<Handlers::WhileParsingHandler>()}
  }).get());
  leadingCmdList->add(PARSE_REF(STR("module.While")));

  //// for = "for" + Exp + Statement
  this->set(STR("root.Main.For"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, { { STR("for"), 0 }, { STR("لكل"), 0 } })},
      {STR("prms"), List::create({}, {
        Map::create(false, {}, {
          {STR("prd"), PARSE_REF(STR("root.Expression"))},
          {STR("min"), std::make_shared<TiInt>(1)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        }),
        Map::create(false, {}, {
          {STR("prd"), PARSE_REF(STR("root.BlockMain.Statement"))},
          {STR("min"), std::make_shared<TiInt>(1)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {STR("handler"), std::make_shared<Handlers::ForParsingHandler>()}
  }).get());
  leadingCmdList->add(PARSE_REF(STR("module.For")));

  //// continue = "continue" + Subject.Literal
  this->set(STR("root.Main.Continue"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, { { STR("continue"), 0 }, { STR("أكمل"), 0 } })},
      {STR("prms"), List::create({}, {
        Map::create(false, {}, {
          {STR("prd"), PARSE_REF(STR("root.Subject.Parameter"))},
          {STR("min"), std::make_shared<TiInt>(0)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {STR("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto metadata = state->getData().ti_cast_get<Data::Ast::Metadata>();
      auto currentList = state->getData().ti_cast_get<Basic::Containing<TiObject>>();
      auto continueStatement = Ast::ContinueStatement::create({
        { STR("prodId"), metadata->getProdId() },
        { STR("sourceLocation"), metadata->findSourceLocation() }
      });
      if (currentList != 0) {
        auto intLiteral = ti_cast<Core::Data::Ast::IntegerLiteral>(currentList->getElement(1));
        if (currentList->getElement(1) != 0 && intLiteral == 0) {
          state->addNotice(
            std::make_shared<Spp::Notices::InvalidContinueStatementNotice>(metadata->findSourceLocation())
          );
          state->setData(SharedPtr<TiObject>(0));
          return;
        }
        continueStatement->setSteps(getSharedPtr(intLiteral));
      }
      state->setData(continueStatement);
    })}
  }).get());
  leadingCmdList->add(PARSE_REF(STR("module.Continue")));

  //// break = "break" + Subject.Literal
  this->set(STR("root.Main.Break"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, { { STR("break"), 0 }, { STR("اقطع"), 0 } })},
      {STR("prms"), List::create({}, {
        Map::create(false, {}, {
          {STR("prd"), PARSE_REF(STR("root.Subject.Parameter"))},
          {STR("min"), std::make_shared<TiInt>(0)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {STR("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto metadata = state->getData().ti_cast_get<Data::Ast::Metadata>();
      auto currentList = state->getData().ti_cast_get<Basic::Containing<TiObject>>();
      auto breakStatement = Ast::BreakStatement::create({
        { "prodId", metadata->getProdId() },
        { "sourceLocation", metadata->findSourceLocation() }
      });
      if (currentList != 0) {
        auto intLiteral = ti_cast<Core::Data::Ast::IntegerLiteral>(currentList->getElement(1));
        if (currentList->getElement(1) != 0 && intLiteral == 0) {
          state->addNotice(std::make_shared<Spp::Notices::InvalidBreakStatementNotice>(metadata->findSourceLocation()));
          state->setData(SharedPtr<TiObject>(0));
          return;
        }
        breakStatement->setSteps(getSharedPtr(intLiteral));
      }
      state->setData(breakStatement);
    })}
  }).get());
  leadingCmdList->add(PARSE_REF(STR("module.Break")));

  //// return = "return" + Expression
  this->set(STR("root.Main.Return"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, { { STR("return"), 0 }, { STR("أرجع"), 0 } })},
      {STR("prms"), List::create({}, {
        Map::create(false, {}, {
          {STR("prd"), PARSE_REF(STR("root.Expression"))},
          {STR("min"), std::make_shared<TiInt>(0)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {STR("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto metadata = state->getData().ti_cast_get<Data::Ast::Metadata>();
      auto currentList = state->getData().ti_cast_get<Basic::Containing<TiObject>>();
      auto returnStatement = Ast::ReturnStatement::create({
        { "prodId", metadata->getProdId() },
        { "sourceLocation", metadata->findSourceLocation() }
      });
      if (currentList != 0) {
        returnStatement->setOperand(getSharedPtr(currentList->getElement(1)));
      }
      state->setData(returnStatement);
    })}
  }).get());
  leadingCmdList->add(PARSE_REF(STR("module.Return")));

  // Create inner commands.

  //// module = "module" + Set
  this->set(STR("root.Subject.Module"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, { { STR("module"), 0 }, { STR("حزمة"), 0 } })},
      {STR("prms"), List::create({}, {
        Map::create(false, {}, {
          {STR("prd"), PARSE_REF(STR("root.ModuleBody"))},
          {STR("min"), std::make_shared<TiInt>(1)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {STR("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto currentList = state->getData().tii_cast_get<Basic::Containing<TiObject>>();
      state->setData(getSharedPtr(currentList->getElement(1)));
    })}
  }).get());
  innerCmdList->add(PARSE_REF(STR("module.Module")));
  this->set(STR("root.ModuleBody"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("root.Set"))},
  }, {
    {STR("vars"), Map::create(false, {}, {
      {STR("stmt"), PARSE_REF(STR("root.Main.ModuleStatementList"))}
    })}
  }).get());
  this->set(STR("root.Main.ModuleStatementList"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("module.StatementList"))},
  }, {
    {STR("handler"), ScopeParsingHandler<Spp::Ast::Module>::create(-1)}
  }).get());

  //// type = "type" + Set
  this->set(STR("root.Subject.Type"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, { { STR("type"), 0 }, { STR("صنف"), 0 } })},
      {STR("prms"), List::create({}, {
        Map::create(false, {}, {
          {STR("prd"), PARSE_REF(STR("root.Block"))},
          {STR("min"), std::make_shared<TiInt>(1)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {STR("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto currentList = state->getData().ti_cast_get<Basic::Containing<TiObject>>();
      auto metadata = ti_cast<Data::Ast::Metadata>(currentList);
      auto type = Ast::UserType::create({
        { "prodId", metadata->getProdId()},
        { "sourceLocation", metadata->findSourceLocation() }
      }, {
        { "body", currentList->getElement(1) }
      });
      state->setData(type);
    })}
  }).get());
  innerCmdList->add(PARSE_REF(STR("module.Type")));

  // Function
  this->set(STR("root.Subject.Function"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, { { STR("function"), 0 }, { STR("دالّة"), 0 } })},
      {STR("prms"), List::create({}, {
        Map::create(false, {}, {
          {STR("prd"), PARSE_REF(STR("root.FuncSigExpression"))},
          {STR("min"), std::make_shared<TiInt>(1)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        }),
        Map::create(false, {}, {
          {STR("prd"), PARSE_REF(STR("root.Block"))},
          {STR("min"), std::make_shared<TiInt>(0)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {STR("handler"), std::make_shared<Handlers::FunctionParsingHandler>() }
  }).get());
  innerCmdList->add(PARSE_REF(STR("module.Function")));

  // FuncSigExpression
  this->set(STR("root.FuncSigExpression"), GrammarModule::create({
    {STR("startRef"), PARSE_REF(STR("module.LowLinkExp"))},
    {STR("baseRef"), PARSE_REF(STR("root.Expression")) }
  }).get());
  this->set(STR("root.FuncSigExpression.LowLinkExp"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("bmodule.LowLinkExp"))},
  }, {
    {STR("vars"), Map::create(false, {}, {{STR("enable"), std::make_shared<TiInt>(1)}})},
  }).get());
  this->set(STR("root.FuncSigExpression.AddExp"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("bmodule.AddExp"))},
  }, {
    {STR("vars"), Map::create(false, {}, {{STR("enable"), std::make_shared<TiInt>(0)}})},
  }).get());
  this->set(STR("root.FuncSigExpression.MulExp"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("bmodule.MulExp"))},
  }, {
    {STR("vars"), Map::create(false, {}, {{STR("enable"), std::make_shared<TiInt>(0)}})},
  }).get());
  this->set(STR("root.FuncSigExpression.BitwiseExp"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("bmodule.BitwiseExp"))},
  }, {
    {STR("vars"), Map::create(false, {}, {{STR("enable"), std::make_shared<TiInt>(0)}})},
  }).get());
  this->set(STR("root.FuncSigExpression.UnaryExp"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("bmodule.UnaryExp"))},
  }, {
    {STR("vars"), Map::create(false, {}, {
      {STR("enable1"), std::make_shared<TiInt>(0)},
      {STR("enable2"), std::make_shared<TiInt>(0)}
    })},
  }).get());
  this->set(STR("root.FuncSigExpression.FunctionalExp"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("bmodule.FunctionalExp"))},
  }, {
    {STR("vars"), Map::create(false, {}, {
      {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP | TermFlags::ONE_ROUTE_TERM)},
      {STR("operand"), PARSE_REF(STR("root.Subject"))},
      {STR("pty2"), std::make_shared<TiInt>(1)},
      {STR("dup"), 0},
      {STR("fltr2"), 0}
    })}
  }).get());

  // Block
  this->set(STR("root.Block"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("root.Set"))},
  }, {
    {STR("vars"), Map::create(false, {}, {
      {STR("stmt"), PARSE_REF(STR("root.Main.BlockStatementList"))}
    })}
  }).get());
  this->set(STR("root.Main.BlockStatementList"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("module.StatementList"))},
  }, {
    {STR("handler"), ScopeParsingHandler<Spp::Ast::Block>::create(-1) }
  }).get());

  // Block based statement.
  this->set(STR("root.BlockSubject"), GrammarModule::create({
    {STR("baseRef"), PARSE_REF(STR("root.Subject")) }
  }).get());
  this->set(STR("root.BlockSubject.Subject1"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("bmodule.Subject1"))},
  }, {
    {STR("vars"), Map::create(false, {}, {
      {STR("sbj1"), List::create({}, {
        PARSE_REF(STR("module.SubjectCmdGrp")),
        PARSE_REF(STR("module.Parameter")),
        PARSE_REF(STR("root.Block"))
      })},
      {STR("sbj2"), List::create({}, {PARSE_REF(STR("root.Expression"))})},
      {STR("sbj3"), List::create({}, {PARSE_REF(STR("root.Expression"))})},
      {STR("frc2"), 0},
      {STR("frc3"), 0}
    })}
  }).get());
  this->set(STR("root.BlockExpression"), GrammarModule::create({
    {STR("baseRef"), PARSE_REF(STR("root.Expression")) }
  }).get());
  this->set(STR("root.BlockExpression.FunctionalExp"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("bmodule.FunctionalExp"))},
  }, {
    {STR("vars"), Map::create(false, {}, {
      {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
      {STR("operand"), PARSE_REF(STR("root.BlockSubject"))},
      {STR("pty2"), std::make_shared<TiInt>(1)},
      {STR("dup"), 0},
      {STR("fltr2"), 0}
    })}
  }).get());
  this->set(STR("root.BlockMain"), GrammarModule::create({
    {STR("baseRef"), PARSE_REF(STR("root.Main")) }
  }).get());
  this->set(STR("root.BlockMain.ExpPhrase"), SymbolDefinition::create({
   {STR("baseRef"), PARSE_REF(STR("bmodule.ExpPhrase"))},
  }, {
    {STR("vars"), Map::create(false, {}, {
      {STR("subjects"), List::create({}, {
         Map::create(false, {}, {
           {STR("prd"), PARSE_REF(STR("root.BlockExpression"))},
           {STR("min"), std::make_shared<TiInt>(1)},
           {STR("max"), std::make_shared<TiInt>(1)},
           {STR("pty"), std::make_shared<TiInt>(1)}
         })
       })}
    })}
  }).get());

  // Create tilde commands.

  // ~ptr
  this->set(STR("root.Expression.PointerTilde"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, { { STR("ptr"), 0 }, { STR("مؤشر"), 0 } })},
      {STR("prms"), List::create() }
    })},
    {STR("handler"), Spp::Handlers::TildeOpParsingHandler<Spp::Ast::PointerOp>::create() }
  }).get());
  tildeCmdList->add(PARSE_REF(STR("module.PointerTilde")));
  // ~cnt
  this->set(STR("root.Expression.ContentTilde"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, { { STR("cnt"), 0 }, { STR("محتوى"), 0 } })},
      {STR("prms"), List::create() }
    })},
    {STR("handler"), Spp::Handlers::TildeOpParsingHandler<Spp::Ast::ContentOp>::create() }
  }).get());
  tildeCmdList->add(PARSE_REF(STR("module.ContentTilde")));
  // ~cast
  this->set(STR("root.Expression.CastTilde"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, {{STR("cast"), 0}, {STR("مثّل"), 0}, {STR("مثل"), 0}})},
      {STR("prms"), List::create({}, {
        Map::create(false, {}, {
          {STR("prd"), PARSE_REF(STR("root.CastSubject"))},
          {STR("min"), std::make_shared<TiInt>(1)},
          {STR("max"), std::make_shared<TiInt>(1)},
          {STR("pty"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {STR("handler"), Spp::Handlers::TildeOpParsingHandler<Spp::Ast::CastOp>::create() }
  }).get());
  tildeCmdList->add(PARSE_REF(STR("module.CastTilde")));
  this->set(STR("root.CastSubject"), GrammarModule::create({
    {STR("baseRef"), PARSE_REF(STR("root.Subject"))},
    {STR("startRef"), PARSE_REF(STR("module.Subject2")) }
  }).get());
  this->set(STR("root.CastSubject.Subject2"), SymbolDefinition::create({
   {STR("baseRef"), PARSE_REF(STR("bmodule.Subject2"))},
  }, {
    {STR("vars"), Map::create(false, {}, {
      {STR("sbj"), PARSE_REF(STR("root.Expression"))},
      {STR("fltr"), std::make_shared<TiInt>(2)},
      {STR("frc"), std::make_shared<TiInt>(1)}
    })}
  }).get());
  // ~size
  this->set(STR("root.Expression.SizeTilde"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, { { STR("size"), 0 }, { STR("حجم"), 0 } })},
      {STR("prms"), List::create() }
    })},
    {STR("handler"), Spp::Handlers::TildeOpParsingHandler<Spp::Ast::SizeOp>::create() }
  }).get());
  tildeCmdList->add(PARSE_REF(STR("module.SizeTilde")));
}


void GrammarFactory::cleanGrammar(Core::Data::Ast::Scope *rootScope)
{
  this->setRootScope(rootScope);

  auto leadingCmdList = this->getLeadingCommandsList();
  auto innerCmdList = this->getInnerCommandsList();
  auto tildeCmdList = this->getTildeCommandsList();

  // Remove commands from tilde commands list.
  this->removeReferenceFromCommandList(tildeCmdList, STR("module.SizeTilde"));
  this->removeReferenceFromCommandList(tildeCmdList, STR("module.CastTilde"));
  this->removeReferenceFromCommandList(tildeCmdList, STR("module.ContentTilde"));
  this->removeReferenceFromCommandList(tildeCmdList, STR("module.PointerTilde"));

  // Remove commands from leading commands list.
  this->removeReferenceFromCommandList(leadingCmdList, STR("module.DumpLlvmIr"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module.Run"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module.If"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module.While"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module.For"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module.Continue"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module.Break"));
  this->removeReferenceFromCommandList(leadingCmdList, STR("module.Return"));

  // Remove command from inner commands list.
  this->removeReferenceFromCommandList(innerCmdList, STR("module.Module"));
  this->removeReferenceFromCommandList(innerCmdList, STR("module.Type"));
  this->removeReferenceFromCommandList(innerCmdList, STR("module.Function"));

  // Delete definitions.
  this->tryRemove(STR("root.Main.DumpLlvmIr"));
  this->tryRemove(STR("root.Main.Run"));
  this->tryRemove(STR("root.Main.If"));
  this->tryRemove(STR("root.Main.While"));
  this->tryRemove(STR("root.Main.For"));
  this->tryRemove(STR("root.Main.Continue"));
  this->tryRemove(STR("root.Main.Break"));
  this->tryRemove(STR("root.Main.Return"));
  this->tryRemove(STR("root.Main.ModuleStatementList"));
  this->tryRemove(STR("root.Main.BlockStatementList"));
  this->tryRemove(STR("root.Subject.Module"));
  this->tryRemove(STR("root.ModuleBody"));
  this->tryRemove(STR("root.Subject.Type"));
  this->tryRemove(STR("root.Subject.Function"));
  this->tryRemove(STR("root.FuncSigExpression"));
  this->tryRemove(STR("root.Block"));
  this->tryRemove(STR("root.BlockSubject"));
  this->tryRemove(STR("root.BlockExpression"));
  this->tryRemove(STR("root.BlockMain"));
  this->tryRemove(STR("root.Expression.PointerTilde"));
  this->tryRemove(STR("root.Expression.ContentTilde"));
  this->tryRemove(STR("root.Expression.SizeTilde"));
  this->tryRemove(STR("root.Expression.CastTilde"));
  this->tryRemove(STR("root.CastSubject"));
}


List* GrammarFactory::getLeadingCommandsList()
{
  TiObject *obj;
  Core::Data::Grammar::GrammarModule *module;
  if (!this->tryGet(STR("root.Main.LeadingCmdGrp"), obj, &module)) {
    throw EXCEPTION(GenericException, STR("Could not find leading command group."));
  }
  SymbolDefinition *def = tio_cast<SymbolDefinition>(obj);
  if (def == 0) {
    throw EXCEPTION(GenericException, STR("Could not find leading command group."));
  }

  Map *vars = this->context.getSymbolVars(def, module);
  List *cmd_list = ti_cast<List>(vars->getElement(STR("cmds")));
  if (cmd_list == 0) {
    throw EXCEPTION(GenericException, STR("Could not find leading command group's command list."));
  }

  return cmd_list;
}


List* GrammarFactory::getInnerCommandsList()
{
  TiObject *obj;
  Core::Data::Grammar::GrammarModule *module;
  if (!this->tryGet(STR("root.Subject.SubjectCmdGrp"), obj, &module)) {
    throw EXCEPTION(GenericException, STR("Could not find inner command group."));
  }
  SymbolDefinition *def = tio_cast<SymbolDefinition>(obj);
  if (def == 0) {
    throw EXCEPTION(GenericException, STR("Could not find inner command group."));
  }

  Map *vars = this->context.getSymbolVars(def, module);
  List *cmd_list = ti_cast<List>(vars->getElement(STR("cmds")));
  if (cmd_list == 0) {
    throw EXCEPTION(GenericException, STR("Could not find inner command group's command list."));
  }

  return cmd_list;
}


List* GrammarFactory::getTildeCommandsList()
{
  TiObject *obj;
  Core::Data::Grammar::GrammarModule *module;
  if (!this->tryGet(STR("root.Expression.DefaultPostfixTildeCmd"), obj, &module)) {
    throw EXCEPTION(GenericException, STR("Could not find tilde command group."));
  }
  SymbolDefinition *def = tio_cast<SymbolDefinition>(obj);
  if (def == 0) {
    throw EXCEPTION(GenericException, STR("Could not find tilde command group."));
  }

  Map *vars = this->context.getSymbolVars(def, module);
  List *cmd_list = ti_cast<List>(vars->getElement(STR("cmds")));
  if (cmd_list == 0) {
    throw EXCEPTION(GenericException, STR("Could not find inner command group's command list."));
  }

  return cmd_list;
}


void GrammarFactory::removeReferenceFromCommandList(List *cmdList, Char const *qualifier)
{
  auto ref = Core::Data::Grammar::createReference(qualifier, &this->referenceCache);
  for (Int i = 0; i < cmdList->getCount(); ++i) {
    auto reference = ti_cast<Core::Data::Grammar::Reference>(cmdList->getElement(i));
    if (ref->isEqual(reference)) {
      cmdList->remove(i);
      return;
    }
  }
}

} // namespace
