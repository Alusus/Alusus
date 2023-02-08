/**
 * @file Spp/SppFactory.cpp
 *
 * @copyright Copyright (C) 2023 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Grammar
{

using namespace Core::Data::Grammar;
using namespace Core::Processing;
using namespace Core::Processing::Handlers;

using Map = Core::Data::Grammar::Map;

//==============================================================================
// Overloaded Abstract Functions

void SppFactory::createGrammar()
{
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
    S("class"), S("type"), S("صنف"),
    S("func"), S("function"), S("دالّة"), S("دالة"),
    S("macro"), S("ماكرو"),
    S("preprocess"), S("تمهيد"),
    S("ptr"), S("مؤشر"),
    S("cnt"), S("محتوى"),
    S("deref"), S("تتبع"),
    S("no_deref"), S("عطل_التتبع"),
    S("cast"), S("مثّل"), S("مثل"),
    S("size"), S("حجم"),
    S("ast"), S("شبم"),
    S("ast_ref"), S("سند_شبم"),
    S("this_type"), S("هذا_الصنف"),
    S("handler"), S("عملية"),
    S("this"), S("هذا"),
    S("value"), S("قيمة"),
    S("init"), S("هيئ"),
    S("terminate"), S("اتلف"), S("أتلف"),
    S("integer"), S("صحيح"),
    S("string"), S("محارف"),
    S("any"), S("أيما"),
    S("next_arg"), S("المعطى_التالي"),
    S("as_ptr"), S("كمؤشر"),
    S("set_ptr"), S("حدد_مؤشر"),
    S("no_injection"), S("بلا_حقن")
  });

  // Add translations for def modifiers.
  this->set(S("root.Main.Def.modifierTranslations.مشترك"), TiStr::create(S("shared")));
  this->set(S("root.Main.Def.modifierTranslations.دون_ربط"), TiStr::create(S("no_bind")));
  this->set(S("root.Main.Def.modifierTranslations.حقنة"), TiStr::create(S("injection")));
  this->set(S("root.Main.Def.modifierTranslations.عملية"), TiStr::create(S("operation")));

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
  }, newSrdObj<Handlers::IfParsingHandler>());

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
  }}, newSrdObj<Handlers::WhileParsingHandler>());

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
  }}, newSrdObj<Handlers::ForParsingHandler>());

  //// continue = "continue" + Subject.Literal
  this->createCommand(S("root.Main.Continue"), {{
    Map::create({}, { { S("continue"), 0 }, { S("أكمل"), 0 } }),
    {{
      PARSE_REF(S("module.Subject.Parameter")),
      TiInt::create(0),
      TiInt::create(1),
      TiInt::create(ParsingFlags::PASS_ITEMS_UP)
    }}
  }}, newSrdObj<CustomParsingHandler>([](Parser *parser, ParserState *state) {
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
          newSrdObj<Spp::Notices::InvalidContinueStatementNotice>(metadata->findSourceLocation())
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
  }}, newSrdObj<CustomParsingHandler>([](Parser *parser, ParserState *state) {
    auto metadata = state->getData().ti_cast_get<Data::Ast::MetaHaving>();
    auto currentList = state->getData().ti_cast_get<Containing<TiObject>>();
    auto breakStatement = Ast::BreakStatement::create({
      { "prodId", metadata->getProdId() },
      { "sourceLocation", metadata->findSourceLocation() }
    });
    if (currentList != 0) {
      auto intLiteral = ti_cast<Core::Data::Ast::IntegerLiteral>(currentList->getElement(1));
      if (currentList->getElement(1) != 0 && intLiteral == 0) {
        state->addNotice(newSrdObj<Spp::Notices::InvalidBreakStatementNotice>(metadata->findSourceLocation()));
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
  }}, newSrdObj<CustomParsingHandler>([](Parser *parser, ParserState *state) {
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

  //// TypeOp = "handler" + Exp + TypeOpOptions + Statement
  this->createCommand(S("root.Main.TypeOp"), {{
    Map::create({}, { { S("handler"), 0 }, { S("عملية"), 0 } }),
    {
      {
        PARSE_REF(S("module.SquareArgsSubject")),
        TiInt::create(0),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.Expression")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.TypeOpOptions")),
        TiInt::create(0),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.BlockStatements.OuterStmt")),
        TiInt::create(0),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, newSrdObj<Handlers::TypeHandlersParsingHandler>());
  this->createCommand(S("root.Main.TypeOpOptions"), {{
    Map::create({}, {
      { S("as_ptr"), 0 },
      { S("كمؤشر"), newSrdObj<TiStr>(("as_ptr")) },
      { S("set_ptr"), 0 },
      { S("حدد_مؤشر"), newSrdObj<TiStr>(("set_ptr")) }
    }), {}
  }}, newSrdObj<GenericParsingHandler>());

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
  }}, newSrdObj<Handlers::ModuleParsingHandler>());
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
    {S("handler"), ScopeParsingHandler<Spp::Ast::Module>::create(this->rootManager->getSeeker())}
  }).get());

  //// type = "class" + Set
  this->createCommand(S("root.Main.Type"), {{
    Map::create({}, { { S("class"), 0 }, { S("صنف"), 0 } }),
    {
      {
        PARSE_REF(S("module.ParamOnlySubject")),
        TiInt::create(0),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.SquareArgsSubject")),
        TiInt::create(0),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.BlockSet")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, newSrdObj<Handlers::TypeParsingHandler>());
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
        PARSE_REF(S("module.SquareArgsSubject")),
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
  }}, newSrdObj<Handlers::FunctionParsingHandler>());
  this->set(S("root.Main.Function.modifierTranslations"), Map::create({}, {
    {S("تصدير"), TiStr::create(S("expname"))},
    {S("عضو"), TiStr::create(S("member"))},
    {S("عملية"), TiStr::create(S("operation"))}
  }));

  // FuncSigExpression
  this->set(S("root.Main.FuncSigExpression"), Module::create({
    {S("baseRef"), PARSE_REF(S("module.owner.Expression")) },
    {S("startRef"), PARSE_REF(S("module.LowerLinkExp"))}
  }, {
  }).get());
  this->set(S("root.Main.FuncSigExpression.LowerLinkExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.LowerLinkExp"))},
  }, {
    {S("vars"), Map::create({}, {{S("enable"), newSrdObj<TiInt>(1)}})},
  }).get());
  this->set(S("root.Main.FuncSigExpression.AssignmentExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.AssignmentExp"))},
  }, {
    {S("vars"), Map::create({}, {{S("enable"), newSrdObj<TiInt>(0)}})},
  }).get());
  this->set(S("root.Main.FuncSigExpression.LogExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.LogExp"))},
  }, {
    {S("vars"), Map::create({}, {{S("enable"), newSrdObj<TiInt>(0)}})},
  }).get());
  this->set(S("root.Main.FuncSigExpression.ComparisonExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.ComparisonExp"))},
  }, {
    {S("vars"), Map::create({}, {{S("enable"), newSrdObj<TiInt>(0)}})},
  }).get());
  this->set(S("root.Main.FuncSigExpression.LowLinkExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.LowLinkExp"))},
  }, {
    {S("vars"), Map::create({}, {{S("enable"), newSrdObj<TiInt>(1)}})},
  }).get());
  this->set(S("root.Main.FuncSigExpression.AddExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.AddExp"))},
  }, {
    {S("vars"), Map::create({}, {{S("enable"), newSrdObj<TiInt>(0)}})},
  }).get());
  this->set(S("root.Main.FuncSigExpression.MulExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.MulExp"))},
  }, {
    {S("vars"), Map::create({}, {{S("enable"), newSrdObj<TiInt>(0)}})},
  }).get());
  this->set(S("root.Main.FuncSigExpression.BitwiseExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.BitwiseExp"))},
  }, {
    {S("vars"), Map::create({}, {{S("enable"), newSrdObj<TiInt>(0)}})},
  }).get());
  this->set(S("root.Main.FuncSigExpression.UnaryExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.UnaryExp"))},
  }, {
    {S("vars"), Map::create({}, {
      {S("enable1"), newSrdObj<TiInt>(0)},
      {S("enable2"), newSrdObj<TiInt>(0)}
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
        PARSE_REF(S("module.SquareArgsSubject")),
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
  }}, newSrdObj<Handlers::MacroParsingHandler>());
  this->set(S("root.Main.Macro.modifierTranslations"), Map::create({}, {
    {S("عضو"), TiStr::create(S("member"))}
  }));

  // Square Arguments Signature
  this->set(S("root.Main.SquareArgsSubject"), Module::create({
    {S("baseRef"), PARSE_REF(S("module.owner.ParamOnlySubject")) }
  }).get());
  this->set(S("root.Main.SquareArgsSubject.Sbj"), SymbolDefinition::create({
   {S("baseRef"), PARSE_REF(S("module.base.Sbj"))},
  }, {
    {S("vars"), Map::create({}, {
      {S("sbj1"), PARSE_REF(S("module.expression"))},
      {S("sbj2"), PARSE_REF(S("module.expression"))},
      {S("sbj3"), PARSE_REF(S("module.expression"))},
      {S("frc2"), newSrdObj<TiInt>(1)},
      {S("frc3"), newSrdObj<TiInt>(1)},
      {S("fltr"), newSrdObj<TiInt>(2)}
    })}
  }).get());

  //// Preprocess = "preprocess" + Statement
  this->createCommand(S("root.Main.Preprocess"), {{
    Map::create({}, { { S("preprocess"), 0 }, { S("تمهيد"), 0 } }),
    {
      {
        PARSE_REF(S("module.BlockStatements.OuterStmt")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, newSrdObj<CustomParsingHandler>([](Parser *parser, ParserState *state) {
    auto metadata = state->getData().ti_cast_get<Data::Ast::MetaHaving>();
    auto currentList = state->getData().ti_cast_get<Containing<TiObject>>();
    if (currentList->getElementCount() != 2) {
      throw EXCEPTION(GenericException, S("Unexpected error while parsing `preprocess` statement."));
    }
    auto preprocessStatement = Ast::PreprocessStatement::create({
      { S("prodId"), metadata->getProdId() },
      { S("sourceLocation"), metadata->findSourceLocation() }
    }, {
      { S("body"), currentList->getElement(1) }
    });
    state->setData(preprocessStatement);
  }));

  //// astLiteral = "ast" + Statement
  this->createCommand(S("root.Main.AstLiteral"), {
    {
      Map::create({}, { { S("ast"), 0 }, { S("شبم"), 0 } }),
      {
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
    }
  }, newSrdObj<CustomParsingHandler>(
    [](Core::Processing::Parser *parser, Core::Processing::ParserState *state) {
      auto metadata = state->getData().ti_cast_get<Data::Ast::MetaHaving>();
      auto currentList = state->getData().ti_cast_get<Containing<TiObject>>();
      if (currentList == 0 || currentList->getElementCount() != 2) {
        throw EXCEPTION(GenericException, S("Unexpected data type while parsing AST literal command."));
      }
      auto astLiteralCommand = Ast::AstLiteralCommand::create({
        { "prodId", metadata->getProdId() },
        { "sourceLocation", metadata->findSourceLocation() }
      }, {
        { "body", currentList->getElement(1) }
      });
      state->setData(astLiteralCommand);
    },
    true,
    [](Parser *parser, ParserState *state, TioSharedPtr const &modifierData, Bool prodProcessingComplete)->Bool {
      if (!prodProcessingComplete) return false;

      // Look for no_preprocess modifier.
      auto identifier = modifierData.ti_cast_get<Core::Data::Ast::Identifier>();
      if (identifier == 0) return false;
      auto symbolDef = state->refTopProdLevel().getProd();
      auto keyword = symbolDef->getTranslatedModifierKeyword(identifier->getValue().get());

      if (keyword != S("no_preprocess")) return false;

      // Update the AST literal command.
      Int levelOffset = -state->getTopProdTermLevelCount();
      auto astLiteral = state->getData(levelOffset).ti_cast_get<Spp::Ast::AstLiteralCommand>();
      if (astLiteral == 0) {
        throw EXCEPTION(GenericException, S("Unexpected data type found."));
      }
      astLiteral->setPreprocessDisabled(true);

      return true;
    }
  ));
  this->set(S("root.Main.AstLiteral.modifierTranslations"), Map::create({}, {
    {S("بلا_تمهيد"), TiStr::create(S("no_preprocess"))}
  }));

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
  }));
  this->set(S("root.Main.BlockStatements.StmtList"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.base.StmtList"))},
  }, {
    {S("handler"), ScopeParsingHandler<Spp::Ast::Block>::create(this->rootManager->getSeeker())}
  }).get());
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
  this->set(S("root.Main.Expression._subjectBackup"), getSharedPtr(this->get(S("root.Main.Expression.subject"))));
  this->set(S("root.Main.Expression.subject"), PARSE_REF(S("module.owner.BlockSubject")));

  // Keywords
  this->createCommand(S("root.Main.Keywords"), {{
    Map::create({}, {
      { S("this"), 0 },
      { S("هذا"), newSrdObj<TiStr>(("this")) },
      { S("value"), 0 },
      { S("قيمة"), newSrdObj<TiStr>(("value")) },
      { S("module"), 0 },
      { S("وحدة"), newSrdObj<TiStr>(("module")) },
      { S("type"), 0 },
      { S("صنف"), newSrdObj<TiStr>(("type")) },
      { S("function"), 0 },
      { S("دالة"), newSrdObj<TiStr>(("function")) },
      { S("integer"), 0 },
      { S("صحيح"), newSrdObj<TiStr>(("integer")) },
      { S("string"), 0 },
      { S("محارف"), newSrdObj<TiStr>(("string")) },
      { S("ast"), 0 },
      { S("شبم"), newSrdObj<TiStr>(("ast")) },
      { S("ast_ref"), 0 },
      { S("سند_شبم"), newSrdObj<TiStr>(("ast_ref")) },
      { S("any"), 0 },
      { S("أيما"), newSrdObj<TiStr>(("any")) }
    }),
    {
    }
  }}, newSrdObj<CustomParsingHandler>([](Parser *parser, ParserState *state) {
    auto current = state->getData().ti_cast_get<Core::Data::Ast::Token>();
    SharedPtr<Core::Data::Ast::Text> newObj = newSrdObj<Core::Data::Ast::Identifier>();
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
  }}, newSrdObj<CustomParsingHandler>([](Parser *parser, ParserState *state) {
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
  // ~deref
  this->createCommand(S("root.Main.DerefTilde"), {{
    Map::create({}, { { S("deref"), 0 }, { S("تتبع"), 0 } }),
    {
    }
  }}, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::DerefOp>::create());
  // ~no_deref
  this->createCommand(S("root.Main.NoDerefTilde"), {{
    Map::create({}, { { S("no_deref"), 0 }, { S("عطل_التتبع"), 0 } }),
    {
    }
  }}, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::NoDerefOp>::create());
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
      {S("frc2"), newSrdObj<TiInt>(1)},
      {S("frc3"), newSrdObj<TiInt>(1)},
      {S("fltr"), newSrdObj<TiInt>(2)}
    })}
  }).get());
  // ~size
  this->createCommand(S("root.Main.SizeTilde"), {{
    Map::create({}, { { S("size"), 0 }, { S("حجم"), 0 } }),
    {
    }
  }}, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::SizeOp>::create());
  // ~type
  this->createCommand(S("root.Main.TypeTilde"), {{
    Map::create({}, { { S("type"), 0 }, { S("صنف"), 0 } }),
    {
    }
  }}, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::TypeOp>::create());
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
      {S("frc2"), newSrdObj<TiInt>(0)},
      {S("frc3"), newSrdObj<TiInt>(0)},
      {S("fltr"), newSrdObj<TiInt>(1)}
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
      {S("frc2"), newSrdObj<TiInt>(0)},
      {S("frc3"), newSrdObj<TiInt>(0)},
      {S("fltr"), newSrdObj<TiInt>(1)}
    })}
  }).get());
  // ~next_arg
  this->createCommand(S("root.Main.NextArgTilde"), {{
    Map::create({}, {{S("next_arg"), 0}, {S("المعطى_التالي"), 0}}),
    {
      {
        PARSE_REF(S("module.CastSubject")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, Spp::Handlers::TildeOpParsingHandler<Spp::Ast::NextArgOp>::create());
  // ~use_in = "~use_in" + Subject + UseInOptions + Set
  this->createCommand(S("root.Main.UseInTilde"), {{
    Map::create({}, {{S("use_in"), 0}, {S("استخدم_في"), 0}}),
    {
      {
        PARSE_REF(S("module.UseInSubject")),
        TiInt::create(0),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.UseInOptions")),
        TiInt::create(0),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      },
      {
        PARSE_REF(S("module.BlockSet")),
        TiInt::create(1),
        TiInt::create(1),
        TiInt::create(ParsingFlags::PASS_ITEMS_UP)
      }
    }
  }}, newSrdObj<Spp::Handlers::UseInParsingHandler>());
  this->set(S("root.Main.UseInSubject"), Module::create({
    {S("baseRef"), PARSE_REF(S("module.owner.Subject")) }
  }).get());
  this->set(S("root.Main.UseInSubject.Sbj"), SymbolDefinition::create({
   {S("baseRef"), PARSE_REF(S("module.base.Sbj"))},
  }, {
    {S("vars"), Map::create({}, {
      {S("sbj1"), PARSE_REF(S("module.Identifier"))},
      {S("sbj2"), PARSE_REF(S("module.Identifier"))},
      {S("sbj3"), PARSE_REF(S("module.Identifier"))},
      {S("frc2"), newSrdObj<TiInt>(1)},
      {S("frc3"), newSrdObj<TiInt>(1)},
      {S("fltr"), newSrdObj<TiInt>(1)}
    })}
  }).get());
  this->createCommand(S("root.Main.UseInOptions"), {{
    Map::create({}, {
      { S("no_injection"), 0 },
      { S("بلا_حقن"), newSrdObj<TiStr>(("no_injection")) }
    }), {}
  }}, newSrdObj<GenericParsingHandler>());

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
    PARSE_REF(S("module.TypeTilde")),
    PARSE_REF(S("module.CastTilde")),
    PARSE_REF(S("module.DerefTilde")),
    PARSE_REF(S("module.NoDerefTilde")),
    PARSE_REF(S("module.ContentTilde")),
    PARSE_REF(S("module.PointerTilde")),
    PARSE_REF(S("module.InitTilde")),
    PARSE_REF(S("module.TerminateTilde")),
    PARSE_REF(S("module.NextArgTilde")),
    PARSE_REF(S("module.UseInTilde"))
  });

  this->addProdsToGroup(S("root.Main.SubjectCmdGrp"), {
    PARSE_REF(S("module.Module")),
    PARSE_REF(S("module.Type")),
    PARSE_REF(S("module.Function")),
    PARSE_REF(S("module.Macro")),
    PARSE_REF(S("module.Preprocess")),
    PARSE_REF(S("module.AstLiteral")),
    PARSE_REF(S("module.Keywords")),
    PARSE_REF(S("module.ThisTypeRef"))
  });
}


void SppFactory::cleanGrammar()
{
  Core::Data::clearCaches(this->rootManager->getRootScope().get());

  this->cleanCustomGrammarAndCommands();

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
    S("class"), S("type"), S("صنف"),
    S("func"), S("function"), S("دالّة"), S("دالة"),
    S("macro"), S("ماكرو"),
    S("preprocess"), S("تمهيد"),
    S("ptr"), S("مؤشر"),
    S("cnt"), S("محتوى"),
    S("deref"), S("تتبع"),
    S("no_deref"), S("عطل_التتبع"),
    S("cast"), S("مثّل"), S("مثل"),
    S("size"), S("حجم"),
    S("ast"), S("شبم"),
    S("ast_ref"), S("سند_شبم"),
    S("this_type"), S("هذا_الصنف"),
    S("handler"), S("عملية"),
    S("this"), S("هذا"),
    S("value"), S("قيمة"),
    S("init"), S("هيئ"),
    S("terminate"), S("اتلف"), S("أتلف"),
    S("integer"), S("صحيح"),
    S("string"), S("محارف"),
    S("any"), S("أيما"),
    S("next_arg"), S("المعطى_التالي"),
    S("as_ptr"), S("كمؤشر"),
    S("set_ptr"), S("حدد_مؤشر"),
    S("no_injection"), S("بلا_حقن")
  });

  // Remove translation for static modifier.
  this->remove(S("root.Main.Def.modifierTranslations.مشترك"));
  this->remove(S("root.Main.Def.modifierTranslations.دون_ربط"));
  this->remove(S("root.Main.Def.modifierTranslations.حقنة"));
  this->remove(S("root.Main.Def.modifierTranslations.عملية"));

  // Remove commands from tilde commands list.
  this->removeProdsFromGroup(S("root.Main.PostfixTildeCmdGrp"), {
    S("module.AstRefTilde"),
    S("module.SizeTilde"),
    S("module.TypeTilde"),
    S("module.CastTilde"),
    S("module.DerefTilde"),
    S("module.NoDerefTilde"),
    S("module.ContentTilde"),
    S("module.PointerTilde"),
    S("module.InitTilde"),
    S("module.TerminateTilde"),
    S("module.NextArgTilde"),
    S("module.UseInTilde")
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
    S("module.Preprocess"),
    S("module.AstLiteral"),
    S("module.Keywords"),
    S("module.ThisTypeRef")
  });

  // Delete tilde command definitions.
  this->tryRemove(S("root.Main.PointerTilde"));
  this->tryRemove(S("root.Main.ContentTilde"));
  this->tryRemove(S("root.Main.DerefTilde"));
  this->tryRemove(S("root.Main.NoDerefTilde"));
  this->tryRemove(S("root.Main.CastTilde"));
  this->tryRemove(S("root.Main.CastSubject"));
  this->tryRemove(S("root.Main.SizeTilde"));
  this->tryRemove(S("root.Main.TypeTilde"));
  this->tryRemove(S("root.Main.AstRefTilde"));
  this->tryRemove(S("root.Main.InitTilde"));
  this->tryRemove(S("root.Main.InitTildeSubject"));
  this->tryRemove(S("root.Main.TerminateTilde"));
  this->tryRemove(S("root.Main.TerminateTildeSubject"));
  this->tryRemove(S("root.Main.NextArgTilde"));
  this->tryRemove(S("root.Main.UseInTilde"));
  this->tryRemove(S("root.Main.UseInSubject"));
  this->tryRemove(S("root.Main.UseInOptions"));

  // Delete leading command definitions.
  this->tryRemove(S("root.Main.If"));
  this->tryRemove(S("root.Main.While"));
  this->tryRemove(S("root.Main.For"));
  this->tryRemove(S("root.Main.Continue"));
  this->tryRemove(S("root.Main.Break"));
  this->tryRemove(S("root.Main.Return"));
  this->tryRemove(S("root.Main.TypeOp"));
  this->tryRemove(S("root.Main.TypeOpOptions"));

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
  this->tryRemove(S("root.Main.SquareArgsSubject"));
  this->tryRemove(S("root.Main.Preprocess"));
  this->tryRemove(S("root.Main.AstLiteral"));
  this->tryRemove(S("root.Main.Keywords"));
  this->tryRemove(S("root.Main.ThisTypeRef"));

  // Delete block definitions.
  this->set(S("root.Main.Expression.subject"), getSharedPtr(this->get(S("root.Main.Expression._subjectBackup"))));
  this->tryRemove(S("root.Main.Expression._subjectBackup"));
  this->tryRemove(S("root.Main.BlockSet"));
  this->tryRemove(S("root.Main.BlockStatements"));
  this->tryRemove(S("root.Main.BlockSubject"));
}


Bool SppFactory::createCustomCommand(
  Char const *qualifier, TiObject *ast, ParsingHandlerFunc func, Core::Notices::Store *noticeStore
) {
  Core::Data::clearCaches(this->context.getRoot());

  // TODO: Allow creating commands in places other than root.Main (like tilde commands for example).
  Array<TiObject*> sectionList;
  convertInfixOpIntoList(ast, S("+"), sectionList);
  std::vector<CommandSection> sections;
  for (Int i = 0; i < sectionList.getLength(); ++i) {
    CommandSection section;
    if (!parseCommandSection(sectionList.at(i), section, noticeStore)) return false;
    sections.push_back(section);
  }
  this->createCommand(Str(S("root.Main.")) + qualifier, sections, newSrdObj<CustomParsingHandler>(
    [=](Parser *parser, ParserState *state)
    {
      auto data = state->getData();
      state->setData(func(data));
    }
  ));
  this->addProdsToGroup(S("root.Main.SubjectCmdGrp"), {
    PARSE_REF(Str(S("module.")) + qualifier),
  });
  // Add keywords.
  auto tokenizingHandler = this->get<Core::Processing::Handlers::IdentifierTokenizingHandler>(
    S("root.LexerDefs.Identifier.handler")
  );
  for (Int i = 0; i < sections.size(); ++i) {
    for (Int j = 0; j < sections.at(i).keywords->getCount(); j++) {
      tokenizingHandler->addKeyword(sections.at(i).keywords->getKey(j));
      this->customCommandKeywords.add(Str(sections.at(i).keywords->getKey(j)));
    }
  }
  // Store qualifier for later deletion.
  this->customCommandQualifiers.add(Str(qualifier));
  return true;
}


Bool SppFactory::createCustomGrammar(
  Char const *qualifier, Char const *baseQualifier, TiObject *overridesAst, Core::Notices::Store *noticeStore
) {
  Core::Data::clearCaches(this->context.getRoot());

  // Create the grammar root object.
  auto obj = this->get(Str(S("root.Main.")) + baseQualifier);
  auto factory = obj != 0 ? obj->getMyTypeInfo()->getFactory() : 0;
  if (factory == 0) return false;
  auto clone = factory->createShared();
  auto cloneBinding = ti_cast<Binding>(clone);
  if (cloneBinding != 0) {
    cloneBinding->setMember(S("baseRef"), PARSE_REF(Str(S("module.owner.")) + baseQualifier));
  }
  this->set(Str(S("root.Main.")) + qualifier, clone);

  // Store qualifier for later deletion.
  this->customGrammarQualifiers.add(Str(qualifier));

  // Override sub-objects.
  return overrideTree(clone.get(), Str("module.base"), overridesAst, 0, noticeStore);
}


void SppFactory::cleanCustomGrammarAndCommands()
{
  // Clean custom grammar.
  for (Int i = 0; i < this->customGrammarQualifiers.getLength(); ++i) {
    this->tryRemove(Str(S("root.Main.")) + this->customGrammarQualifiers.at(i));
  }
  this->customGrammarQualifiers.clear();

  // Clean all keywords.
  auto tokenizingHandler = this->get<Core::Processing::Handlers::IdentifierTokenizingHandler>(
    S("root.LexerDefs.Identifier.handler")
  );
  for (Int i = 0; i < this->customCommandKeywords.getLength(); ++i) {
    tokenizingHandler->removeKeyword(this->customCommandKeywords.at(i));
  }
  this->customCommandKeywords.clear();

  // Clean the commands themselves.
  for (Int i = 0; i < this->customCommandQualifiers.getLength(); ++i) {
    this->removeProdsFromGroup(S("root.Main.SubjectCmdGrp"), {
      Str(S("module.")) + this->customCommandQualifiers.at(i)
    });
    this->tryRemove(Str(S("root.Main.")) + this->customCommandQualifiers.at(i));
  }
  this->customCommandQualifiers.clear();
}

} // namespace
