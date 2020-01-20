/**
 * @file Spp/GrammarFactory.cpp
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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
  CodeGen::AstProcessor *astProcessor, CodeGen::Generator *generator,
  LlvmCodeGen::TargetGenerator *targetGenerator
) {
  this->setRootScope(rootScope);

  Core::Data::clearCaches(this->context.getRoot());

  // Add additional keywords.
  this->get<Core::Processing::Handlers::IdentifierTokenizingHandler>(
    S("root.LexerDefs.Identifier.handler")
  )->addKeywords({
    S("if"), S("إذا"), S("else"), S("وإلا"),
    S("while"), S("بينما"),
    S("for"), S("لكل"),
    S("continue"), S("أكمل"),
    S("break"), S("اقطع"),
    S("return"), S("أرجع"), S("ارجع"),
    S("module"), S("وحدة"),
    S("type"), S("صنف"),
    S("func"), S("function"), S("دالّة"), S("دالة"),
    S("macro"), S("ماكرو"),
    S("ptr"), S("مؤشر"),
    S("cnt"), S("محتوى"),
    S("cast"), S("مثّل"), S("مثل"),
    S("size"), S("حجم"),
    S("ast"), S("شبم"),
    S("this_type"), S("هذا_الصنف"),
    S("handler"), S("عملية"),
    S("this"), S("هذا"),
    S("value"), S("قيمة"),
    S("init"), S("هيئ"),
    S("terminate"), S("اتلف"), S("أتلف"),
    S("integer"), S("صحيح"),
    S("string"), S("محارف"),
    S("any"), S("أيما")
  });

  // Add translation for shared modifier.
  this->set(S("root.Main.Def.modifierTranslations.مشترك"), TiStr::create(S("shared")));

  // Create leading commands.

  //// if = "if" + Expression + Statement + ("else" + Statement)*(0, 1)
  this->createCommand(S("root.Main.If"), {
    {
      Map::create({}, { { S("if"), 0 }, { S("إذا"), 0 } }),
      {
        {
          PARSE_REF(S("module.Expression")),
          TiInt::create(1),
          TiInt::create(1),
          TiInt::create(ParsingFlags::PASS_ITEMS_UP)
        },
        {
          PARSE_REF(S("module.BlockStatements.OuterStmt")),
          TiInt::create(1),
          TiInt::create(1),
          TiInt::create(ParsingFlags::PASS_ITEMS_UP)
        }
      },
      TiInt::create(1),
      TiInt::create(1),
      TiInt::create(ParsingFlags::PASS_ITEMS_UP)
    }, {
      Map::create({}, { { S("else"), 0 }, { S("وإلا"), 0 } }),
      {
        {
          PARSE_REF(S("module.BlockStatements.OuterStmt")),
          TiInt::create(1),
          TiInt::create(1),
          TiInt::create(ParsingFlags::PASS_ITEMS_UP)
        }
      },
      TiInt::create(0),
      TiInt::create(1),
      TiInt::create(ParsingFlags::PASS_ITEMS_UP)
    }
  }, std::make_shared<Handlers::IfParsingHandler>());

  //// while = "while" + Expression + Statement
  this->createCommand(S("root.Main.While"), {{
    Map::create({}, { { S("while"), 0 }, { S("بينما"), 0 } }),
    {
      {
        PARSE_REF(S("module.Expression")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.BlockStatements.OuterStmt")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, std::make_shared<Handlers::WhileParsingHandler>());

  //// for = "for" + Exp + Statement
  this->createCommand(S("root.Main.For"), {{
    Map::create({}, { { S("for"), 0 }, { S("لكل"), 0 } }),
    {
      {
        PARSE_REF(S("module.Expression")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.BlockStatements.OuterStmt")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, std::make_shared<Handlers::ForParsingHandler>());

  //// continue = "continue" + Subject.Literal
  this->createCommand(S("root.Main.Continue"), {{
    Map::create({}, { { S("continue"), 0 }, { S("أكمل"), 0 } }),
    {{
      PARSE_REF(S("module.Subject.Parameter")),
      TiInt::create(0),
      TiInt::create(1),
      TiInt::create(ParsingFlags::PASS_ITEMS_UP)
    }}
  }}, std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
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
  }));

  //// break = "break" + Subject.Literal
  this->createCommand(S("root.Main.Break"), {{
    Map::create({}, { { S("break"), 0 }, { S("اقطع"), 0 } }),
    {{
      PARSE_REF(S("module.Subject.Parameter")),
      TiInt::create(0),
      TiInt::create(1),
      TiInt::create(ParsingFlags::PASS_ITEMS_UP)
    }}
  }}, std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
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
  }));

  //// return = "return" + Expression
  this->createCommand(S("root.Main.Return"), {{
    Map::create({}, { { S("return"), 0 }, { S("أرجع"), 0 }, { S("ارجع"), 0 } }),
    {{
      PARSE_REF(S("module.Expression")),
      TiInt::create(0),
      TiInt::create(1),
      TiInt::create(ParsingFlags::PASS_ITEMS_UP)
    }}
  }}, std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
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
  }));

  //// TypeOp = "handler" + Exp + Statement
  this->createCommand(S("root.Main.TypeOp"), {{
    Map::create({}, { { S("handler"), 0 }, { S("عملية"), 0 } }),
    {
      {
        PARSE_REF(S("module.Expression")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.BlockStatements.OuterStmt")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, std::make_shared<Handlers::TypeOpParsingHandler>());

  // Create inner commands.

  //// module = "module" + Set
  this->createCommand(S("root.Main.Module"), {{
    Map::create({}, { { S("module"), 0 }, { S("وحدة"), 0 } }),
    {
      {
        PARSE_REF(S("module.Subject.Identifier")),
        TiInt::create(0),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.ModuleBody")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, std::make_shared<Handlers::ModuleParsingHandler>());
  this->set(S("root.Main.Module.modifierTranslations"), Map::create({}, {
    {S("دمج"), TiStr::create(S("merge"))}
  }));
  this->set(S("root.Main.ModuleBody"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("root.Set"))},
  }, {
    {S("vars"), Map::create({}, {
      {S("stmt"), PARSE_REF(S("module.ModuleStatements.StmtList"))}
    })}
  }).get());
  this->set(S("root.Main.ModuleStatements"), Module::create({
    {S("baseRef"), PARSE_REF(S("module.owner.Statements"))}
  }));
  this->set(S("root.Main.ModuleStatements.StmtList"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.StmtList"))},
  }, {
    {S("handler"), ScopeParsingHandler<Spp::Ast::Module>::create()}
  }).get());

  //// type = "type" + Set
  this->createCommand(S("root.Main.Type"), {{
    Map::create({}, { { S("type"), 0 }, { S("صنف"), 0 } }),
    {
      {
        PARSE_REF(S("module.ParamOnlySubject")),
        TiInt::create(0),
        TiInt::create(2),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.BlockSet")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, std::make_shared<Handlers::TypeParsingHandler>());
  this->set(S("root.Main.Type.modifierTranslations"), Map::create({}, {
    {S("دمج"), TiStr::create(S("merge"))}
  }));

  // Function
  this->createCommand(S("root.Main.Function"), {{
    Map::create({}, { { S("func"), 0 }, { S("function"), 0 }, { S("دالّة"), 0 }, { S("دالة"), 0 } }),
    {
      {
        PARSE_REF(S("module.Subject.Identifier")),
        TiInt::create(0),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.FuncSigExpression")),
        TiInt::create(0),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.BlockSet")),
        TiInt::create(0),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, std::make_shared<Handlers::FunctionParsingHandler>());
  this->set(S("root.Main.Function.modifierTranslations"), Map::create({}, {
    {S("تصدير"), TiStr::create(S("expname"))},
    {S("مشترك"), TiStr::create(S("shared"))}
  }));

  // FuncSigExpression
  this->set(S("root.Main.FuncSigExpression"), Module::create({
    {S("baseRef"), PARSE_REF(S("module.owner.Expression")) },
    {S("startRef"), PARSE_REF(S("module.LowerLinkExp"))}
  }, {
    {S("subject"), PARSE_REF(S("module.owner.ParamOnlySubject"))}
  }).get());
  this->set(S("root.Main.FuncSigExpression.LowerLinkExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.LowerLinkExp"))},
  }, {
    {S("vars"), Map::create({}, {{S("enable"), std::make_shared<TiInt>(1)}})},
  }).get());
  this->set(S("root.Main.FuncSigExpression.LowLinkExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.LowLinkExp"))},
  }, {
    {S("vars"), Map::create({}, {{S("enable"), std::make_shared<TiInt>(1)}})},
  }).get());
  this->set(S("root.Main.FuncSigExpression.AddExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.AddExp"))},
  }, {
    {S("vars"), Map::create({}, {{S("enable"), std::make_shared<TiInt>(0)}})},
  }).get());
  this->set(S("root.Main.FuncSigExpression.MulExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.MulExp"))},
  }, {
    {S("vars"), Map::create({}, {{S("enable"), std::make_shared<TiInt>(0)}})},
  }).get());
  this->set(S("root.Main.FuncSigExpression.BitwiseExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.BitwiseExp"))},
  }, {
    {S("vars"), Map::create({}, {{S("enable"), std::make_shared<TiInt>(0)}})},
  }).get());
  this->set(S("root.Main.FuncSigExpression.UnaryExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.UnaryExp"))},
  }, {
    {S("vars"), Map::create({}, {
      {S("enable1"), std::make_shared<TiInt>(0)},
      {S("enable2"), std::make_shared<TiInt>(0)}
    })},
  }).get());

  this->set(S("root.Main.ParamOnlyExpression"), Module::create({
    {S("baseRef"), PARSE_REF(S("module.owner.Expression")) }
  }, {
    {S("subject"), PARSE_REF(S("module.owner.ParamOnlySubject"))}
  }).get());

  this->set(S("root.Main.ParamOnlySubject"), Module::create({
    {S("baseRef"), PARSE_REF(S("module.owner.Subject")) }
  }, {
    {S("expression"), PARSE_REF(S("module.owner.ParamOnlyExpression"))},
    {S("cmdGrp"), PARSE_REF(S("module.owner.Keywords"))}
  }).get());
  this->createProdGroup(S("root.Main.ParamOnlySubject.TermGroup"), {
    PARSE_REF(S("module.cmdGrp")),
    PARSE_REF(S("module.Parameter"))
  });

  // Macro
  this->createCommand(S("root.Main.Macro"), {{
    Map::create({}, { { S("macro"), 0 }, { S("ماكرو"), 0 } }),
    {
      {
        PARSE_REF(S("module.Subject.Identifier")),
        TiInt::create(0),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.MacroSignature")),
        TiInt::create(0),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.Expression")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, std::make_shared<Handlers::MacroParsingHandler>());
  // Macro Signature
  this->set(S("root.Main.MacroSignature"), Module::create({
    {S("baseRef"), PARSE_REF(S("module.owner.Subject")) }
  }).get());
  this->set(S("root.Main.MacroSignature.Sbj"), SymbolDefinition::create({
   {S("baseRef"), PARSE_REF(S("module.base.Sbj"))},
  }, {
    {S("vars"), Map::create({}, {
      {S("sbj1"), PARSE_REF(S("module.expression"))},
      {S("sbj2"), PARSE_REF(S("module.expression"))},
      {S("sbj3"), PARSE_REF(S("module.expression"))},
      {S("frc2"), std::make_shared<TiInt>(1)},
      {S("frc3"), std::make_shared<TiInt>(1)},
      {S("fltr"), std::make_shared<TiInt>(2)}
    })}
  }).get());

  // BlockSet
  this->set(S("root.Main.BlockSet"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("root.Set"))},
  }, {
    {S("vars"), Map::create({}, {
      {S("stmt"), PARSE_REF(S("module.BlockStatements.StmtList"))}
    })}
  }).get());
  // BlockStatements
  this->set(S("root.Main.BlockStatements"), Module::create({
    {S("baseRef"), PARSE_REF(S("module.owner.Statements"))}
  }, {
    {S("expression"), PARSE_REF(S("module.owner.BlockExpression"))}
  }));
  this->createProdGroup(S("root.Main.BlockStatements.OuterStmt"), {
    PARSE_REF(S("module.owner.BlockSet")),
    PARSE_REF(S("module.CmdVariation")),
    PARSE_REF(S("module.ExpVariation"))
  });
  // BlockSubject
  this->set(S("root.Main.BlockSubject"), Module::create({
    {S("baseRef"), PARSE_REF(S("module.owner.Subject")) }
  }, {
    {S("set"), PARSE_REF(S("module.owner.BlockSet"))}
  }).get());
  // BlockExpression
  this->set(S("root.Main.BlockExpression"), Module::create({
    {S("baseRef"), PARSE_REF(S("module.owner.Expression")) }
  }, {
    {S("subject"), PARSE_REF(S("module.owner.BlockSubject"))}
  }).get());

  // Keywords
  this->createCommand(S("root.Main.Keywords"), {{
    Map::create({}, {
      { S("this"), 0 },
      { S("هذا"), std::make_shared<TiStr>(("this")) },
      { S("value"), 0 },
      { S("قيمة"), std::make_shared<TiStr>(("value")) },
      { S("type"), 0 },
      { S("صنف"), std::make_shared<TiStr>(("type")) },
      { S("function"), 0 },
      { S("دالة"), std::make_shared<TiStr>(("function")) },
      { S("integer"), 0 },
      { S("صحيح"), std::make_shared<TiStr>(("integer")) },
      { S("string"), 0 },
      { S("محارف"), std::make_shared<TiStr>(("string")) },
      { S("any"), 0 },
      { S("أيما"), std::make_shared<TiStr>(("any")) }
    }),
    {
    }
  }}, std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
    auto current = state->getData().ti_cast_get<Core::Data::Ast::Token>();
    SharedPtr<Core::Data::Ast::Text> newObj = std::make_shared<Core::Data::Ast::Identifier>();
    newObj->setValue(current->getText());
    newObj->setProdId(current->getProdId());
    newObj->setSourceLocation(current->findSourceLocation());
    state->setData(newObj);
  }));

  // this_type
  this->createCommand(S("root.Main.ThisTypeRef"), {{
    Map::create({}, { { S("this_type"), 0 }, { S("هذا_الصنف"), 0 } }),
    {
    }
  }}, std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
    auto metadata = state->getData().ti_cast_get<Data::Ast::MetaHaving>();
    auto thisTypeRef = Ast::ThisTypeRef::create({
      { "prodId", metadata->getProdId() },
      { "sourceLocation", metadata->findSourceLocation() }
    });
    state->setData(thisTypeRef);
  }));

  // Create tilde commands.

  // ~ptr
  this->createCommand(S("root.Main.PointerTilde"), {{
    Map::create({}, { { S("ptr"), 0 }, { S("مؤشر"), 0 } }),
    {
    }
  }}, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::PointerOp>::create());
  // ~cnt
  this->createCommand(S("root.Main.ContentTilde"), {{
    Map::create({}, { { S("cnt"), 0 }, { S("محتوى"), 0 } }),
    {
    }
  }}, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::ContentOp>::create());
  // ~cast
  this->createCommand(S("root.Main.CastTilde"), {{
    Map::create({}, {{S("cast"), 0}, {S("مثّل"), 0}, {S("مثل"), 0}}),
    {
      {
        PARSE_REF(S("module.CastSubject")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::CastOp>::create());
  this->set(S("root.Main.CastSubject"), Module::create({
    {S("baseRef"), PARSE_REF(S("module.owner.Subject")) }
  }).get());
  this->set(S("root.Main.CastSubject.Sbj"), SymbolDefinition::create({
   {S("baseRef"), PARSE_REF(S("module.base.Sbj"))},
  }, {
    {S("vars"), Map::create({}, {
      {S("sbj1"), PARSE_REF(S("module.expression"))},
      {S("sbj2"), PARSE_REF(S("module.expression"))},
      {S("sbj3"), PARSE_REF(S("module.expression"))},
      {S("frc2"), std::make_shared<TiInt>(1)},
      {S("frc3"), std::make_shared<TiInt>(1)},
      {S("fltr"), std::make_shared<TiInt>(2)}
    })}
  }).get());
  // ~size
  this->createCommand(S("root.Main.SizeTilde"), {{
    Map::create({}, { { S("size"), 0 }, { S("حجم"), 0 } }),
    {
    }
  }}, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::SizeOp>::create());
  // ~ast
  this->createCommand(S("root.Main.AstRefTilde"), {{
    Map::create({}, { { S("ast"), 0 }, { S("شبم"), 0 } }),
    {
    }
  }}, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::AstRefOp>::create());
  // ~init
  this->createCommand(S("root.Main.InitTilde"), {{
    Map::create({}, {{S("init"), 0}, {S("هيئ"), 0}}),
    {
      {
        PARSE_REF(S("module.InitTildeSubject")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::InitOp>::create());
  this->set(S("root.Main.InitTildeSubject"), Module::create({
    {S("baseRef"), PARSE_REF(S("module.owner.Subject")) }
  }).get());
  this->set(S("root.Main.InitTildeSubject.Sbj"), SymbolDefinition::create({
   {S("baseRef"), PARSE_REF(S("module.base.Sbj"))},
  }, {
    {S("vars"), Map::create({}, {
      {S("sbj1"), PARSE_REF(S("module.expression"))},
      {S("sbj2"), PARSE_REF(S("module.expression"))},
      {S("sbj3"), PARSE_REF(S("module.expression"))},
      {S("frc2"), std::make_shared<TiInt>(0)},
      {S("frc3"), std::make_shared<TiInt>(0)},
      {S("fltr"), std::make_shared<TiInt>(1)}
    })}
  }).get());
  // ~terminate
  this->createCommand(S("root.Main.TerminateTilde"), {{
    Map::create({}, { { S("terminate"), 0 }, { S("اتلف"), 0 }, { S("أتلف"), 0 } }),
    {
      {
        PARSE_REF(S("module.TerminateTildeSubject")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::TerminateOp>::create());
  this->set(S("root.Main.TerminateTildeSubject"), Module::create({
    {S("baseRef"), PARSE_REF(S("module.owner.Subject")) }
  }).get());
  this->set(S("root.Main.TerminateTildeSubject.Sbj"), SymbolDefinition::create({
   {S("baseRef"), PARSE_REF(S("module.base.Sbj"))},
  }, {
    {S("vars"), Map::create({}, {
      {S("sbj1"), PARSE_REF(S("module.expression"))},
      {S("sbj2"), PARSE_REF(S("module.expression"))},
      {S("sbj3"), PARSE_REF(S("module.expression"))},
      {S("frc2"), std::make_shared<TiInt>(0)},
      {S("frc3"), std::make_shared<TiInt>(0)},
      {S("fltr"), std::make_shared<TiInt>(1)}
    })}
  }).get());

  // Add command references.

  this->addProdsToGroup(S("root.Main.LeadingCmdGrp"), {
    PARSE_REF(S("module.If")),
    PARSE_REF(S("module.While")),
    PARSE_REF(S("module.For")),
    PARSE_REF(S("module.Continue")),
    PARSE_REF(S("module.Break")),
    PARSE_REF(S("module.Return")),
    PARSE_REF(S("module.TypeOp"))
  });

  this->addProdsToGroup(S("root.Main.PostfixTildeCmdGrp"), {
    PARSE_REF(S("module.AstRefTilde")),
    PARSE_REF(S("module.SizeTilde")),
    PARSE_REF(S("module.CastTilde")),
    PARSE_REF(S("module.ContentTilde")),
    PARSE_REF(S("module.PointerTilde")),
    PARSE_REF(S("module.InitTilde")),
    PARSE_REF(S("module.TerminateTilde"))
  });

  this->addProdsToGroup(S("root.Main.SubjectCmdGrp"), {
    PARSE_REF(S("module.Module")),
    PARSE_REF(S("module.Type")),
    PARSE_REF(S("module.Function")),
    PARSE_REF(S("module.Macro")),
    PARSE_REF(S("module.Keywords")),
    PARSE_REF(S("module.ThisTypeRef"))
  });
}


void GrammarFactory::cleanGrammar(Core::Data::Ast::Scope *rootScope)
{
  this->setRootScope(rootScope);

  Core::Data::clearCaches(rootScope);

  // Add additional keywords.
  this->get<Core::Processing::Handlers::IdentifierTokenizingHandler>(
    S("root.LexerDefs.Identifier.handler")
  )->removeKeywords({
    S("if"), S("إذا"), S("else"), S("وإلا"),
    S("while"), S("بينما"),
    S("for"), S("لكل"),
    S("continue"), S("أكمل"),
    S("break"), S("اقطع"),
    S("return"), S("أرجع"), S("ارجع"),
    S("module"), S("وحدة"),
    S("type"), S("صنف"),
    S("func"), S("function"), S("دالّة"), S("دالة"),
    S("macro"), S("ماكرو"),
    S("ptr"), S("مؤشر"),
    S("cnt"), S("محتوى"),
    S("cast"), S("مثّل"), S("مثل"),
    S("size"), S("حجم"),
    S("ast"), S("شبم"),
    S("this_type"), S("هذا_الصنف"),
    S("handler"), S("عملية"),
    S("this"), S("هذا"),
    S("value"), S("قيمة"),
    S("init"), S("هيئ"),
    S("terminate"), S("اتلف"), S("أتلف"),
    S("integer"), S("صحيح"),
    S("string"), S("محارف"),
    S("any"), S("أيما")
  });

  // Add translation for static modifier.
  this->remove(S("root.Main.Def.modifierTranslations.مشترك"));

  // Remove commands from tilde commands list.
  this->removeProdsFromGroup(S("root.Main.PostfixTildeCmdGrp"), {
    S("module.AstRefTilde"),
    S("module.SizeTilde"),
    S("module.CastTilde"),
    S("module.ContentTilde"),
    S("module.PointerTilde"),
    S("module.InitTilde"),
    S("module.TerminateTilde")
  });

  // Remove commands from leading commands list.
  this->removeProdsFromGroup(S("root.Main.LeadingCmdGrp"), {
    S("module.If"),
    S("module.While"),
    S("module.For"),
    S("module.Continue"),
    S("module.Break"),
    S("module.Return"),
    S("module.TypeOp")
  });

  // Remove command from inner commands list.
  this->removeProdsFromGroup(S("root.Main.SubjectCmdGrp"), {
    S("module.Module"),
    S("module.Type"),
    S("module.Function"),
    S("module.Macro"),
    S("module.Keywords"),
    S("module.ThisTypeRef")
  });

  // Delete tilde command definitions.
  this->tryRemove(S("root.Main.PointerTilde"));
  this->tryRemove(S("root.Main.ContentTilde"));
  this->tryRemove(S("root.Main.CastTilde"));
  this->tryRemove(S("root.Main.CastSubject"));
  this->tryRemove(S("root.Main.SizeTilde"));
  this->tryRemove(S("root.Main.AstRefTilde"));
  this->tryRemove(S("root.Main.InitTilde"));
  this->tryRemove(S("root.Main.InitTildeSubject"));
  this->tryRemove(S("root.Main.TerminateTilde"));
  this->tryRemove(S("root.Main.TerminateTildeSubject"));

  // Delete leading command definitions.
  this->tryRemove(S("root.Main.If"));
  this->tryRemove(S("root.Main.While"));
  this->tryRemove(S("root.Main.For"));
  this->tryRemove(S("root.Main.Continue"));
  this->tryRemove(S("root.Main.Break"));
  this->tryRemove(S("root.Main.Return"));
  this->tryRemove(S("root.Main.TypeOp"));

  // Delete inner command definitions.
  this->tryRemove(S("root.Main.Module"));
  this->tryRemove(S("root.Main.ModuleBody"));
  this->tryRemove(S("root.Main.ModuleStatements"));
  this->tryRemove(S("root.Main.Type"));
  this->tryRemove(S("root.Main.Function"));
  this->tryRemove(S("root.Main.FuncSigExpression"));
  this->tryRemove(S("root.Main.ParamOnlyExpression"));
  this->tryRemove(S("root.Main.ParamOnlySubject"));
  this->tryRemove(S("root.Main.Macro"));
  this->tryRemove(S("root.Main.MacroSignature"));
  this->tryRemove(S("root.Main.Keywords"));
  this->tryRemove(S("root.Main.ThisTypeRef"));

  // Delete block definitions.
  this->tryRemove(S("root.Main.BlockSet"));
  this->tryRemove(S("root.Main.BlockStatements"));
  this->tryRemove(S("root.Main.BlockSubject"));
  this->tryRemove(S("root.Main.BlockExpression"));
}

} // namespace
