/**
 * @file Core/Data/Grammar/StandardFactory.cpp
 * Contains the implementation of class Core::Data::Grammar::StandardFactory.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Grammar
{

using namespace Core::Data::Grammar;
using namespace Core::Processing;
using namespace Core::Processing::Handlers;

//==============================================================================
// Member Functions

/**
 * Creates the entire list of definitions for productions, tokens, and char
 * groups for the Core's grammar. This function will give you the complete
 * grammar definitions with all the required handlers.
 */
void StandardFactory::createGrammar(
  DynamicContaining<TiObject> *rootScope, Main::RootManager *root, Bool exprOnly
) {
  this->setRootScope(rootScope);

  this->constTokenPrefix = S("LexerDefs");
  this->constTokenId = ID_GENERATOR->getId(S("CONSTTOKEN"));

  // Instantiate handlers.
  this->stringLiteralHandler = std::make_shared<StringLiteralTokenizingHandler>(
    StringLiteralTokenizingHandler::OuterQuoteType::DOUBLE
  );
  this->charLiteralHandler = std::make_shared<StringLiteralTokenizingHandler>(
    StringLiteralTokenizingHandler::OuterQuoteType::SINGLE
  );
  this->constTokenHandler = std::make_shared<ConstTokenizingHandler>(this->constTokenId);
  this->parsingHandler = std::make_shared<GenericParsingHandler>();
  this->importHandler = std::make_shared<ImportParsingHandler>(root);
  this->dumpAstParsingHandler = std::make_shared<DumpAstParsingHandler>(root);
  this->leadingModifierHandler = std::make_shared<ModifierParsingHandler>(true);
  this->trailingModifierHandler = std::make_shared<ModifierParsingHandler>(false);
  this->doCommandParsingHandler = std::make_shared<GenericCommandParsingHandler>(S("do"));
  this->rootScopeParsingHandler = std::make_shared<RootScopeParsingHandler>(root->getRootScopeHandler());

  // Create lexer definitions.
  this->set(S("root.LexerDefs"), Module::create({}));
  this->createCharGroupDefinitions();
  this->createTokenDefinitions();

  // Create parser definitions.
  this->createProductionDefinitions(exprOnly);

  // Create error sync block pairs.
  this->set(S("root.ErrorSyncBlockPairs"), List::create({}, {
    TokenTerm::create({
      {S("tokenId"), TiInt::create(this->constTokenId)},
      {S("tokenText"), TiStr::create(S("("))}
    }),
    TokenTerm::create({
      {S("tokenId"), TiInt::create(this->constTokenId)},
      {S("tokenText"), TiStr::create(S(")"))}
    }),
    TokenTerm::create({
      {S("tokenId"), TiInt::create(this->constTokenId)},
      {S("tokenText"), TiStr::create(S("["))}
    }),
    TokenTerm::create({
      {S("tokenId"), TiInt::create(this->constTokenId)},
      {S("tokenText"), TiStr::create(S("]"))}
    }),
    TokenTerm::create({
      {S("tokenId"), TiInt::create(this->constTokenId)},
      {S("tokenText"), TiStr::create(S("{"))}
    }),
    TokenTerm::create({
      {S("tokenId"), TiInt::create(this->constTokenId)},
      {S("tokenText"), TiStr::create(S("}"))}
    })
  }));

  // Set start production and lexer module.
  Module *rootModule = this->context.getRoot();
  rootModule->setStartRef(PARSE_REF(S("module.Program")));
  rootModule->setLexerModuleRef(PARSE_REF(S("root.LexerDefs")));
  rootModule->setErrorSyncBlockPairsRef(PARSE_REF(S("root.ErrorSyncBlockPairs")));

  // Generate const token definitions from production definitions.
  this->generateConstTokenDefinitions();
}


/**
 * Create a CharGroupDefinitionList and add all the required definitions for
 * the Core's grammar.
 */
void StandardFactory::createCharGroupDefinitions()
{
  // BinDigit : char '0'..'1';
  this->set(S("root.LexerDefs.BinDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(S("0"), S("1"))
  ));

  // OctDigit : char '0'..'7';
  this->set(S("root.LexerDefs.OctDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(S("0"), S("7"))
  ));

  // DecDigit : char '0'..'9';
  this->set(S("root.LexerDefs.DecDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(S("0"), S("9"))
  ));

  // HexDigit : char '0'..'9', 'a'..'f', 'A'..'F';
  this->set(S("root.LexerDefs.HexDigit"), CharGroupDefinition::create(
    UnionCharGroupUnit::create({
      SequenceCharGroupUnit::create(S("0"), S("9")),
      SequenceCharGroupUnit::create(S("a"), S("f")),
      SequenceCharGroupUnit::create(S("A"), S("F"))
    })
  ));

  // Letter : char 'a'..'z', 'A'..'Z', '_', 0h0620..0h065F, 0h066E..0h06DC;
  SharedPtr<CharGroupUnit> letterCharGroup = UnionCharGroupUnit::create({
    SequenceCharGroupUnit::create(S("a"), S("z")),
    SequenceCharGroupUnit::create(S("A"), S("Z")),
    SequenceCharGroupUnit::create(S("_"), S("_")),
    SequenceCharGroupUnit::create(S("ؠ"), S("ٟ")),
    SequenceCharGroupUnit::create(S("ٮ"), S("ۜ"))
  });
  this->set(S("root.LexerDefs.Letter"), CharGroupDefinition::create(letterCharGroup));

  // AnyChar : char any;
  this->set(S("root.LexerDefs.AnyChar"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(WCHAR_MIN, WCHAR_MAX)
  ));

  // AnyCharNoEs : char !('\\');
  this->set(S("root.LexerDefs.AnyCharNoEs"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      SequenceCharGroupUnit::create(S("\\"), S("\\"))
    )
  ));

  // AnyCharNoEsOrSingleQuote : char !("\\'");
  this->set(S("root.LexerDefs.AnyCharNoEsOrSingleQuote"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      RandomCharGroupUnit::create(S("\\'"))
    )
  ));

  // AnyCharNoEsOrDoubleQuote : char !("\\\"");
  this->set(S("root.LexerDefs.AnyCharNoEsOrDoubleQuote"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      RandomCharGroupUnit::create(S("\\\""))
    )
  ));

  // AnyCharNoReturn = !('\\');
  this->set(S("root.LexerDefs.AnyCharNoReturn"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      SequenceCharGroupUnit::create(S("\n"), S("\n"))
    )
  ));

  // Spacing : char " \n\r\t";
  this->set(S("root.LexerDefs.Spacing"), CharGroupDefinition::create(
    RandomCharGroupUnit::create(S(" \r\n\t"))
  ));
}


/**
 * Create a TokenDefinitionList and add all the required definitions for
 * the Core's grammar.
 */
void StandardFactory::createTokenDefinitions()
{
  // Identifier : trule as { Letter + (Letter || DecDigit)*(0,endless) };
  this->set(S("root.LexerDefs.Identifier"), SymbolDefinition::create({
    {S("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
  }, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
         CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.Letter")) }}),
         MultiplyTerm::create({}, {
           {S("term"), AlternateTerm::create({}, {
              {S("terms"), List::create({}, {
                 CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.Letter")) }}),
                 CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.DecDigit")) }})
               })}
            })}
         })
       })}
    })}
  }));

  // IntLiteral : trule as {
  //   (DecIntLiteral || BinIntLiteral || OctIntLiteral || HexIntLiteral) +
  //   ("u" || "U")*(0,1) + (("i" || "I") + DecIntLiteral)*(0,1)
  // };
  this->set(S("root.LexerDefs.IntLiteral"), SymbolDefinition::create({
    {S("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
  }, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        AlternateTerm::create({}, {
          {S("terms"), List::create({}, {
            ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.DecIntLiteral")) }}),
            ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.BinIntLiteral")) }}),
            ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.OctIntLiteral")) }}),
            ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.HexIntLiteral")) }})
          })}
        }),
        MultiplyTerm::create({
          {S("max"), std::make_shared<TiInt>(1)}
          }, {
          {S("term"), AlternateTerm::create({}, {
            {S("terms"), List::create({}, {
              ConstTerm::create({{ S("matchString"), TiWStr(S("u")) }}),
              ConstTerm::create({{ S("matchString"), TiWStr(S("U")) }}),
              ConstTerm::create({{ S("matchString"), TiWStr(S("ط")) }})
            })}
          })}
        }),
        MultiplyTerm::create({
          {S("max"), std::make_shared<TiInt>(1)}
          }, {
          {S("term"), ConcatTerm::create({}, {
            {S("terms"), List::create({}, {
              AlternateTerm::create({}, {
                {S("terms"), List::create({}, {
                  ConstTerm::create({{ S("matchString"), TiWStr(S("i")) }}),
                  ConstTerm::create({{ S("matchString"), TiWStr(S("I")) }}),
                  ConstTerm::create({{ S("matchString"), TiWStr(S("ص")) }})
                })}
              }),
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.DecIntLiteral")) }})
            })}
          })}
        })
      })}
    })}
  }));

  // @inner DecIntLiteral : trule as { DecDigit*(1,endless) };
  this->set(S("root.LexerDefs.DecIntLiteral"), SymbolDefinition::create({}, {
    {S("term"), MultiplyTerm::create({
      {S("min"), std::make_shared<TiInt>(1)}
    }, {
      {S("term"), CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.DecDigit")) }})}
    })}
  }));

  // @inner BinIntLiteral : trule as { ("0b" || "0B") + BinDigit*(1,endless) };
  this->set(S("root.LexerDefs.BinIntLiteral"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        AlternateTerm::create({}, {
          {S("terms"), List::create({}, {
            ConstTerm::create({{ S("matchString"), TiWStr(S("0b")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("0B")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("0ن")) }})
          })}
        }),
        MultiplyTerm::create({
          {S("min"), std::make_shared<TiInt>(1)}
        }, {
          {S("term"), CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.BinDigit")) }})}
        })
      })}
    })}
  }));

  // @inner OctIntLiteral : trule as { ("0o" || "0O") + OctDigit*(1,endless) };
  this->set(S("root.LexerDefs.OctIntLiteral"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        AlternateTerm::create({}, {
          {S("terms"), List::create({}, {
            ConstTerm::create({{ S("matchString"), TiWStr(S("0o")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("0O")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("0م")) }})
          })}
        }),
        MultiplyTerm::create({
          {S("min"), std::make_shared<TiInt>(1)}
        }, {
          {S("term"), CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.OctDigit")) }})}
        })
      })}
    })}
  }));

  // @inner HexIntLiteral : trule as { ("0h" || "0H") + HexDigit*(1,endless) };
  this->set(S("root.LexerDefs.HexIntLiteral"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        AlternateTerm::create({}, {
          {S("terms"), List::create({}, {
            ConstTerm::create({{ S("matchString"), TiWStr(S("0h")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("0H")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("0x")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("0X")) }})
          })}
        }),
        MultiplyTerm::create({
          {S("min"), std::make_shared<TiInt>(1)}
        }, {
          {S("term"), CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }})}
        })
      })}
    })}
  }));

  // FloatLiteral : trule as {
  //   DecDigit*(1,endless) + FloatPostfix ||
  //   DecDigit*(1,endless) + FloatExponent + FloatPostfix*(0,1) ||
  //   DecDigit*(1,endless) + "." + DecDigit*(1,endless) +
  //     FloatExponent*(0,1) + FloatPostfix*(1,1)
  // };
  this->set(S("root.LexerDefs.FloatLiteral"), SymbolDefinition::create({
    {S("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
  }, {
    {S("term"), AlternateTerm::create({}, {
      {S("terms"), List::create({}, {
        ConcatTerm::create({}, {
          {S("terms"), List::create({}, {
            MultiplyTerm::create({
              {S("min"), std::make_shared<TiInt>(1)}
            }, {
              {S("term"), CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.DecDigit")) }})}
            }),
            ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.FloatPostfix")) }})
          })}
        }),
        ConcatTerm::create({}, {
          {S("terms"), List::create({}, {
            MultiplyTerm::create({
              {S("min"), std::make_shared<TiInt>(1)}
            }, {
              {S("term"), CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.DecDigit")) }})}
            }),
            ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.FloatExponent")) }}),
            MultiplyTerm::create({
              {S("max"), std::make_shared<TiInt>(1)}
            }, {
              {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.FloatPostfix")) }})}
            })
          })}
        }),
        ConcatTerm::create({}, {
          {S("terms"), List::create({}, {
            MultiplyTerm::create({}, {
              {S("term"), CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.DecDigit")) }})}
            }),
            ConstTerm::create({{ S("matchString"), TiWStr(S(".")) }}),
            MultiplyTerm::create({
              {S("min"), std::make_shared<TiInt>(1)}
            }, {
              {S("term"), CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.DecDigit")) }})}
            }),
            MultiplyTerm::create({
              {S("max"), std::make_shared<TiInt>(1)}
            }, {
              {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.FloatExponent")) }})}
            }),
            MultiplyTerm::create({
              {S("max"), std::make_shared<TiInt>(1)}
            }, {
              {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.FloatPostfix")) }})}
            })
          })}
        })
      })}
    })}
  }));

  // @inner FloatExponent : trule as { ("e" || "E") + ("+" || "-")*(0,1) + DecDigit*(1,endless) };
  this->set(S("root.LexerDefs.FloatExponent"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        AlternateTerm::create({}, {
          {S("terms"), List::create({}, {
            ConstTerm::create({{ S("matchString"), TiWStr(S("e")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("E")) }})
          })}
        }),
        MultiplyTerm::create({
          {S("max"), std::make_shared<TiInt>(1)}
        }, {
          {S("term"), AlternateTerm::create({}, {
            {S("terms"), List::create({}, {
              ConstTerm::create({{ S("matchString"), TiWStr(S("+")) }}),
              ConstTerm::create({{ S("matchString"), TiWStr(S("-")) }})
            })}
          })}
        }),
        MultiplyTerm::create({
          {S("min"), std::make_shared<TiInt>(1)}
        }, {
          {S("term"), CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.DecDigit")) }})}
        })
      })}
    })}
  }));

  // @inner FloatPostfix : trule as { ("f" || "F") + DecDigit*(0,endless) };
  this->set(S("root.LexerDefs.FloatPostfix"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
         AlternateTerm::create({}, {
           {S("terms"), List::create({}, {
              ConstTerm::create({{ S("matchString"), TiWStr(S("f")) }}),
              ConstTerm::create({{ S("matchString"), TiWStr(S("F")) }}),
              ConstTerm::create({{ S("matchString"), TiWStr(S("ع")) }})
           })}
         }),
         MultiplyTerm::create({}, {
           {S("term"), CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.DecDigit")) }})}
         })
       })}
    })}
  }));

  // CharLiteral : trule as { "'" + EsCharWithDoubleQuote + "'" + CharCodePostfix*(0,1) };
  // TODO: Add the char_code_postfix part
  this->set(S("root.LexerDefs.CharLiteral"), SymbolDefinition::create({
    {S("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
  }, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
         ConstTerm::create({{ S("matchString"), TiWStr(S("'")) }}),
         ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.EsCharWithDoubleQuote")) }}),
         ConstTerm::create({{ S("matchString"), TiWStr(S("'")) }})
       })}
    })},
    {S("handler"), this->charLiteralHandler.s_cast<TiObject>()}
  }));

  // StringLiteral : trule as {
  //   StringLiteralPart + (Spacing*(0,endless) + StringLiteralPart)*(0,endless) +
  //   CharCodePostfix*(0,1)
  // };
  // TODO: Add the CharCodePostfix part
  this->set(S("root.LexerDefs.StringLiteral"), SymbolDefinition::create({
    {S("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
  }, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.StringLiteralPart")) }}),
        MultiplyTerm::create({}, {
          {S("term"), ConcatTerm::create({}, {
            {S("terms"), List::create({}, {
              MultiplyTerm::create({}, {
                {S("term"), CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.Spacing")) }})}
              }),
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.StringLiteralPart")) }})
            })}
          })}
        })
      })}
    })},
    {S("handler"), this->stringLiteralHandler.s_cast<TiObject>()}
  }));

  // @inner StringLiteralPart : trule as { "\"" + EsCharWithSingleQuote*(0,endless) + "\"" };
  this->set(S("root.LexerDefs.StringLiteralPart"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ConstTerm::create({{ S("matchString"), TiWStr(S("\"")) }}),
        MultiplyTerm::create({}, {
          {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.EsCharWithSingleQuote")) }})}
        }),
        ConstTerm::create({{ S("matchString"), TiWStr(S("\"")) }})
      })}
    })}
  }));

  // @inner EsCharWithSingleQuote : trule as {
  //   AnyCharNoEsOrDoubleQuote || EsCodeSequence ||
  //   alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the heap.escape_sequences part
  this->set(S("root.LexerDefs.EsCharWithSingleQuote"), SymbolDefinition::create({}, {
    {S("term"), AlternateTerm::create({}, {
      {S("terms"), List::create({}, {
         CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.AnyCharNoEsOrDoubleQuote")) }}),
         ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.EsSequence")) }})
       })}
    })}
  }));

  // @inner EsCharWithDoubleQuote : trule as {
  //   AnyCharNoEsOrSingleQuote || EsCodeSequence ||
  //   alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the root.TokenData.escapeSequences part
  this->set(S("root.LexerDefs.EsCharWithDoubleQuote"), SymbolDefinition::create({}, {
    {S("term"), AlternateTerm::create({}, {
      {S("terms"), List::create({}, {
         CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.AnyCharNoEsOrSingleQuote")) }}),
         ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.EsSequence")) }})
       })}
    })}
  }));

  // @inner EsCharWithQuotes : trule as {
  //   AnyCharNoEs || EsCodeSequence || alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the heap.escape_sequences part
  this->set(S("root.LexerDefs.EsCharWithQuotes"), SymbolDefinition::create({}, {
    {S("term"), AlternateTerm::create({}, {
      {S("terms"), List::create({}, {
         CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.AnyCharNoEs")) }}),
         ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.EsSequence")) }})
       })}
    })}
  }));

  // @inner EsSequence : trule as {
  //   '\\' + ('c' + HexDigit*(2,2) ||
  //           'u' + HexDigit*(4,4) ||
  //           'w' + HexDigit*(8,8) ||
  //           'n' || 't' || 'r' || '"' || '\'' || '\\' || 'ج' || 'ت' || 'ر')
  // };
  this->set(S("root.LexerDefs.EsSequence"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ConstTerm::create({{ S("matchString"), TiWStr(S("\\")) }}),
        AlternateTerm::create({}, {
          {S("terms"), List::create({}, {
            ConstTerm::create({{ S("matchString"), TiWStr(S("\\")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("\"")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("'")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("n")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("t")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("f")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("r")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("ج")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("ت")) }}),
            ConstTerm::create({{ S("matchString"), TiWStr(S("ر")) }}),
            ConcatTerm::create({}, {
              {S("terms"), List::create({}, {
                AlternateTerm::create({}, {
                  {S("terms"), List::create({}, {
                    ConstTerm::create({{ S("matchString"), TiWStr(S("h")) }}),
                    ConstTerm::create({{ S("matchString"), TiWStr(S("x")) }})
                  })}
                }),
                CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }}),
                CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }})
              })}
            }),
            ConcatTerm::create({}, {
              {S("terms"), List::create({}, {
                  ConstTerm::create({{ S("matchString"), TiWStr(S("u")) }}),
                  CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }}),
                  CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }}),
                  CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }}),
                  CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }})
              })}
            }),
            ConcatTerm::create({}, {
              {S("terms"), List::create({}, {
                  ConstTerm::create({{ S("matchString"), TiWStr(S("U")) }}),
                  CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }}),
                  CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }}),
                  CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }}),
                  CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }}),
                  CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }}),
                  CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }}),
                  CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }}),
                  CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.HexDigit")) }})
              })}
            })
          })}
        })
      })}
    })}
  }));

  //// IGNORED TOKENS

  // ignore { Spacing*(1,endless) };
  this->set(S("root.LexerDefs.IgnoredSpaces"), SymbolDefinition::create({
    {S("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN|SymbolFlags::IGNORED_TOKEN)}
  }, {
    {S("term"), MultiplyTerm::create({
      {S("min"), std::make_shared<TiInt>(1)}
    }, {
      {S("term"), CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.Spacing")) }})}
    })}
  }));

  // @minimum ignore { "//" + any*(0,endless) + "\n" }
  // For now this is implemented as:
  // ignore { "//" + AnyCharNoReturn*(0,endless) + "\n" }
  // because the lexer still doesn't support the @minimum modifier.
  this->set(S("root.LexerDefs.LineComment"), SymbolDefinition::create({
    {S("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN|SymbolFlags::IGNORED_TOKEN)}
  }, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
         AlternateTerm::create({}, {
           {S("terms"), List::create({}, {
              ConstTerm::create({{ S("matchString"), TiWStr(S("//")) }}),
              ConstTerm::create({{ S("matchString"), TiWStr(S("#")) }}),
           })}
         }),
         MultiplyTerm::create({}, {
           {S("term"), CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.AnyCharNoReturn")) }})}
         }),
         ConstTerm::create({{ S("matchString"), TiWStr(S("\n")) }})
       })}
    })}
  }));

  // ignore { "/*" + any*(0,endless) + "*/" }
  this->set(S("root.LexerDefs.MultilineComment"), SymbolDefinition::create({
    {S("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN|SymbolFlags::IGNORED_TOKEN|SymbolFlags::PREFER_SHORTER)}
  }, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
         ConstTerm::create({{ S("matchString"), TiWStr(S("/*")) }}),
         MultiplyTerm::create({}, {
           {S("term"), CharGroupTerm::create({{ S("charGroupReference"), PARSE_REF(S("module.AnyChar")) }})}
         }),
         ConstTerm::create({{ S("matchString"), TiWStr(S("*/")) }})
       })}
    })}
  }));
}


/**
 * Create a Production_Definition_List and add all the required definitions for
 * the Core's grammar. The created list will have assigned parsing handlers.
 */
void StandardFactory::createProductionDefinitions(Bool exprOnly)
{
  //// Program : Main.Statements.StmtList.
  // Program : prod as Main.Statements.StmtList(DefaultMain.DefaultStatement);
  if (exprOnly) {
    this->set(S("root.Program"), SymbolDefinition::create({}, {
      {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("root.Main.Statements.Stmt")) }})},
      {S("handler"), this->parsingHandler}
    }));
  } else {
    this->set(S("root.Program"), SymbolDefinition::create({}, {
      {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("root.Main.RootStatements.StmtList")) }})},
      {S("handler"), this->parsingHandler}
    }));
  }


  // ProdGroup
  this->set(S("root.ProdGroup"), SymbolDefinition::create({}, {
    {S("term"), AlternateTerm::create({
      {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)},
      {S("targetRef"), PARSE_REF(S("stack.prod"))}
    }, {
      {S("data"), PARSE_REF(S("args.prods"))},
      {S("terms"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("stack.prod")) }})}
    })},
    {S("vars"), Map::create({}, {{ S("prods"), List::create() }} )},
    {S("handler"), this->parsingHandler}
  }));

  this->createTokenDataModule();

  this->createStatementsProductionModule();

  this->createCommandTemplateProductionDefinitions();

  this->createExpressionProductionModule();

  this->createSubjectProductionModule();

  this->createSetProductionDefinitions();

  this->createModifierProductionDefinitions();

  this->createMainProductionModule(exprOnly);
}


void StandardFactory::createTokenDataModule()
{
  this->set(S("root.TokenData"), Module::create({}));
  // assignmentOpList : keywords = ("=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "$=", "<<=", ">>=");
  this->set(S("root.TokenData.assignmentOpList"), Map::create({}, {
   {S("="), 0},
   {S("+="), 0},
   {S("-="), 0},
   {S("*="), 0},
   {S("/="), 0},
   {S("÷="), std::make_shared<TiStr>(("/="))},
   {S("%="), 0},
   {S("&="), 0},
   {S("|="), 0},
   {S("$="), 0},
   {S("<<="), 0},
   {S(">>="), 0}
  }, true));
  // comparisonOpList : keywords = ("==", "!=", "<", ">", "<=", ">=");
  this->set(S("root.TokenData.comparisonOpList"), Map::create({}, {
   {S("=="), 0},
   {S("!="), 0},
   {S("<"), 0},
   {S(">"), 0},
   {S("<="), 0},
   {S(">="), 0}
  }, true));
  // addOpList : keywords = ("+", "-");
  this->set(S("root.TokenData.addOpList"), Map::create({}, {
    {S("+"), 0},
    {S("-"), 0}
  }, true));
  // mulOpList : keywords = ("*", "/", "%");
  this->set(S("root.TokenData.mulOpList"), Map::create({}, {
    {S("*"), 0},
    {S("/"), 0},
    {S("÷"), std::make_shared<TiStr>(S("/"))},
    {S("%"), 0}
  }, true));
  // bitwiseOpList : keywords = ("|", "$", "&", "<<", ">>");
  this->set(S("root.TokenData.bitwiseOpList"), Map::create({}, {
    {S("|"), 0},
    {S("$"), 0},
    {S("&"), 0},
    {S("<<"), 0},
    {S(">>"), 0}
  }, true));
  // logOpList : keywords = ("||", "$$", "&&", "or", "nor", "xor", "xnor", "and", "nand");
  this->set(S("root.TokenData.logOpList"), Map::create({}, {
    {S("||"), 0},
    {S("$$"), 0},
    {S("&&"), 0},
    {S("or"), 0},
    {S("أو"), std::make_shared<TiStr>(S("or"))},
    {S("nor"), 0},
    {S("وليس"), std::make_shared<TiStr>(S("nor"))},
    {S("xor"), 0},
    {S("أو_حصرا"), std::make_shared<TiStr>(S("xor"))},
    {S("xnor"), 0},
    {S("وليس_حصرا"), std::make_shared<TiStr>(S("xnor"))},
    {S("and"), 0},
    {S("و"), std::make_shared<TiStr>(S("and"))},
    {S("nand"), 0},
    {S("أو_ليس"), std::make_shared<TiStr>(S("nand"))}
  }, true));
  // prefixOpList : keywords = ("++", "--", "+", "-", "!", "!!", "not");
  this->set(S("root.TokenData.prefixOpList"), Map::create({}, {
    {S("++"), 0},
    {S("--"), 0},
    {S("+"), 0},
    {S("-"), 0},
    {S("!"), 0},
    {S("!!"), 0},
    {S("not"), 0},
    {S("ليس"), std::make_shared<TiStr>(S("not"))},
    {S("..."), 0}
  }, true));
  // postfixOpList : keywords = ("++", "--");
  this->set(S("root.TokenData.postfixOpList"), Map::create({}, {
    {S("++"), 0},
    {S("--"), 0}
  }, true));
  // linkOpList : keywords = ("->", ".", ".>", "<.");
  this->set(S("root.TokenData.linkOpList"), Map::create({}, {
   {S("->"), 0},
   {S("."), 0},
   {S(".>"), 0},
   {S("<."), 0}
  }, true));
  // lowLinkOpList : keywords = ("=>", "..", "..>", "<..");
  this->set(S("root.TokenData.lowLinkOpList"), Map::create({}, {
   {S("=>"), 0},
   {S(".."), 0},
   {S("..>"), 0},
   {S("<.>"), 0}
  }, true));
  // lowerLinkOpList : keywords = (":", ":>", "<:");
  this->set(S("root.TokenData.lowerLinkOpList"), Map::create({}, {
   {S(":"), 0},
   {S(":>"), 0},
   {S("<:"), 0}
  }, true));
  // lowestLinkOpList : keywords = ("::", ::>", "<::", "in");
  this->set(S("root.TokenData.lowestLinkOpList"), Map::create({}, {
    {S("::"), 0},
    {S("::>"), 0},
    {S("<::"), 0},
    {S("in"), 0},
    {S("في"), std::make_shared<TiStr>(S("in"))}
  }, true));

  this->generateConstTokensForStrings(this->get(S("root.TokenData")));
}


void StandardFactory::createStatementsProductionModule()
{
  this->set(S("root.Statements"), Module::create({}));

  //// StmtList : Stmt { ";" Stmt }.
  // StmtList : prod (stmt:production[Stmt]) as
  //     stmt*(0, 1) + (lexer.Constant(";") + stmt*(0, 1))*(0, endless);
  this->set(S("root.Statements.StmtList"), SymbolDefinition::create({
    {S("flags"), TiInt::create(ParsingFlags::ENFORCE_PROD_OBJ)}
  }, {
    {S("term"), ConcatTerm::create({
      {S("errorSyncPosId"), TiInt(1)},
    }, {
      {S("terms"), List::create({}, {
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
          {S("max"), std::make_shared<TiInt>(1)}
        }, {
          {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.Stmt")) }})}
        }),
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP|TermFlags::ERROR_SYNC_TERM)}
        }, {
          {S("term"), ConcatTerm::create({}, {
            {S("terms"), List::create({}, {
              TokenTerm::create({
                {S("flags"), TiInt::create(ParsingFlags::ENFORCE_TOKEN_OMIT)},
                {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {S("tokenText"), Map::create({}, {{S(";"),0},{S("؛"),0}})}
              }),
              MultiplyTerm::create({
                {S("priority"), std::make_shared<TiInt>(1)},
                {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
                {S("max"), std::make_shared<TiInt>(1)}
              }, {
                {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.Stmt")) }})}
              })
            })}
          })}
        })
      })}
    })},
    {S("handler"), ScopeParsingHandler<Data::Ast::Scope>::create()}
  }));

  //// Statement : (Variation | Variation | ...).
  // Statement : @limit[user.parent==self,child.terms=self] prule
  //   prefix self.id, DefaultModifier
  //   as (variations:list[prule[Variation]]=(ExpVariation, CmdVariation)=>{
  //     alternate (variations:variation)->( variation )
  //   };
  this->set(S("root.Statements.Stmt"), SymbolDefinition::create({}, {
    {S("term"), AlternateTerm::create({
      {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)},
      {S("targetRef"), PARSE_REF(S("stack.variation"))}
    }, {
      {S("data"), PARSE_REF(S("module.variations"))},
      {S("terms"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("stack.variation")) }})}
    })},
    {S("handler"), this->parsingHandler}
  }));

  //// Variation : { Subject }.
  //  Variation : prule as (sections:list[map[prd:valid_subject, min:integer, max:integer, pty:integer]])=>{
  //    concat (sections:s)->( @priority(s.pty,0) s.prd*(s.min,s.max) )
  //  };
  this->set(S("root.Statements.Variation"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({
      {S("errorSyncPosId"), TiInt(1000)},
      {S("targetRef"), PARSE_REF(S("stack.subject"))}
    }, {
      {S("data"), PARSE_REF(S("args.subjects"))},
      {S("terms"), MultiplyTerm::create({
        {S("priority"), PARSE_REF(S("stack.subject.pty"))},
        {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
        {S("min"), PARSE_REF(S("stack.subject.min"))},
        {S("max"), PARSE_REF(S("stack.subject.max"))}
      }, {
        {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("stack.subject.prd")) }})}
      })}
    })}
  }));

  this->set(S("root.Statements.variations"), TioSharedPtr::null);
}


void StandardFactory::createCommandTemplateProductionDefinitions()
{
  //// Cmd : keyword {Subject}.
  // Cmd : @limit[user.parent==root.Command] prule
  //   as (kwd:keywords, args:list[map[prd:valid_subject, min:integer, max:integer, pty:integer]])=>{
  //     root.KeywordGroup(kwd) + concat (args:a)->( @priority(a.pty,0) a.prd*(a.min,a.max) )
  //   };
  this->set(S("root.Cmd"), SymbolDefinition::create({
    {S("flags"), TiInt::create(ParsingFlags::ENFORCE_PROD_OBJ)}
  }, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        TokenTerm::create({
          {S("tokenId"), std::make_shared<TiInt>(ID_GENERATOR->getId(S("LexerDefs.Identifier")))},
          {S("tokenText"), PARSE_REF(S("args.kwd"))}
        }),
        ConcatTerm::create({
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
          {S("targetRef"), PARSE_REF(S("stack.p"))}
        }, {
          {S("data"), PARSE_REF(S("args.prms"))},
          {S("terms"), MultiplyTerm::create({
            {S("priority"), PARSE_REF(S("stack.p.pty"))},
            {S("flags"), PARSE_REF(S("stack.p.flags"))},
            {S("min"), PARSE_REF(S("stack.p.min"))},
            {S("max"), PARSE_REF(S("stack.p.max"))}
          }, {
            {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("stack.p.prd")) }})}
          })}
        })
      })}
    })},
    {S("handler"), this->parsingHandler}
  }));

  //// MultiCmd : {keyword {Subject}}.
  // MultiCmd : @limit[user.parent==root.Command] prule as
  //   (sections:list[map[kwd:keywords, min:integer, max:integer, pty:integer,
  //     args:list[map[prd:valid_subject, min:integer, max:integer, pty:integer]]
  //   ]])=>{
  //     concat (sections:s)->(
  //       @priority(s.pty,0) (root.KeywordGroup(s.kwd) + concat (s.args:a)->(
  //         @priority(a.pty,0) a.arg*(a.min, a.max)
  //       ))*(s.min, s.max)
  //     )
  //   };
  this->set(S("root.MultiCmd"), SymbolDefinition::create({
    {S("flags"), TiInt::create(ParsingFlags::ENFORCE_PROD_OBJ)}
  }, {
    {S("term"), ConcatTerm::create({
      {S("targetRef"), PARSE_REF(S("stack.s"))}
    }, {
      {S("data"), PARSE_REF(S("args.sections"))},
      {S("terms"), MultiplyTerm::create({
        {S("priority"), PARSE_REF(S("stack.s.pty"))},
        {S("flags"), PARSE_REF(S("stack.s.flags"))},
        {S("min"), PARSE_REF(S("stack.s.min"))},
        {S("max"), PARSE_REF(S("stack.s.max"))},
      }, {
        {S("term"), ConcatTerm::create({
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        }, {
          {S("terms"), List::create({}, {
            TokenTerm::create({
              {S("tokenId"), std::make_shared<TiInt>(ID_GENERATOR->getId(S("LexerDefs.Identifier")))},
              {S("tokenText"), PARSE_REF(S("stack.s.kwd"))}
            }),
            ConcatTerm::create({
              {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
              {S("targetRef"), PARSE_REF(S("stack.a"))}
            }, {
              {S("data"), PARSE_REF(S("stack.s.args"))},
              {S("terms"), MultiplyTerm::create({
                {S("priority"), PARSE_REF(S("stack.a.pty"))},
                {S("flags"), PARSE_REF(S("stack.a.flags"))},
                {S("min"), PARSE_REF(S("stack.a.min"))},
                {S("max"), PARSE_REF(S("stack.a.max"))}
              }, {
                {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("stack.a.prd")) }})}
              })}
            })
          })}
        })}
      })}
    })},
    {S("handler"), this->parsingHandler}
  }));
}


void StandardFactory::createExpressionProductionModule()
{
  // Expression :
  // operand {binaryOp operand}.
  // [unaryOp] operand.
  // operand [unaryOp].
  // operand {FunctionalOp}.
  this->set(
    S("root.Expression"),
    Module::create({{S("startRef"), PARSE_REF(S("module.Exp"))}}).get()
  );

  // Exp : @single prod as LowestLinkExp + (@priority(in,0) lexer.Constant("\\")*(0,1));
  this->set(S("root.Expression.Exp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.LowestLinkExp")) }}),
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
          {S("max"), std::make_shared<TiInt>(1)},
        }, {
          {S("term"), TokenTerm::create({
            {S("tokenId"), TiInt::create(this->constTokenId)},
            {S("tokenText"), TiStr::create(S("\\"))}
          })}
        })
      })}
    })},
    {S("handler"), this->parsingHandler}
  }));

  // LowestLinkExp : @single @prefix(heap.Modifiers.LowestLinkModifierCmd)
  //     prod (enable:integer=endless) as
  //     ConditionalExp + (@priority(in,0) (LowestLinkOp + ConditionalExp)*(0,enable));
  this->set(S("root.Expression.LowestLinkExp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.ConditionalExp")) }}),
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {S("max"), PARSE_REF(S("args.enable"))},
        }, {
          {S("term"), ConcatTerm::create({}, {
            {S("terms"), List::create({}, {
              TokenTerm::create({
                {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {S("tokenText"), PARSE_REF(S("root.TokenData.lowestLinkOpList"))}
              }),
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.ConditionalExp")) }})
            })}
          })}
        })
      })}
    })},
    {S("vars"), Map::create({}, {{S("enable"), 0}})},
    {S("handler"), std::make_shared<InfixParsingHandler<Ast::LinkOperator>>(false)}
  }));

  // ConditionalExp : @single @prefix(heap.Modifiers.ConditionalModifierCmd)
  //     prod (enable:integer[0<=n<=1]=1) as
  //     ListExp + (@priority(in,0) (lexer.Constant("?") + ListExp)*(0,enable));
  this->set(S("root.Expression.ConditionalExp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.ListExp")) }}),
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {S("max"), PARSE_REF(S("args.enable"))}
        }, {
          {S("term"), ConcatTerm::create({}, {
            {S("terms"), List::create({}, {
              TokenTerm::create({
                {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {S("tokenText"), Map::create({}, {{S("?"), 0}, {S("؟"), 0}})}
              }),
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.ListExp")) }})
            })}
          })}
        })
      })}
    })},
    {S("vars"), Map::create({}, {{S("enable"), std::make_shared<TiInt>(1)}})},
    {S("handler"), std::make_shared<InfixParsingHandler<Ast::ConditionalOperator>>(false)}
  }));

  // ListExp : @single @prefix(heap.Modifiers.ListModifierCmd)
  //   prod (enable:integer=1) as
  //     @filter(enable) LowerLinkExp ||
  //                     (LowerLinkExp || lexer.Constant(",") + LowerLinkExp*(0,1)) +
  //                       (@priority(in,0) (lexer.Constant(",") + LowerLinkExp*(0,1))*(0,endless));
  this->set(S("root.Expression.ListExp"), SymbolDefinition::create({}, {
    {S("term"), AlternateTerm::create({}, {
      {S("data"), PARSE_REF(S("args.enable"))},
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.LowerLinkExp")) }}),
        ConcatTerm::create({
          {S("flags"), TiInt::create(ParsingFlags::ENFORCE_LIST_ITEM)}
        }, {
          {S("terms"), List::create({}, {
            AlternateTerm::create({}, {
              {S("terms"), List::create({}, {
                ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.LowerLinkExp")) }}),
                ConcatTerm::create({}, {
                  {S("terms"), List::create({}, {
                    TokenTerm::create({
                      {S("flags"), TiInt::create(ParsingFlags::ENFORCE_TOKEN_OMIT)},
                      {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                      {S("tokenText"), Map::create({}, {{S(","), 0}, {S("،"), 0}})}
                    }),
                    MultiplyTerm::create({
                      {S("priority"), std::make_shared<TiInt>(1)},
                      {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
                      {S("max"), std::make_shared<TiInt>(1)},
                    }, {
                      {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.LowerLinkExp")) }})}
                    })
                  })}
                })
              })}
            }),
            MultiplyTerm::create({
              {S("priority"), std::make_shared<TiInt>(1)},
              {S("flags"), TiInt::create(
                TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP|ParsingFlags::ENFORCE_LIST_ITEM
              )}
            }, {
              {S("term"), ConcatTerm::create({}, {
                {S("terms"), List::create({}, {
                  TokenTerm::create({
                    {S("flags"), TiInt::create(ParsingFlags::ENFORCE_TOKEN_OMIT)},
                    {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                    {S("tokenText"), Map::create({}, {{S(","), 0}, {S("،"), 0}})}
                  }),
                  MultiplyTerm::create({
                    {S("priority"), std::make_shared<TiInt>(1)},
                    {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
                    {S("max"), std::make_shared<TiInt>(1)}
                  }, {
                    {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.LowerLinkExp")) }})}
                  })
                })}
              })}
            })
          })}
        })
      })}
    })},
    {S("vars"), Map::create({}, {{S("enable"), std::make_shared<TiInt>(1)}})},
    {S("handler"), std::make_shared<ListExpParsingHandler<Ast::List>>()}
  }));

  // LowerLinkExp : @single @prefix(heap.Modifiers.LowerLinkModifierCmd)
  //     prod (enable:integer=endless) as
  //     AssignmentExp + (@priority(in,0) (LowerLinkOp + AssignmentExp)*(0,enable));
  this->set(S("root.Expression.LowerLinkExp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.AssignmentExp")) }}),
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {S("max"), PARSE_REF(S("args.enable"))}
        }, {
          {S("term"), ConcatTerm::create({}, {
            {S("terms"), List::create({}, {
              TokenTerm::create({
                {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {S("tokenText"), PARSE_REF(S("root.TokenData.lowerLinkOpList"))}
              }),
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.AssignmentExp")) }})
            })}
          })}
        })
      })}
    })},
    {S("vars"), Map::create({}, {{S("enable"), 0}})},
    {S("handler"), std::make_shared<InfixParsingHandler<Ast::LinkOperator>>(false)}
  }));

  // AssignmentExp : @single @prefix(heap.Modifiers.AssignmentModifierCmd)
  //     prod (enable:integer=endless) as
  //     LogExp + (@priority(in,0) (AssignmentOp + LogExp)*(0,enable));
  this->set(S("root.Expression.AssignmentExp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.LogExp")) }}),
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {S("max"), PARSE_REF(S("args.enable"))}
        }, {
          {S("term"), ConcatTerm::create({}, {
            {S("terms"), List::create({}, {
              TokenTerm::create({
                {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {S("tokenText"), PARSE_REF(S("root.TokenData.assignmentOpList"))}
              }),
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.LogExp")) }})
            })}
          })}
        })
      })}
    })},
    {S("vars"), Map::create({}, {{S("enable"), 0}})},
    {S("handler"), std::make_shared<InfixParsingHandler<Ast::AssignmentOperator>>(false)}
  }));

  // LogExp : @single @prefix(heap.Modifiers.LogModifierCmd)
  //     prod (enable:integer=endless) as
  //     ComparisonExp + (@priority(in,0) (LogOp + ComparisonExp)*(0,enable));
  this->set(S("root.Expression.LogExp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.ComparisonExp")) }}),
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {S("max"), PARSE_REF(S("args.enable"))}
        }, {
          {S("term"), ConcatTerm::create({}, {
            {S("terms"), List::create({}, {
              TokenTerm::create({
                {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {S("tokenText"), PARSE_REF(S("root.TokenData.logOpList"))}
              }),
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.ComparisonExp")) }})
            })}
          })}
        })
      })}
    })},
    {S("vars"), Map::create({}, {{S("enable"), 0}})},
    {S("handler"), std::make_shared<InfixParsingHandler<Ast::LogOperator>>(false)}
  }));

  // ComparisonExp : @single @prefix(heap.Modifiers.ComparisonModifierCmd)
  //     prod (enable:integer=endless) as
  //     LowLinkExp + (@priority(in,0) (ComparisonOp + LowLinkExp)*(0,enable));
  this->set(S("root.Expression.ComparisonExp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.LowLinkExp")) }}),
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {S("max"), PARSE_REF(S("args.enable"))}
        }, {
          {S("term"), ConcatTerm::create({}, {
            {S("terms"), List::create({}, {
              TokenTerm::create({
                {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {S("tokenText"), PARSE_REF(S("root.TokenData.comparisonOpList"))}
              }),
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.LowLinkExp")) }})
            })}
          })}
        })
      })}
    })},
    {S("vars"), Map::create({}, {{S("enable"), 0}})},
    {S("handler"), std::make_shared<InfixParsingHandler<Ast::ComparisonOperator>>(false)}
  }));

  // LowLinkExp : @single @prefix(heap.Modifiers.LowLinkModifierCmd)
  //     prod (enable:integer=endless) as
  //     AddExp + (@priority(in,0) (LowLinkOp + AddExp)*(0,enable));
  this->set(S("root.Expression.LowLinkExp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.AddExp")) }}),
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {S("max"), PARSE_REF(S("args.enable"))}
        }, {
          {S("term"), ConcatTerm::create({}, {
            {S("terms"), List::create({}, {
              TokenTerm::create({
                {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {S("tokenText"), PARSE_REF(S("root.TokenData.lowLinkOpList"))}
              }),
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.AddExp")) }})
            })}
          })}
        })
      })}
    })},
    {S("vars"), Map::create({}, {{S("enable"), 0}})},
    {S("handler"), std::make_shared<InfixParsingHandler<Ast::LinkOperator>>(false)}
  }));

  // AddExp : @single @prefix(heap.Modifiers.AddModifierCmd)
  //     prod (enable:integer=endless) as
  //     MulExp + (@priority(in,0) (AddOp + MulExp)*(0,enable));
  this->set(S("root.Expression.AddExp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.MulExp")) }}),
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {S("max"), PARSE_REF(S("args.enable"))}
        }, {
          {S("term"), ConcatTerm::create({}, {
            {S("terms"), List::create({}, {
              TokenTerm::create({
                {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {S("tokenText"), PARSE_REF(S("root.TokenData.addOpList"))}
              }),
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.MulExp")) }})
            })}
          })}
        })
      })}
    })},
    {S("vars"), Map::create({}, {{S("enable"), 0}})},
    {S("handler"), std::make_shared<InfixParsingHandler<Ast::AdditionOperator>>(false)}
  }));

  // MulExp : @single @prefix(heap.Modifiers.MulModifierCmd)
  //     prod (enable:integer=endless) as
  //     BitwiseExp + (@priority(in,0) (MulOp + BitwiseExp)*(0,enable));
  this->set(S("root.Expression.MulExp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.BitwiseExp")) }}),
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {S("max"), PARSE_REF(S("args.enable"))}
        }, {
          {S("term"), ConcatTerm::create({}, {
            {S("terms"), List::create({}, {
              TokenTerm::create({
                {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {S("tokenText"), PARSE_REF(S("root.TokenData.mulOpList"))}
              }),
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.BitwiseExp")) }})
            })}
          })}
        })
      })}
    })},
    {S("vars"), Map::create({}, {{S("enable"), 0}})},
    {S("handler"), std::make_shared<InfixParsingHandler<Ast::MultiplicationOperator>>(false)}
  }));

  // BitwiseExp : @single @prefix(heap.Modifiers.BitwiseModifierCmd)
  //     prod (enable:integer=endless) as
  //     UnaryExp + (@priority(in,0) (BitwiseOp + UnaryExp)*(0,enable));
  this->set(S("root.Expression.BitwiseExp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.UnaryExp")) }}),
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {S("max"), PARSE_REF(S("args.enable"))},
        }, {
          {S("term"), ConcatTerm::create({}, {
            {S("terms"), List::create({}, {
              TokenTerm::create({
                {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {S("tokenText"), PARSE_REF(S("root.TokenData.bitwiseOpList"))}
              }),
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.UnaryExp")) }})
            })}
          })}
        })
      })}
    })},
    {S("vars"), Map::create({}, {{S("enable"), 0}})},
    {S("handler"), std::make_shared<InfixParsingHandler<Ast::BitwiseOperator>>(false)}
  }));

  // UnaryExp : @single @prefix(heap.Modifiers.UnaryModifierCmd)
  //     prod (enable1:integer[0<=n<=1]=1, enable2:integer[0<=n<=1]=1) as
  //     (@priority(in,0) PrefixOp*(0,enable1)) + FunctionalExp + (@priority(in,0) PostfixOp*(0,enable2));
  this->set(S("root.Expression.UnaryExp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {S("max"), PARSE_REF(S("args.enable1"))}
        }, {
          {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.PrefixOp")) }})}
        }),
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.FunctionalExp")) }}),
        MultiplyTerm::create({
          {S("priority"), std::make_shared<TiInt>(1)},
          {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {S("max"), PARSE_REF(S("args.enable2"))}
        }, {
          {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.PostfixOp")) }})}
        })
      })}
    })},
    {S("vars"), Map::create({}, {
      {S("enable1"), std::make_shared<TiInt>(1)},
      {S("enable2"), std::make_shared<TiInt>(1)}
    })},
    {S("handler"), std::make_shared<OutfixParsingHandler<Ast::PrefixOperator, Ast::PostfixOperator>>()}
  }));

  // FunctionalExp : @single @prefix(heap.Modifiers.FunctionalModifierCmd)
  //     prod (operand:production[heap.Subject]=heap.Subject, fltr1:filter=null,
  //           fltr2:filter=null, dup:integer=endless, pty1:integer=1, pty2:integer=in) as
  //     @filter(fltr1) @priority(pty1,0)
  //         (operand + (@priority(pty2,0) (@filter(fltr2) ParamPassExp ||
  //                                                       PostfixTildeExp ||
  //                                                       LinkExp(operand))*(0,dup))) ||
  //         PrefixTildeExp + operand;
  this->set(S("root.Expression.FunctionalExp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.subject")) }}),
        MultiplyTerm::create({
          {S("priority"), PARSE_REF(S("args.pty2"))},
          {S("flags"), PARSE_REF(S("args.flags"))},
          {S("max"), PARSE_REF(S("args.dup"))}
        }, {
          {S("term"), AlternateTerm::create({}, {
            {S("data"), PARSE_REF(S("args.fltr2"))},
            {S("terms"), List::create({}, {
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.LinkExp")) }}),
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.PostfixTildeExp")) }}),
              ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.ParamPassExp")) }})
            })}
          })}
        })
      })}
    })},
    {S("vars"), Map::create({}, {
      {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
      {S("pty2"), std::make_shared<TiInt>(1)},
      {S("dup"), 0},
      {S("fltr2"), 0}
    })},
    {S("handler"), std::make_shared<ChainOpParsingHandler>()}
  }));

  // LinkExp : @single prod (operand:production[heap.Subject]=heap.Subject) as LinkOp + operand;
  this->set(S("root.Expression.LinkExp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
       {S("terms"), List::create({}, {
          TokenTerm::create({
            {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
            {S("tokenText"), PARSE_REF(S("root.TokenData.linkOpList"))}
          }),
          ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.subject")) }})
        })}
     })},
    {S("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto currentList = state->getData().ti_cast_get<Containing<TiObject>>();
      auto metadata = ti_cast<Ast::MetaHaving>(currentList);
      auto token = ti_cast<Ast::Token>(currentList->getElement(0));
      auto linkOp = Ast::LinkOperator::create({
        { "prodId", metadata->getProdId() },
        { "sourceLocation", metadata->findSourceLocation() }
      });
      linkOp->setType(token->getText());
      linkOp->setSecond(getSharedPtr(currentList->getElement(1)));
      state->setData(linkOp);
    })}
  }));

  //// ParamPassExp : "(" [Expression] ")" | "[" [Expression] "]".
  // ParamPassExp : @single prod (expr:production[Expression||Statement]=heap.Expression, fltr:filter=null) as
  //     @filter(fltr) lexer.Constant("(") + expr*(0,1) + lexer.Constant(")") ||
  //                   lexer.Constant("[") + expr*(0,1) + lexer.Constant("]");
  this->set(S("root.Expression.ParamPassExp"), SymbolDefinition::create({}, {
    {S("term"), AlternateTerm::create({
      {S("flags"), TiInt::create(ParsingFlags::ENFORCE_ROUTE_OBJ|TermFlags::ONE_ROUTE_TERM)},
    }, {
      {S("data"), PARSE_REF(S("args.fltr"))},
      {S("terms"), List::create({}, {
        ConcatTerm::create({
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
        }, {
          {S("terms"), List::create({}, {
            TokenTerm::create({
              {S("tokenId"), TiInt::create(this->constTokenId)},
              {S("tokenText"), TiStr::create(S("("))}
            }),
            MultiplyTerm::create({
              {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
              {S("max"), std::make_shared<TiInt>(1)},
            }, {
              {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.paramPassExpr")) }})}
            }),
            TokenTerm::create({
              {S("tokenId"), TiInt::create(this->constTokenId)},
              {S("tokenText"), TiStr::create(S(")"))}
            })
          })}
        }),
        ConcatTerm::create({
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
        }, {
          {S("terms"), List::create({}, {
            TokenTerm::create({
              {S("tokenId"), TiInt::create(this->constTokenId)},
              {S("tokenText"), TiStr::create(S("["))}
            }),
            MultiplyTerm::create({
              {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
              {S("max"), std::make_shared<TiInt>(1)}
            }, {
              {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.paramPassExpr")) }})}
            }),
            TokenTerm::create({
              {S("tokenId"), TiInt::create(this->constTokenId)},
              {S("tokenText"), TiStr::create(S("]"))}
            })
          })}
        })
      })}
    })},
    {S("vars"), Map::create({}, {{S("fltr"), 0}})},
    {S("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto currentRoute = state->getData().ti_cast_get<Ast::Route>();
      auto paramPass = Ast::ParamPass::create({
        { "prodId", currentRoute->getProdId() },
        { "sourceLocation", currentRoute->findSourceLocation() }
      });
      paramPass->setType(currentRoute->getRoute()==0 ? Ast::BracketType::ROUND : Ast::BracketType::SQUARE);
      paramPass->setParam(currentRoute->getData());
      state->setData(paramPass);
    })}
  }));

  //// PostfixTildeExp :
  //// "~" keyword {Subject}.
  //// "~(" Expression {Subject} ")".
  //PostfixTildeExp : @single prod (cmd:production[PostfixTildeCmd]=DefaultPostfixTildeCmd) as
  //    lexer.Constant("~") + cmd;
  this->set(S("root.Expression.PostfixTildeExp"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        TokenTerm::create({
          {S("tokenId"), TiInt::create(this->constTokenId)},
          {S("tokenText"), TiStr::create(S("~"))}
        }),
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.postfixTildeCmdGrp")) }})
      })}
    })},
    {S("handler"), this->parsingHandler}
  }));
  //OpenPostfixTildeCmd : @limit[user.parent==PostfixTildeCmd]
  //    prod (expr:production[Expression], args:list[hash[sbj:valid_subject, min:integer,
  //                                                      max:integer, pty:integer]]) as
  //    lexer.Constant("(") + expr + concat (args:a)->( @priority(pty,0) a.sbj*(a.min,a.max) ) +
  //    lexer.Constant(")");
  this->set(S("root.Expression.OpenPostfixTildeCmd"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({}, {
      {S("terms"), List::create({}, {
        TokenTerm::create({
          {S("tokenId"), TiInt::create(this->constTokenId)},
          {S("tokenText"), TiStr::create(S("("))}
        }),
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.openPostfixTildeExpr")) }}),
        ConcatTerm::create({
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
          {S("targetRef"), PARSE_REF(S("stack.p"))}
        }, {
          {S("data"), PARSE_REF(S("args.prms"))},
          {S("terms"), MultiplyTerm::create({
            {S("priority"), PARSE_REF(S("stack.p.pty"))},
            {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
            {S("min"), PARSE_REF(S("stack.p.min"))},
            {S("max"), PARSE_REF(S("stack.p.max"))},
          }, {
            {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("stack.p.prd")) }})}
          })}
        }),
        TokenTerm::create({
          {S("tokenId"), TiInt::create(this->constTokenId)},
          {S("tokenText"), TiStr::create(S(")"))}
        })
      })}
    })},
    {S("vars"), Map::create({}, {
      {S("prms"), List::create({}, {Map::create({}, {
        {S("pty"), std::make_shared<TiInt>(1)},
        {S("min"), 0},
        {S("max"), 0},
        {S("prd"), PARSE_REF(S("module.openPostfixTildeExpr"))}
      })})}
    })},
    {S("handler"), std::make_shared<CustomParsingHandler>(
      [](Parser *parser, ParserState *state)
      {
        auto data = state->getData();
        auto metadata = data.ti_cast_get<Ast::MetaHaving>();
        auto linkOp = Ast::LinkOperator::create({
          { "prodId", metadata->getProdId() },
          { "sourceLocation", metadata->findSourceLocation() }
        });
        linkOp->setType(S("~"));
        linkOp->setSecond(data);
        state->setData(linkOp);
      }
    )}
  }));

  //// Operators :
  // PrefixOp : prod ref heap.ConstantKeywordGroup(heap.TokenData.prefixOpList);
  this->set(S("root.Expression.PrefixOp"), SymbolDefinition::create({}, {
    {S("term"), TokenTerm::create({
      {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
      {S("tokenText"), PARSE_REF(S("root.TokenData.prefixOpList"))}
    })},
    {S("handler"), this->parsingHandler}
  }));
  // PostfixOp : prod ref heap.ConstantKeywordGroup(heap.TokenData.postfixOpList);
  this->set(S("root.Expression.PostfixOp"), SymbolDefinition::create({}, {
    {S("term"), TokenTerm::create({
      {S("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
      {S("tokenText"), PARSE_REF(S("root.TokenData.postfixOpList"))}
    })},
    {S("handler"), this->parsingHandler}
  }));

  this->set(S("root.Expression.paramPassExpr"), TioSharedPtr::null);
  this->set(S("root.Expression.openPostfixTildeExpr"), TioSharedPtr::null);
  this->set(S("root.Expression.subject"), TioSharedPtr::null);
  this->set(S("root.Expression.postfixTildeCmdGrp"), TioSharedPtr::null);
}


void StandardFactory::createSubjectProductionModule()
{
  // Subject : Parameter | Command | Expression | Statement | Set.
  this->set(
    S("root.Subject"),
    Module::create({{S("startRef"), PARSE_REF(S("module.Sbj"))}}).get()
  );

  // Sbj : @limit[user.owner==Subject] prod (
  //   sbj1:production[Parameter||Command||Expression||Statement||Set],
  //   sbj2:production[Parameter||Command||Expression||Statement||Set],
  //   sbj3:production[Parameter||Command||Expression||Statement||Set],
  //   frc2:integer,
  //   frc3:integer,
  //   fltr:filter
  // ) as @filter(fltr) sbj1 ||
  //                    lexer.Constant("(") + sbj2*(frc2,1) + lexer.Constant(")") ||
  //                    lexer.Constant("[") + sbj3*(frc3,1) + lexer.Constant("]");
  this->set(S("root.Subject.Sbj"), SymbolDefinition::create({}, {
    {S("term"), AlternateTerm::create({
      {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)}
    }, {
      {S("data"), PARSE_REF(S("args.fltr"))},
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("args.sbj1")) }}),
        ConcatTerm::create({
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        }, {
          {S("terms"), List::create({}, {
            TokenTerm::create({
              {S("tokenId"), TiInt::create(this->constTokenId)},
              {S("tokenText"), TiStr::create(S("("))}
            }),
            MultiplyTerm::create({
              {S("min"), PARSE_REF(S("args.frc2"))},
              {S("max"), std::make_shared<TiInt>(1)}
            }, {
              {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("args.sbj2")) }})}
            }),
            TokenTerm::create({
              {S("tokenId"), TiInt::create(this->constTokenId)},
              {S("tokenText"), TiStr::create(S(")"))}
            })
          })}
        }),
        ConcatTerm::create({
          {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        }, {
          {S("terms"), List::create({}, {
            TokenTerm::create({
              {S("tokenId"), TiInt::create(this->constTokenId)},
              {S("tokenText"), TiStr::create(S("["))}
            }),
            MultiplyTerm::create({
              {S("min"), PARSE_REF(S("args.frc3"))},
              {S("max"), std::make_shared<TiInt>(1)}
            }, {
              {S("term"), ReferenceTerm::create({{ S("reference"), PARSE_REF(S("args.sbj3")) }})}
            }),
            TokenTerm::create({
              {S("tokenId"), TiInt::create(this->constTokenId)},
              {S("tokenText"), TiStr::create(S("]"))}
            })
          })}
        })
      })}
    })},
    {S("vars"), Map::create({}, {
      {S("sbj1"), PARSE_REF(S("module.TermGroup"))},
      {S("sbj2"), PARSE_REF(S("module.expression"))},
      {S("sbj3"), PARSE_REF(S("module.expression"))},
      {S("frc2"), 0},
      {S("frc3"), 0},
      {S("fltr"), 0}
    })},
    {S("handler"), std::make_shared<SubjectParsingHandler>()}
  }));

  // SbjGroup: (cmdGrp || Parameter || set)
  this->set(S("root.Subject.TermGroup"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("root.ProdGroup"))}
  }, {
    {S("vars"), Map::create({}, {
      {S("prods"), List::create({}, {
        PARSE_REF(S("module.cmdGrp")),
        PARSE_REF(S("module.Parameter")),
        PARSE_REF(S("module.set"))
      })}
    })}
  }));

  //// Parameter: Identifier | Literal.
  // Parameter : @single @prefix(heap.Modifiers.ParameterModifierCmd)
  //     prod (fltr:filter=null, cnsts:keywords=null) as
  //     @filter(fltr) lexer.Identifier || Literal || heap.ConstantKeywordGroup(cnsts);
  this->set(S("root.Subject.Parameter"), SymbolDefinition::create({}, {
    {S("term"), AlternateTerm::create({
      {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)}
    }, {
      {S("data"), PARSE_REF(S("args.fltr"))},
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.Identifier")) }}),
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.Literal")) }})
      })}
    })},
    {S("vars"), Map::create({}, {{ S("fltr"), 0 }} )},
    {S("handler"), this->parsingHandler}
  }));

  // Identifier: lexer.Identifier
  this->set(S("root.Subject.Identifier"), SymbolDefinition::create({}, {
    {S("term"), TokenTerm::create({
      {S("tokenId"), std::make_shared<TiInt>(ID_GENERATOR->getId(S("LexerDefs.Identifier")))}
    })},
    {S("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto current = state->getData().ti_cast_get<Ast::Token>();
      SharedPtr<Ast::Text> newObj = std::make_shared<Ast::Identifier>();
      newObj->setValue(current->getText());
      newObj->setProdId(current->getProdId());
      newObj->setSourceLocation(current->findSourceLocation());
      state->setData(newObj);
    })}
  }));

  // Literal:
  // Literal : @single prod (fltr:filter=null) as
  //     @filter(fltr) lexer.IntLiteral || lexer.FloatLiteral || lexer.CharLiteral || lexer.StringLiteral ||
  //                   lexer.CustomLiteral;
  this->set(S("root.Subject.Literal"), SymbolDefinition::create({}, {
    {S("term"), AlternateTerm::create({
      {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)}
    }, {
      {S("data"), PARSE_REF(S("args.fltr"))},
      {S("terms"), List::create({}, {
        TokenTerm::create({{ S("tokenId"), TiInt::create(ID_GENERATOR->getId(S("LexerDefs.IntLiteral"))) }}),
        TokenTerm::create({{ S("tokenId"), TiInt::create(ID_GENERATOR->getId(S("LexerDefs.FloatLiteral"))) }}),
        TokenTerm::create({{ S("tokenId"), TiInt::create(ID_GENERATOR->getId(S("LexerDefs.CharLiteral"))) }}),
        TokenTerm::create({{ S("tokenId"), TiInt::create(ID_GENERATOR->getId(S("LexerDefs.StringLiteral"))) }})
      })}
    })},
    {S("vars"), Map::create({}, {{ S("fltr"), 0 }} )},
    {S("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto current = state->getData().ti_cast_get<Ast::Token>();
      SharedPtr<Ast::Text> newObj;
      if (current->getId() == ID_GENERATOR->getId(S("LexerDefs.IntLiteral"))) {
        newObj = std::make_shared<Ast::IntegerLiteral>();
      } else if (current->getId() == ID_GENERATOR->getId(S("LexerDefs.FloatLiteral"))) {
        newObj = std::make_shared<Ast::FloatLiteral>();
      } else if (current->getId() == ID_GENERATOR->getId(S("LexerDefs.CharLiteral"))) {
        newObj = std::make_shared<Ast::CharLiteral>();
      } else if (current->getId() == ID_GENERATOR->getId(S("LexerDefs.StringLiteral"))) {
        newObj = std::make_shared<Ast::StringLiteral>();
      }
      newObj->setValue(current->getText());
      newObj->setProdId(current->getProdId());
      newObj->setSourceLocation(current->findSourceLocation());
      state->setData(newObj);
    })}
  }));

  this->set(S("root.Subject.expression"), TioSharedPtr::null);
  this->set(S("root.Subject.set"), TioSharedPtr::null);
  this->set(S("root.Subject.cmdGrp"), TioSharedPtr::null);
}


void StandardFactory::createSetProductionDefinitions()
{
  // Set : "{" Statements.StmtList "}".
  // Set : @limit[child.terms==self,user.parent==self] @prefix(heap.Modifiers.DefaultModifierCmd)
  //     prod (stmt:production[Statement], ovrd:grammar_override) as
  //     lexer.Constant("{") + @override(ovrd) Statements.StmtList(stmt) + lexer.Constant("}");
  this->set(S("root.Set"), SymbolDefinition::create({}, {
    {S("term"), ConcatTerm::create({
      {S("errorSyncPosId"), TiInt(1)}
    }, {
      {S("terms"), List::create({}, {
        TokenTerm::create({
          {S("tokenId"), TiInt::create(this->constTokenId)},
          {S("tokenText"), TiStr::create(S("{"))}
        }),
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("args.stmt")) }}),
        TokenTerm::create({
          {S("tokenId"), TiInt::create(this->constTokenId)},
          {S("tokenText"), TiStr::create(S("}"))}
        })
      })}
    })},
    {S("handler"), this->parsingHandler}
  }));
}


void StandardFactory::createModifierProductionDefinitions()
{
  // Modifiers
  this->set(S("root.Modifier"), Module::create({}));
  // Modifier.Expression
  this->set(S("root.Modifier.Expression"), Module::create({
    { S("startRef"), PARSE_REF(S("module.FunctionalExp")) },
    { S("baseRef"), PARSE_REF(S("root.Expression")) }
  }, {
    { S("paramPassExpr"), PARSE_REF(S("module.owner.Expression")) },
    { S("openPostfixTildeExpr"), PARSE_REF(S("module.owner.Expression")) },
    { S("subject"), PARSE_REF(S("module.owner.Subject")) }
  }));
  this->set(S("root.Modifier.Expression.FunctionalExp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("bmodule.FunctionalExp")) }
  }, {
    {S("vars"), Map::create({}, {
      {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP | TermFlags::ONE_ROUTE_TERM)},
      {S("pty2"), std::make_shared<TiInt>(1)},
      {S("dup"), TiInt::create(1)},
      {S("fltr2"), TiInt::create(2)}
     })}
  }));
  // Modifier.Subject
  this->set(S("root.Modifier.Subject"), Module::create({
    {S("baseRef"), PARSE_REF(S("root.Subject"))}
  }, {
    {S("expression"), PARSE_REF(S("module.owner.Expression"))},
    {S("set"), PARSE_REF(S("root.Set"))},
    {S("cmdGrp"), PARSE_REF(S("module.owner.SubjectCmdGrp"))}
  }));
  // Modifier.CmdGroup
  this->set(S("root.Modifier.CmdGroup"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF("root.ProdGroup")}
  }, {
    {S("vars"), Map::create({}, {
      {S("prods"), List::create()}
    })}
  }));
  // Modifier.SubjectCmdGrp
  this->set(S("root.Modifier.SubjectCmdGrp"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF("root.ProdGroup")}
  }, {
    {S("vars"), Map::create({}, {
      {S("prods"), List::create()}
    })}
  }));
  // Modifier.Phrase
  this->set(S("root.Modifier.Phrase"), SymbolDefinition::create({}, {
    {S("term"), AlternateTerm::create({
      {S("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)}
    }, {
      {S("terms"), List::create({}, {
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.CmdGroup")) }}),
        ReferenceTerm::create({{ S("reference"), PARSE_REF(S("module.Expression")) }})
      })}
    })}
  }));
  // Modifier.LeadingModifier
  this->set(S("root.Modifier.LeadingModifier"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.Phrase")) }
  }, {
    {S("handler"), this->leadingModifierHandler}
  }));
  // Modifier.TrailingModifier
  this->set(S("root.Modifier.TrailingModifier"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("module.Phrase")) }
  }, {
    {S("handler"), this->trailingModifierHandler}
  }));

  // Modifiers parsing dimensions.
  this->set(S("root.LeadingModifierDim"), ParsingDimension::create({
    {S("entryTokenText"), std::make_shared<TiStr>(S("@"))},
    {S("startRef"), PARSE_REF(S("root.Modifier.LeadingModifier"))}
  }));
  this->set(S("root.TrailingModifierDim"), ParsingDimension::create({
    {S("entryTokenText"), std::make_shared<TiStr>(S("@<"))},
    {S("startRef"), PARSE_REF(S("root.Modifier.TrailingModifier"))}
  }));
}


void StandardFactory::createMainProductionModule(Bool exprOnly)
{
  this->set(S("root.Main"), Module::create({}));

  // root.Main.Statements : module inherits root.Statements {
  //   variations = (module.CmdVariation, module.ExpVariation);
  // };
  this->set(S("root.Main.Statements"), Module::create({
    {S("baseRef"), PARSE_REF(S("root.Statements"))}
  }, {
    {S("variations"), List::create({}, {
      PARSE_REF(S("module.CmdVariation")),
      PARSE_REF(S("module.ExpVariation"))
    })},
    {S("cmdGrp"), PARSE_REF(S("module.owner.LeadingCmdGrp"))},
    {S("expression"), PARSE_REF(S("module.owner.Expression"))}
  }));

  // CmdVariation : prule ref Variation(sections=((prd=cmdGrp,min=1,max=1,pty=1)));
  this->set(S("root.Main.Statements.CmdVariation"), SymbolDefinition::create({}, {
   {S("term"), PARSE_REF(S("module.Variation"))},
   {S("vars"), Map::create({}, {
      {S("subjects"), List::create({}, {
         Map::create({}, {
           {S("prd"), PARSE_REF(S("module.cmdGrp"))},
           {S("min"), std::make_shared<TiInt>(1)},
           {S("max"), std::make_shared<TiInt>(1)},
           {S("pty"), std::make_shared<TiInt>(1)}
         })
       })}
    })},
   {S("handler"), this->parsingHandler}
  }));

  // ExpVariation : prule ref Variation(sections=((prd=module.expression,min=1,max=1,pty=1)));
  this->set(S("root.Main.Statements.ExpVariation"), SymbolDefinition::create({}, {
   {S("term"), PARSE_REF(S("module.Variation"))},
   {S("vars"), Map::create({}, {
      {S("subjects"), List::create({}, {
         Map::create({}, {
           {S("prd"), PARSE_REF(S("module.expression"))},
           {S("min"), std::make_shared<TiInt>(1)},
           {S("max"), std::make_shared<TiInt>(1)},
           {S("pty"), std::make_shared<TiInt>(1)}
         })
       })}
    })},
   {S("handler"), this->parsingHandler}
  }));

  if (!exprOnly) {
    this->set(S("root.Main.RootStatements"), Module::create({
      {S("baseRef"), PARSE_REF(S("module.Statements"))}
    }, {}));

    this->set(S("root.Main.RootStatements.StmtList"), SymbolDefinition::create({
      {S("baseRef"), PARSE_REF(S("bmodule.StmtList"))}
    }, {
      {S("handler"), this->rootScopeParsingHandler}
    }));
  }

  // root.Main.Expression : module inherits root.Expression {
  //   paramPassExpr = module.owner.Expression;
  //   openPostfixTildeExpr = module.owner.Expression;
  // };
  this->set(S("root.Main.Expression"), Module::create({
    {S("baseRef"), PARSE_REF(S("root.Expression"))}
  }, {
    {S("paramPassExpr"), PARSE_REF(S("module.owner.Expression"))},
    {S("openPostfixTildeExpr"), PARSE_REF(S("module.owner.Expression"))},
    {S("subject"), PARSE_REF(S("module.owner.Subject"))},
    {S("postfixTildeCmdGrp"), PARSE_REF(S("module.owner.PostfixTildeCmdGrp"))}
  }));

  // root.Main.Subject : module inherits root.Subject {
  //   expression = module.owner.Expression;
  //   set = module.owner.Set;
  //   cmdGrp = module.owner.SubjectCmdGrp;
  // };
  this->set(S("root.Main.Subject"), Module::create({
    {S("baseRef"), PARSE_REF(S("root.Subject"))}
  }, {
    {S("expression"), PARSE_REF(S("module.owner.Expression"))},
    {S("set"), PARSE_REF(S("module.owner.Set"))},
    {S("cmdGrp"), PARSE_REF(S("module.owner.SubjectCmdGrp"))}
  }));

  // root.Main.Set
  this->set(S("root.Main.Set"), SymbolDefinition::create({
    {S("baseRef"), PARSE_REF(S("root.Set"))}
  }, {
    {S("vars"), Map::create({}, {
       {S("stmt"), PARSE_REF(S("module.Statements.StmtList"))}
     })}
  }));

  if (exprOnly) {
    // LeadingCommandGroup
    this->set(S("root.Main.LeadingCmdGrp"), SymbolDefinition::create({
      {S("baseRef"), PARSE_REF(S("root.ProdGroup"))}
    }, {
      {S("vars"), Map::create({}, {{ S("prods"), List::create() }})}
    }));

    // SubjectCommandGroup
    this->set(S("root.Main.SubjectCmdGrp"), SymbolDefinition::create({
      {S("baseRef"), PARSE_REF(S("root.ProdGroup"))}
    }, {
      {S("vars"), Map::create({}, {{ S("prods"), List::create() }})}
    }));

    // TildeCommandGroup
    this->set(S("root.Main.PostfixTildeCmdGrp"), SymbolDefinition::create({
      {S("baseRef"), PARSE_REF(S("root.ProdGroup"))}
    }, {
      {S("vars"), Map::create({}, {{ S("prods"), List::create({}, {
        PARSE_REF(S("module.Expression.OpenPostfixTildeCmd")),
      }) }})}
    }));
  } else {
    // LeadingCommandGroup
    this->set(S("root.Main.LeadingCmdGrp"), SymbolDefinition::create({
      {S("baseRef"), PARSE_REF(S("root.ProdGroup"))}
    }, {
      {S("vars"), Map::create({}, {
        {S("prods"), List::create({}, {
          PARSE_REF(S("module.Do")),
          PARSE_REF(S("module.Import")),
          PARSE_REF(S("module.Def")),
          PARSE_REF(S("module.Use")),
          PARSE_REF(S("module.DumpAst"))
        })}
      })}
    }));

    // SubjectCommandGroup
    this->set(S("root.Main.SubjectCmdGrp"), SymbolDefinition::create({
      {S("baseRef"), PARSE_REF(S("root.ProdGroup"))}
    }, {
      {S("vars"), Map::create({}, {{ S("prods"), List::create({}, {
        PARSE_REF(S("module.Alias"))
      }) }} )}
    }));

    // TildeCommandGroup
    this->set(S("root.Main.PostfixTildeCmdGrp"), SymbolDefinition::create({
      {S("baseRef"), PARSE_REF(S("root.ProdGroup"))}
    }, {
      {S("vars"), Map::create({}, {{ S("prods"), List::create({}, {
        PARSE_REF(S("module.Expression.OpenPostfixTildeCmd")),
      }) }})}
    }));

    //// Do = "do" + Subject
    this->set(S("root.Main.Do"), SymbolDefinition::create({}, {
      {S("term"), PARSE_REF(S("root.Cmd"))},
      {S("vars"), Map::create({}, {
        {S("kwd"), std::make_shared<TiStr>(S("do"))},
        {S("prms"), List::create({}, {
          Map::create({}, {
            {S("prd"), PARSE_REF(S("module.Subject"))},
            {S("min"), std::make_shared<TiInt>(1)},
            {S("max"), std::make_shared<TiInt>(1)},
            {S("pty"), std::make_shared<TiInt>(1)},
            {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
          })
        })}
      })},
      {S("handler"), this->doCommandParsingHandler}
    }));

    //// Import = "import" + Subject
    this->set(S("root.Main.Import"), SymbolDefinition::create({}, {
      {S("term"), PARSE_REF(S("root.Cmd"))},
      {S("vars"), Map::create({}, {
         {S("kwd"), Map::create({}, {{S("import"),0},{S("اشمل"),0}})},
         {S("prms"), List::create({}, {
            Map::create({}, {
              {S("prd"), PARSE_REF(S("module.Subject"))},
              {S("min"), std::make_shared<TiInt>(1)},
              {S("max"), std::make_shared<TiInt>(1)},
              {S("pty"), std::make_shared<TiInt>(1)},
              {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
            })
          })}
       })},
      {S("handler"), this->importHandler.s_cast<TiObject>()}
    }));

    //// Def = "def" + Subject
    this->set(S("root.Main.Def"), SymbolDefinition::create({}, {
      {S("term"), PARSE_REF(S("root.Cmd")) },
      {S("vars"), Map::create({}, {
        {S("kwd"), Map::create({}, { { S("def"), 0 }, { S("عرّف"), 0 }, { S("عرف"), 0 } }) },
        {S("prms"), List::create({}, {
          Map::create({}, {
            {S("prd"), PARSE_REF(S("module.Expression"))},
            {S("min"), std::make_shared<TiInt>(1)},
            {S("max"), std::make_shared<TiInt>(1)},
            {S("pty"), std::make_shared<TiInt>(1)},
            {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
          })
        })}
      })},
      {S("modifierTranslations"), Map::create({}, {
        {S("دمج"), TiStr::create(S("merge"))}
      })},
      {S("handler"), std::make_shared<DefParsingHandler>()}
    }));

    //// use = "use" + Expression
    this->set(S("root.Main.Use"), SymbolDefinition::create({}, {
      {S("term"), PARSE_REF(S("root.Cmd"))},
      {S("vars"), Map::create({}, {
        {S("kwd"), Map::create({}, { { S("use"), 0 }, { S("استخدم"), 0 } })},
        {S("prms"), List::create({}, {
          Map::create({}, {
            {S("prd"), PARSE_REF(S("module.Expression"))},
            {S("min"), std::make_shared<TiInt>(1)},
            {S("max"), std::make_shared<TiInt>(1)},
            {S("pty"), std::make_shared<TiInt>(1)},
            {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
          })
        })}
      })},
      {S("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
        auto metadata = state->getData().ti_cast_get<Data::Ast::MetaHaving>();
        auto currentList = state->getData().ti_cast_get<Containing<TiObject>>();
        auto bridge = Data::Ast::Bridge::create({
          {S("prodId"), metadata->getProdId()},
          {S("sourceLocation"), metadata->findSourceLocation()}
        });
        bridge->setTarget(getSharedPtr(currentList->getElement(1)));
        state->setData(bridge);
      })}
    }).get());

    //// dump = "dump" + Subject
    this->set(S("root.Main.DumpAst"), SymbolDefinition::create({}, {
      { S("term"), PARSE_REF(S("root.Cmd")) },
      {
        S("vars"), Map::create({}, {
          { S("kwd"), Map::create({}, { { S("dump_ast"), 0 }, { S("أدرج_ش_ب_م"), 0 } }) },
          {
            S("prms"), List::create({}, {
              Map::create({}, {
                {S("prd"), PARSE_REF(S("module.Expression"))},
                {S("min"), std::make_shared<TiInt>(1)},
                {S("max"), std::make_shared<TiInt>(1)},
                {S("pty"), std::make_shared<TiInt>(1)},
                {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
              })
            })
          }
        })
      },
      { S("handler"), this->dumpAstParsingHandler }
    }));

    //// Alias = "alias" + Subject
    this->set(S("root.Main.Alias"), SymbolDefinition::create({}, {
      {S("term"), PARSE_REF(S("root.Cmd"))},
      {S("vars"), Map::create({}, {
        {S("kwd"), Map::create({}, { { S("alias"), 0 }, { S("لقب"), 0 } }) },
        {S("prms"), List::create({}, {
          Map::create({}, {
            {S("prd"), PARSE_REF(S("module.Expression"))},
            {S("min"), std::make_shared<TiInt>(1)},
            {S("max"), std::make_shared<TiInt>(1)},
            {S("pty"), std::make_shared<TiInt>(1)},
            {S("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
          })
        })}
      })},
      {S("handler"), std::make_shared<CustomParsingHandler>(
        [](Parser *parser, ParserState *state)
        {
          auto currentList = state->getData().ti_cast_get<Containing<TiObject>>();
          auto metadata = ti_cast<Ast::MetaHaving>(currentList);
          auto alias = Ast::Alias::create({
            { "prodId", metadata->getProdId() },
            { "sourceLocation", metadata->findSourceLocation() }
          });
          alias->setReference(getSharedPtr(currentList->getElement(1)));
          state->setData(alias);
        }
      )}
    }));
  }
}


SharedPtr<SymbolDefinition> StandardFactory::createConstTokenDef(Char const *text)
{
  if (SBSTR(text) == S("@") || SBSTR(text) == S("@<")) {
    return SymbolDefinition::create({
      {S("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
    }, {
      {S("term"), ConstTerm::create({{ S("matchString"), TiWStr(text) }})}
    });
  } else {
    return SymbolDefinition::create({
      {S("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
    }, {
      {S("term"), ConstTerm::create({{ S("matchString"), TiWStr(text) }})},
      {S("handler"), this->constTokenHandler}
    });
  }
}

} // namespace
