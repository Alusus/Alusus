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
  Core::Data::Ast::Scope *rootScope, Main::RootManager *manager, Ast::Helper *astHelper,
  CodeGen::Generator *generator, LlvmCodeGen::TargetGenerator *targetGenerator
) {
  this->setRootScope(rootScope);

  // Create leading commands.

  auto leadingCmdList = this->getLeadingCommandsList();
  auto innerCmdList = this->getInnerCommandsList();
  auto tildeCmdList = this->getTildeCommandsList();

  //// DumpIr = "dump_ir" + Subject
  this->set(S("root.Main.DumpLlvmIr"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.Cmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("kwd"), Map::create(false, {}, { { S("dump_llvm_ir"), 0 }, { S("أدرج_ت_و"), 0 } })},
      {S("prms"), List::create({}, {
        Map::create(false, {}, {
          {S("prd"), PARSE_REF(S("root.Expression"))},
          {S("min"), std::make_shared<TiInt>(1)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {S("handler"), std::make_shared<Handlers::CodeGenParsingHandler>(
      manager, astHelper, generator, targetGenerator, false
    )}
  }).get());
  leadingCmdList->add(PARSE_REF(S("module.DumpLlvmIr")));

  //// run = "run" + Subject
  this->set(S("root.Main.Run"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.Cmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("kwd"), Map::create(false, {}, { { S("run"), 0 }, { S("نفّذ"), 0 }, { S("نفذ"), 0 } })},
      {S("prms"), List::create({}, {
        Map::create(false, {}, {
          {S("prd"), PARSE_REF(S("root.Expression"))},
          {S("min"), std::make_shared<TiInt>(1)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {S("handler"), std::make_shared<Handlers::CodeGenParsingHandler>(
      manager, astHelper, generator, targetGenerator, true
    )}
  }).get());
  leadingCmdList->add(PARSE_REF(S("module.Run")));

  //// if = "if" + Expression + Statement + ("else" + Statement)*(0, 1)
  this->set(S("root.Main.If"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.MultiCmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("sections"), List::create({}, {
        Map::create(false, {}, {
          {S("min"), std::make_shared<TiInt>(1)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
          {S("kwd"), Map::create(false, {}, { { S("if"), 0 }, { S("إذا"), 0 } })},
          {S("args"), List::create({}, {
            Map::create(false, {}, {
              {S("min"), std::make_shared<TiInt>(1)},
              {S("max"), std::make_shared<TiInt>(1)},
              {S("pty"), std::make_shared<TiInt>(1)},
              {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
              {S("prd"), PARSE_REF(S("root.Expression"))}
            }),
            Map::create(false, {}, {
              {S("min"), std::make_shared<TiInt>(1)},
              {S("max"), std::make_shared<TiInt>(1)},
              {S("pty"), std::make_shared<TiInt>(1)},
              {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
              {S("prd"), PARSE_REF(S("root.BlockMain.Statement"))}
            })
          })}
        }),
        Map::create(false, {}, {
          {S("min"), std::make_shared<TiInt>(0)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {S("kwd"), Map::create(false, {}, { { S("else"), 0 }, { S("وإلا"), 0 } })},
          {S("args"), List::create({}, {
            Map::create(false, {}, {
              {S("min"), std::make_shared<TiInt>(1)},
              {S("max"), std::make_shared<TiInt>(1)},
              {S("pty"), std::make_shared<TiInt>(1)},
              {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
              {S("prd"), PARSE_REF(S("root.BlockMain.Statement"))}
            })
          })}
        })
      })}
    })},
    {S("handler"), std::make_shared<Handlers::IfParsingHandler>()}
  }).get());
  leadingCmdList->add(PARSE_REF(S("module.If")));

  //// while = "while" + Expression + Statement
  this->set(S("root.Main.While"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.Cmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("kwd"), Map::create(false, {}, { { S("while"), 0 }, { S("بينما"), 0 } })},
      {S("prms"), List::create({}, {
        Map::create(false, {}, {
          {S("prd"), PARSE_REF(S("root.Expression"))},
          {S("min"), std::make_shared<TiInt>(1)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        }),
        Map::create(false, {}, {
          {S("prd"), PARSE_REF(S("root.BlockMain.Statement"))},
          {S("min"), std::make_shared<TiInt>(1)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {S("handler"), std::make_shared<Handlers::WhileParsingHandler>()}
  }).get());
  leadingCmdList->add(PARSE_REF(S("module.While")));

  //// for = "for" + Exp + Statement
  this->set(S("root.Main.For"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.Cmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("kwd"), Map::create(false, {}, { { S("for"), 0 }, { S("لكل"), 0 } })},
      {S("prms"), List::create({}, {
        Map::create(false, {}, {
          {S("prd"), PARSE_REF(S("root.Expression"))},
          {S("min"), std::make_shared<TiInt>(1)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        }),
        Map::create(false, {}, {
          {S("prd"), PARSE_REF(S("root.BlockMain.Statement"))},
          {S("min"), std::make_shared<TiInt>(1)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {S("handler"), std::make_shared<Handlers::ForParsingHandler>()}
  }).get());
  leadingCmdList->add(PARSE_REF(S("module.For")));

  //// continue = "continue" + Subject.Literal
  this->set(S("root.Main.Continue"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.Cmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("kwd"), Map::create(false, {}, { { S("continue"), 0 }, { S("أكمل"), 0 } })},
      {S("prms"), List::create({}, {
        Map::create(false, {}, {
          {S("prd"), PARSE_REF(S("root.Subject.Parameter"))},
          {S("min"), std::make_shared<TiInt>(0)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {S("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto metadata = state->getData().ti_cast_get<Data::Ast::MetaHaving>();
      auto currentList = state->getData().ti_cast_get<Containing<TiObject>>();
      auto continueStatement = Ast::ContinueStatement::create({
        { S("prodId"), metadata->getProdId() },
        { S("sourceLocation"), metadata->findSourceLocation() }
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
  leadingCmdList->add(PARSE_REF(S("module.Continue")));

  //// break = "break" + Subject.Literal
  this->set(S("root.Main.Break"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.Cmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("kwd"), Map::create(false, {}, { { S("break"), 0 }, { S("اقطع"), 0 } })},
      {S("prms"), List::create({}, {
        Map::create(false, {}, {
          {S("prd"), PARSE_REF(S("root.Subject.Parameter"))},
          {S("min"), std::make_shared<TiInt>(0)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {S("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto metadata = state->getData().ti_cast_get<Data::Ast::MetaHaving>();
      auto currentList = state->getData().ti_cast_get<Containing<TiObject>>();
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
  leadingCmdList->add(PARSE_REF(S("module.Break")));

  //// return = "return" + Expression
  this->set(S("root.Main.Return"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.Cmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("kwd"), Map::create(false, {}, { { S("return"), 0 }, { S("أرجع"), 0 } })},
      {S("prms"), List::create({}, {
        Map::create(false, {}, {
          {S("prd"), PARSE_REF(S("root.Expression"))},
          {S("min"), std::make_shared<TiInt>(0)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {S("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto metadata = state->getData().ti_cast_get<Data::Ast::MetaHaving>();
      auto currentList = state->getData().ti_cast_get<Containing<TiObject>>();
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
  leadingCmdList->add(PARSE_REF(S("module.Return")));

  // Create inner commands.

  //// module = "module" + Set
  this->set(S("root.Subject.Module"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.Cmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("kwd"), Map::create(false, {}, { { S("module"), 0 }, { S("حزمة"), 0 } })},
      {S("prms"), List::create({}, {
        Map::create(false, {}, {
          {S("prd"), PARSE_REF(S("root.ModuleBody"))},
          {S("min"), std::make_shared<TiInt>(1)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {S("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto currentList = state->getData().ti_cast_get<Containing<TiObject>>();
      // We'll use the source location of the "module" keyword, rather than of the first statement.
      auto metaHaving = ti_cast<Core::Data::Ast::MetaHaving>(currentList->getElement(1));
      metaHaving->setSourceLocation(Core::Data::Ast::findSourceLocation(currentList->getElement(0)));
      state->setData(getSharedPtr(currentList->getElement(1)));
    })}
  }).get());
  innerCmdList->add(PARSE_REF(S("module.Module")));
  this->set(S("root.ModuleBody"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("root.Set"))},
  }, {
    {S("vars"), Map::create(false, {}, {
      {S("stmt"), PARSE_REF(S("root.Main.ModuleStatementList"))}
    })}
  }).get());
  this->set(S("root.Main.ModuleStatementList"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.StatementList"))},
  }, {
    {S("handler"), ScopeParsingHandler<Spp::Ast::Module>::create(-1)}
  }).get());

  //// type = "type" + Set
  this->set(S("root.Subject.Type"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.Cmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("kwd"), Map::create(false, {}, { { S("type"), 0 }, { S("صنف"), 0 } })},
      {S("prms"), List::create({}, {
        Map::create(false, {}, {
          {S("prd"), PARSE_REF(S("root.Block"))},
          {S("min"), std::make_shared<TiInt>(1)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {S("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto currentList = state->getData().ti_cast_get<Containing<TiObject>>();
      auto metadata = ti_cast<Data::Ast::MetaHaving>(currentList);
      auto type = Ast::UserType::create({
        { "prodId", metadata->getProdId()},
        { "sourceLocation", metadata->findSourceLocation() }
      }, {
        { "body", currentList->getElement(1) }
      });
      state->setData(type);
    })}
  }).get());
  innerCmdList->add(PARSE_REF(S("module.Type")));

  // Function
  this->set(S("root.Subject.Function"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.Cmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("kwd"), Map::create(false, {}, { { S("function"), 0 }, { S("دالّة"), 0 }, { S("دالة"), 0 } })},
      {S("prms"), List::create({}, {
        Map::create(false, {}, {
          {S("prd"), PARSE_REF(S("root.FuncSigExpression"))},
          {S("min"), std::make_shared<TiInt>(1)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        }),
        Map::create(false, {}, {
          {S("prd"), PARSE_REF(S("root.Block"))},
          {S("min"), std::make_shared<TiInt>(0)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {S("modifierTranslations"), Map::create({}, {
      {S("تصدير"), TiStr::create(S("expname"))}
    })},
    {S("handler"), std::make_shared<Handlers::FunctionParsingHandler>() }
  }).get());
  innerCmdList->add(PARSE_REF(S("module.Function")));

  // FuncSigExpression
  this->set(S("root.FuncSigExpression"), Module::create({
    {S("startRef"), PARSE_REF(S("module.LowLinkExp"))},
    {S("baseRef"), PARSE_REF(S("root.Expression")) }
  }).get());
  this->set(S("root.FuncSigExpression.LowLinkExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("bmodule.LowLinkExp"))},
  }, {
    {S("vars"), Map::create(false, {}, {{S("enable"), std::make_shared<TiInt>(1)}})},
  }).get());
  this->set(S("root.FuncSigExpression.AddExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("bmodule.AddExp"))},
  }, {
    {S("vars"), Map::create(false, {}, {{S("enable"), std::make_shared<TiInt>(0)}})},
  }).get());
  this->set(S("root.FuncSigExpression.MulExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("bmodule.MulExp"))},
  }, {
    {S("vars"), Map::create(false, {}, {{S("enable"), std::make_shared<TiInt>(0)}})},
  }).get());
  this->set(S("root.FuncSigExpression.BitwiseExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("bmodule.BitwiseExp"))},
  }, {
    {S("vars"), Map::create(false, {}, {{S("enable"), std::make_shared<TiInt>(0)}})},
  }).get());
  this->set(S("root.FuncSigExpression.UnaryExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("bmodule.UnaryExp"))},
  }, {
    {S("vars"), Map::create(false, {}, {
      {S("enable1"), std::make_shared<TiInt>(0)},
      {S("enable2"), std::make_shared<TiInt>(0)}
    })},
  }).get());
  this->set(S("root.FuncSigExpression.FunctionalExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("bmodule.FunctionalExp"))},
  }, {
    {S("vars"), Map::create(false, {}, {
      {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP | TermFlags::ONE_ROUTE_TERM)},
      {S("operand"), PARSE_REF(S("root.Subject"))},
      {S("pty2"), std::make_shared<TiInt>(1)},
      {S("dup"), 0},
      {S("fltr2"), 0}
    })}
  }).get());

  // Block
  this->set(S("root.Block"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("root.Set"))},
  }, {
    {S("vars"), Map::create(false, {}, {
      {S("stmt"), PARSE_REF(S("root.Main.BlockStatementList"))}
    })}
  }).get());
  this->set(S("root.Main.BlockStatementList"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.StatementList"))},
  }, {
    {S("handler"), ScopeParsingHandler<Spp::Ast::Block>::create(-1) }
  }).get());

  // Block based statement.
  this->set(S("root.BlockSubject"), Module::create({
    {S("baseRef"), PARSE_REF(S("root.Subject")) }
  }).get());
  this->set(S("root.BlockSubject.Subject1"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("bmodule.Subject1"))},
  }, {
    {S("vars"), Map::create(false, {}, {
      {S("sbj1"), List::create({}, {
        PARSE_REF(S("module.SubjectCmdGrp")),
        PARSE_REF(S("module.Parameter")),
        PARSE_REF(S("root.Block"))
      })},
      {S("sbj2"), List::create({}, {PARSE_REF(S("root.Expression"))})},
      {S("sbj3"), List::create({}, {PARSE_REF(S("root.Expression"))})},
      {S("frc2"), 0},
      {S("frc3"), 0}
    })}
  }).get());
  this->set(S("root.BlockExpression"), Module::create({
    {S("baseRef"), PARSE_REF(S("root.Expression")) }
  }).get());
  this->set(S("root.BlockExpression.FunctionalExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("bmodule.FunctionalExp"))},
  }, {
    {S("vars"), Map::create(false, {}, {
      {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
      {S("operand"), PARSE_REF(S("root.BlockSubject"))},
      {S("pty2"), std::make_shared<TiInt>(1)},
      {S("dup"), 0},
      {S("fltr2"), 0}
    })}
  }).get());
  this->set(S("root.BlockMain"), Module::create({
    {S("baseRef"), PARSE_REF(S("root.Main")) }
  }).get());
  this->set(S("root.BlockMain.ExpPhrase"), SymbolDefinition::create({
   {S("baseRef"), PARSE_REF(S("bmodule.ExpPhrase"))},
  }, {
    {S("vars"), Map::create(false, {}, {
      {S("subjects"), List::create({}, {
         Map::create(false, {}, {
           {S("prd"), PARSE_REF(S("root.BlockExpression"))},
           {S("min"), std::make_shared<TiInt>(1)},
           {S("max"), std::make_shared<TiInt>(1)},
           {S("pty"), std::make_shared<TiInt>(1)}
         })
       })}
    })}
  }).get());

  // Create tilde commands.

  // ~ptr
  this->set(S("root.Expression.PointerTilde"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.Cmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("kwd"), Map::create(false, {}, { { S("ptr"), 0 }, { S("مؤشر"), 0 } })},
      {S("prms"), List::create() }
    })},
    {S("handler"), Spp::Handlers::TildeOpParsingHandler<Spp::Ast::PointerOp>::create() }
  }).get());
  tildeCmdList->add(PARSE_REF(S("module.PointerTilde")));
  // ~cnt
  this->set(S("root.Expression.ContentTilde"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.Cmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("kwd"), Map::create(false, {}, { { S("cnt"), 0 }, { S("محتوى"), 0 } })},
      {S("prms"), List::create() }
    })},
    {S("handler"), Spp::Handlers::TildeOpParsingHandler<Spp::Ast::ContentOp>::create() }
  }).get());
  tildeCmdList->add(PARSE_REF(S("module.ContentTilde")));
  // ~cast
  this->set(S("root.Expression.CastTilde"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.Cmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("kwd"), Map::create(false, {}, {{S("cast"), 0}, {S("مثّل"), 0}, {S("مثل"), 0}})},
      {S("prms"), List::create({}, {
        Map::create(false, {}, {
          {S("prd"), PARSE_REF(S("root.CastSubject"))},
          {S("min"), std::make_shared<TiInt>(1)},
          {S("max"), std::make_shared<TiInt>(1)},
          {S("pty"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        })
      })}
    })},
    {S("handler"), Spp::Handlers::TildeOpParsingHandler<Spp::Ast::CastOp>::create() }
  }).get());
  tildeCmdList->add(PARSE_REF(S("module.CastTilde")));
  this->set(S("root.CastSubject"), Module::create({
    {S("baseRef"), PARSE_REF(S("root.Subject"))},
    {S("startRef"), PARSE_REF(S("module.Subject2")) }
  }).get());
  this->set(S("root.CastSubject.Subject2"), SymbolDefinition::create({
   {S("baseRef"), PARSE_REF(S("bmodule.Subject2"))},
  }, {
    {S("vars"), Map::create(false, {}, {
      {S("sbj"), PARSE_REF(S("root.Expression"))},
      {S("fltr"), std::make_shared<TiInt>(2)},
      {S("frc"), std::make_shared<TiInt>(1)}
    })}
  }).get());
  // ~size
  this->set(S("root.Expression.SizeTilde"), SymbolDefinition::create({}, {
    {S("term"), PARSE_REF(S("root.Cmd"))},
    {S("vars"), Map::create(false, {}, {
      {S("kwd"), Map::create(false, {}, { { S("size"), 0 }, { S("حجم"), 0 } })},
      {S("prms"), List::create() }
    })},
    {S("handler"), Spp::Handlers::TildeOpParsingHandler<Spp::Ast::SizeOp>::create() }
  }).get());
  tildeCmdList->add(PARSE_REF(S("module.SizeTilde")));
}


void GrammarFactory::cleanGrammar(Core::Data::Ast::Scope *rootScope)
{
  this->setRootScope(rootScope);

  auto leadingCmdList = this->getLeadingCommandsList();
  auto innerCmdList = this->getInnerCommandsList();
  auto tildeCmdList = this->getTildeCommandsList();

  // Remove commands from tilde commands list.
  this->removeReferenceFromCommandList(tildeCmdList, S("module.SizeTilde"));
  this->removeReferenceFromCommandList(tildeCmdList, S("module.CastTilde"));
  this->removeReferenceFromCommandList(tildeCmdList, S("module.ContentTilde"));
  this->removeReferenceFromCommandList(tildeCmdList, S("module.PointerTilde"));

  // Remove commands from leading commands list.
  this->removeReferenceFromCommandList(leadingCmdList, S("module.DumpLlvmIr"));
  this->removeReferenceFromCommandList(leadingCmdList, S("module.Run"));
  this->removeReferenceFromCommandList(leadingCmdList, S("module.If"));
  this->removeReferenceFromCommandList(leadingCmdList, S("module.While"));
  this->removeReferenceFromCommandList(leadingCmdList, S("module.For"));
  this->removeReferenceFromCommandList(leadingCmdList, S("module.Continue"));
  this->removeReferenceFromCommandList(leadingCmdList, S("module.Break"));
  this->removeReferenceFromCommandList(leadingCmdList, S("module.Return"));

  // Remove command from inner commands list.
  this->removeReferenceFromCommandList(innerCmdList, S("module.Module"));
  this->removeReferenceFromCommandList(innerCmdList, S("module.Type"));
  this->removeReferenceFromCommandList(innerCmdList, S("module.Function"));

  // Delete definitions.
  this->tryRemove(S("root.Main.DumpLlvmIr"));
  this->tryRemove(S("root.Main.Run"));
  this->tryRemove(S("root.Main.If"));
  this->tryRemove(S("root.Main.While"));
  this->tryRemove(S("root.Main.For"));
  this->tryRemove(S("root.Main.Continue"));
  this->tryRemove(S("root.Main.Break"));
  this->tryRemove(S("root.Main.Return"));
  this->tryRemove(S("root.Main.ModuleStatementList"));
  this->tryRemove(S("root.Main.BlockStatementList"));
  this->tryRemove(S("root.Subject.Module"));
  this->tryRemove(S("root.ModuleBody"));
  this->tryRemove(S("root.Subject.Type"));
  this->tryRemove(S("root.Subject.Function"));
  this->tryRemove(S("root.FuncSigExpression"));
  this->tryRemove(S("root.Block"));
  this->tryRemove(S("root.BlockSubject"));
  this->tryRemove(S("root.BlockExpression"));
  this->tryRemove(S("root.BlockMain"));
  this->tryRemove(S("root.Expression.PointerTilde"));
  this->tryRemove(S("root.Expression.ContentTilde"));
  this->tryRemove(S("root.Expression.SizeTilde"));
  this->tryRemove(S("root.Expression.CastTilde"));
  this->tryRemove(S("root.CastSubject"));
}


List* GrammarFactory::getLeadingCommandsList()
{
  TiObject *obj;
  Core::Data::Grammar::Module *module;
  if (!this->tryGet(S("root.Main.LeadingCmdGrp"), obj, &module)) {
    throw EXCEPTION(GenericException, S("Could not find leading command group."));
  }
  SymbolDefinition *def = ti_cast<SymbolDefinition>(obj);
  if (def == 0) {
    throw EXCEPTION(GenericException, S("Could not find leading command group."));
  }

  Map *vars = this->context.getSymbolVars(def, module);
  List *cmd_list = ti_cast<List>(vars->getElement(S("cmds")));
  if (cmd_list == 0) {
    throw EXCEPTION(GenericException, S("Could not find leading command group's command list."));
  }

  return cmd_list;
}


List* GrammarFactory::getInnerCommandsList()
{
  TiObject *obj;
  Core::Data::Grammar::Module *module;
  if (!this->tryGet(S("root.Subject.SubjectCmdGrp"), obj, &module)) {
    throw EXCEPTION(GenericException, S("Could not find inner command group."));
  }
  SymbolDefinition *def = ti_cast<SymbolDefinition>(obj);
  if (def == 0) {
    throw EXCEPTION(GenericException, S("Could not find inner command group."));
  }

  Map *vars = this->context.getSymbolVars(def, module);
  List *cmd_list = ti_cast<List>(vars->getElement(S("cmds")));
  if (cmd_list == 0) {
    throw EXCEPTION(GenericException, S("Could not find inner command group's command list."));
  }

  return cmd_list;
}


List* GrammarFactory::getTildeCommandsList()
{
  TiObject *obj;
  Core::Data::Grammar::Module *module;
  if (!this->tryGet(S("root.Expression.DefaultPostfixTildeCmd"), obj, &module)) {
    throw EXCEPTION(GenericException, S("Could not find tilde command group."));
  }
  SymbolDefinition *def = ti_cast<SymbolDefinition>(obj);
  if (def == 0) {
    throw EXCEPTION(GenericException, S("Could not find tilde command group."));
  }

  Map *vars = this->context.getSymbolVars(def, module);
  List *cmd_list = ti_cast<List>(vars->getElement(S("cmds")));
  if (cmd_list == 0) {
    throw EXCEPTION(GenericException, S("Could not find inner command group's command list."));
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
