/**
 * @file Core/Data/Grammar/StandardFactory.cpp
 * Contains the implementation of class Core::Data::Grammar::StandardFactory.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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
  ListContaining<TiObject> *rootScope, Main::RootManager *root, Bool exprOnly
) {
  this->setRootScope(rootScope);

  this->constTokenPrefix = STR("LexerDefs");
  this->constTokenId = ID_GENERATOR->getId(STR("CONSTTOKEN"));

  // Instantiate handlers.
  this->stringLiteralHandler = std::make_shared<StringLiteralTokenizingHandler>();
  this->constTokenHandler = std::make_shared<ConstTokenizingHandler>(this->constTokenId);
  this->parsingHandler = std::make_shared<GenericParsingHandler>();
  this->importHandler = std::make_shared<ImportParsingHandler>(root);
  this->dumpAstParsingHandler = std::make_shared<DumpAstParsingHandler>(root);
  this->leadingModifierHandler = std::make_shared<ModifierParsingHandler>(true);
  this->trailingModifierHandler = std::make_shared<ModifierParsingHandler>(false);
  this->doCommandParsingHandler = std::make_shared<GenericCommandParsingHandler>(STR("do"));

  // Create lexer definitions.
  this->set(STR("root.LexerDefs"), Module::create({}));
  this->createCharGroupDefinitions();
  this->createTokenDefinitions();

  // Create parser definitions.
  this->createProductionDefinitions(exprOnly);

  // Create error sync block pairs.
  this->set(STR("root.ErrorSyncBlockPairs"), List::create({}, {
    TokenTerm::create({
      {STR("tokenId"), TiInt::create(this->constTokenId)},
      {STR("tokenText"), TiStr::create(STR("("))}
    }),
    TokenTerm::create({
      {STR("tokenId"), TiInt::create(this->constTokenId)},
      {STR("tokenText"), TiStr::create(STR(")"))}
    }),
    TokenTerm::create({
      {STR("tokenId"), TiInt::create(this->constTokenId)},
      {STR("tokenText"), TiStr::create(STR("["))}
    }),
    TokenTerm::create({
      {STR("tokenId"), TiInt::create(this->constTokenId)},
      {STR("tokenText"), TiStr::create(STR("]"))}
    }),
    TokenTerm::create({
      {STR("tokenId"), TiInt::create(this->constTokenId)},
      {STR("tokenText"), TiStr::create(STR("{"))}
    }),
    TokenTerm::create({
      {STR("tokenId"), TiInt::create(this->constTokenId)},
      {STR("tokenText"), TiStr::create(STR("}"))}
    })
  }));

  // Set start production and lexer module.
  Module *rootModule = this->context.getRoot();
  rootModule->setStartRef(PARSE_REF(STR("module.Program")));
  rootModule->setLexerModuleRef(PARSE_REF(STR("root.LexerDefs")));
  rootModule->setErrorSyncBlockPairsRef(PARSE_REF(STR("root.ErrorSyncBlockPairs")));

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
  this->set(STR("root.LexerDefs.BinDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(STR("0"), STR("1"))
  ));

  // OctDigit : char '0'..'7';
  this->set(STR("root.LexerDefs.OctDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(STR("0"), STR("7"))
  ));

  // DecDigit : char '0'..'9';
  this->set(STR("root.LexerDefs.DecDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(STR("0"), STR("9"))
  ));

  // HexDigit : char '0'..'9', 'a'..'f', 'A'..'F';
  this->set(STR("root.LexerDefs.HexDigit"), CharGroupDefinition::create(
    UnionCharGroupUnit::create({
      SequenceCharGroupUnit::create(STR("0"), STR("9")),
      SequenceCharGroupUnit::create(STR("a"), STR("f")),
      SequenceCharGroupUnit::create(STR("A"), STR("F"))
    })
  ));

  // Letter : char 'a'..'z', 'A'..'Z', '_';
  SharedPtr<CharGroupUnit> letterCharGroup = UnionCharGroupUnit::create({
    SequenceCharGroupUnit::create(STR("a"), STR("z")),
    SequenceCharGroupUnit::create(STR("A"), STR("Z")),
    SequenceCharGroupUnit::create(STR("_"), STR("_")),
    SequenceCharGroupUnit::create(STR("ؠ"), STR("ٟ")),
    SequenceCharGroupUnit::create(STR("ٮ"), STR("ۜ"))
  });
  this->set(STR("root.LexerDefs.Letter"), CharGroupDefinition::create(letterCharGroup));

  // AnyCharNoEs : char !('\\');
  this->set(STR("root.LexerDefs.AnyCharNoEs"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      SequenceCharGroupUnit::create(STR("\\"), STR("\\"))
    )
  ));

  // AnyCharNoEsOrSingleQuote : char !("\\'");
  this->set(STR("root.LexerDefs.AnyCharNoEsOrSingleQuote"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      RandomCharGroupUnit::create(STR("\\'"))
    )
  ));

  // AnyCharNoEsOrDoubleQuote : char !("\\\"");
  this->set(STR("root.LexerDefs.AnyCharNoEsOrDoubleQuote"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      RandomCharGroupUnit::create(STR("\\\""))
    )
  ));

  // AnyCharNoReturn = !('\\');
  this->set(STR("root.LexerDefs.AnyCharNoReturn"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      SequenceCharGroupUnit::create(STR("\n"), STR("\n"))
    )
  ));

  // Spacing : char " \n\r\t";
  this->set(STR("root.LexerDefs.Spacing"), CharGroupDefinition::create(
    RandomCharGroupUnit::create(STR(" \r\n\t"))
  ));
}


/**
 * Create a TokenDefinitionList and add all the required definitions for
 * the Core's grammar.
 */
void StandardFactory::createTokenDefinitions()
{
  // Identifier : trule as { Letter + (Letter || DecDigit)*(0,endless) };
  this->set(STR("root.LexerDefs.Identifier"), SymbolDefinition::create({
    {STR("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
  }, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
         CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.Letter")) }}),
         MultiplyTerm::create({}, {
           {STR("term"), AlternateTerm::create({}, {
              {STR("terms"), List::create({}, {
                 CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.Letter")) }}),
                 CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.DecDigit")) }})
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
  this->set(STR("root.LexerDefs.IntLiteral"), SymbolDefinition::create({
    {STR("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
  }, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        AlternateTerm::create({}, {
          {STR("terms"), List::create({}, {
            ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.DecIntLiteral")) }}),
            ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.BinIntLiteral")) }}),
            ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.OctIntLiteral")) }}),
            ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.HexIntLiteral")) }})
          })}
        }),
        MultiplyTerm::create({
          {STR("max"), std::make_shared<TiInt>(1)}
          }, {
          {STR("term"), AlternateTerm::create({}, {
            {STR("terms"), List::create({}, {
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("u")) }}),
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("U")) }})
            })}
          })}
        }),
        MultiplyTerm::create({
          {STR("max"), std::make_shared<TiInt>(1)}
          }, {
          {STR("term"), ConcatTerm::create({}, {
            {STR("terms"), List::create({}, {
              AlternateTerm::create({}, {
                {STR("terms"), List::create({}, {
                  ConstTerm::create({{ STR("matchString"), TiWStr(STR("i")) }}),
                  ConstTerm::create({{ STR("matchString"), TiWStr(STR("I")) }})
                })}
              }),
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.DecIntLiteral")) }})
            })}
          })}
        })
      })}
    })}
  }));

  // @inner DecIntLiteral : trule as { DecDigit*(1,endless) };
  this->set(STR("root.LexerDefs.DecIntLiteral"), SymbolDefinition::create({}, {
    {STR("term"), MultiplyTerm::create({
      {STR("min"), std::make_shared<TiInt>(1)}
    }, {
      {STR("term"), CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.DecDigit")) }})}
    })}
  }));

  // @inner BinIntLiteral : trule as { ("0b" || "0B") + BinDigit*(1,endless) };
  this->set(STR("root.LexerDefs.BinIntLiteral"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        AlternateTerm::create({}, {
          {STR("terms"), List::create({}, {
            ConstTerm::create({{ STR("matchString"), TiWStr(STR("0b")) }}),
            ConstTerm::create({{ STR("matchString"), TiWStr(STR("0B")) }})
          })}
        }),
        MultiplyTerm::create({
          {STR("min"), std::make_shared<TiInt>(1)}
        }, {
          {STR("term"), CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.BinDigit")) }})}
        })
      })}
    })}
  }));

  // @inner OctIntLiteral : trule as { ("0o" || "0O") + OctDigit*(1,endless) };
  this->set(STR("root.LexerDefs.OctIntLiteral"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        AlternateTerm::create({}, {
          {STR("terms"), List::create({}, {
            ConstTerm::create({{ STR("matchString"), TiWStr(STR("0o")) }}),
            ConstTerm::create({{ STR("matchString"), TiWStr(STR("0O")) }})
          })}
        }),
        MultiplyTerm::create({
          {STR("min"), std::make_shared<TiInt>(1)}
        }, {
          {STR("term"), CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.OctDigit")) }})}
        })
      })}
    })}
  }));

  // @inner HexIntLiteral : trule as { ("0h" || "0H") + HexDigit*(1,endless) };
  this->set(STR("root.LexerDefs.HexIntLiteral"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        AlternateTerm::create({}, {
          {STR("terms"), List::create({}, {
            ConstTerm::create({{ STR("matchString"), TiWStr(STR("0h")) }}),
            ConstTerm::create({{ STR("matchString"), TiWStr(STR("0H")) }})
          })}
        }),
        MultiplyTerm::create({
          {STR("min"), std::make_shared<TiInt>(1)}
        }, {
          {STR("term"), CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }})}
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
  this->set(STR("root.LexerDefs.FloatLiteral"), SymbolDefinition::create({
    {STR("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
  }, {
    {STR("term"), AlternateTerm::create({}, {
      {STR("terms"), List::create({}, {
        ConcatTerm::create({}, {
          {STR("terms"), List::create({}, {
            MultiplyTerm::create({
              {STR("min"), std::make_shared<TiInt>(1)}
            }, {
              {STR("term"), CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.DecDigit")) }})}
            }),
            ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.FloatPostfix")) }})
          })}
        }),
        ConcatTerm::create({}, {
          {STR("terms"), List::create({}, {
            MultiplyTerm::create({
              {STR("min"), std::make_shared<TiInt>(1)}
            }, {
              {STR("term"), CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.DecDigit")) }})}
            }),
            ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.FloatExponent")) }}),
            MultiplyTerm::create({
              {STR("max"), std::make_shared<TiInt>(1)}
            }, {
              {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.FloatPostfix")) }})}
            })
          })}
        }),
        ConcatTerm::create({}, {
          {STR("terms"), List::create({}, {
            MultiplyTerm::create({}, {
              {STR("term"), CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.DecDigit")) }})}
            }),
            ConstTerm::create({{ STR("matchString"), TiWStr(STR(".")) }}),
            MultiplyTerm::create({
              {STR("min"), std::make_shared<TiInt>(1)}
            }, {
              {STR("term"), CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.DecDigit")) }})}
            }),
            MultiplyTerm::create({
              {STR("max"), std::make_shared<TiInt>(1)}
            }, {
              {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.FloatExponent")) }})}
            }),
            MultiplyTerm::create({
              {STR("max"), std::make_shared<TiInt>(1)}
            }, {
              {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.FloatPostfix")) }})}
            })
          })}
        })
      })}
    })}
  }));

  // @inner FloatExponent : trule as { ("e" || "E") + ("+" || "-")*(0,1) + DecDigit*(1,endless) };
  this->set(STR("root.LexerDefs.FloatExponent"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        AlternateTerm::create({}, {
          {STR("terms"), List::create({}, {
            ConstTerm::create({{ STR("matchString"), TiWStr(STR("e")) }}),
            ConstTerm::create({{ STR("matchString"), TiWStr(STR("E")) }})
          })}
        }),
        MultiplyTerm::create({
          {STR("max"), std::make_shared<TiInt>(1)}
        }, {
          {STR("term"), AlternateTerm::create({}, {
            {STR("terms"), List::create({}, {
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("+")) }}),
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("-")) }})
            })}
          })}
        }),
        MultiplyTerm::create({
          {STR("min"), std::make_shared<TiInt>(1)}
        }, {
          {STR("term"), CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.DecDigit")) }})}
        })
      })}
    })}
  }));

  // @inner FloatPostfix : trule as { ("f" || "F") + DecDigit*(0,endless) };
  this->set(STR("root.LexerDefs.FloatPostfix"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
         AlternateTerm::create({}, {
           {STR("terms"), List::create({}, {
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("f")) }}),
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("F")) }})
           })}
         }),
         MultiplyTerm::create({}, {
           {STR("term"), CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.DecDigit")) }})}
         })
       })}
    })}
  }));

  // CharLiteral : trule as { "'" + EsCharWithDoubleQuote + "'" + CharCodePostfix*(0,1) };
  // TODO: Add the char_code_postfix part
  this->set(STR("root.LexerDefs.CharLiteral"), SymbolDefinition::create({
    {STR("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
  }, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
         ConstTerm::create({{ STR("matchString"), TiWStr(STR("'")) }}),
         ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.EsCharWithDoubleQuote")) }}),
         ConstTerm::create({{ STR("matchString"), TiWStr(STR("'")) }})
       })}
    })}
  }));

  // StringLiteral : trule as {
  //   StringLiteralPart + (Spacing*(0,endless) + StringLiteralPart)*(0,endless) +
  //   CharCodePostfix*(0,1)
  // };
  // TODO: Add the CharCodePostfix part
  this->set(STR("root.LexerDefs.StringLiteral"), SymbolDefinition::create({
    {STR("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
  }, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.StringLiteralPart")) }}),
        MultiplyTerm::create({}, {
          {STR("term"), ConcatTerm::create({}, {
            {STR("terms"), List::create({}, {
              MultiplyTerm::create({}, {
                {STR("term"), CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.Spacing")) }})}
              }),
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.StringLiteralPart")) }})
            })}
          })}
        })
      })}
    })},
    {STR("handler"), this->stringLiteralHandler.s_cast<TiObject>()}
  }));

  // @inner StringLiteralPart : trule as { "\"" + EsCharWithSingleQuote*(0,endless) + "\"" };
  this->set(STR("root.LexerDefs.StringLiteralPart"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        ConstTerm::create({{ STR("matchString"), TiWStr(STR("\"")) }}),
        MultiplyTerm::create({}, {
          {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.EsCharWithSingleQuote")) }})}
        }),
        ConstTerm::create({{ STR("matchString"), TiWStr(STR("\"")) }})
      })}
    })}
  }));

  // @inner EsCharWithSingleQuote : trule as {
  //   AnyCharNoEsOrDoubleQuote || EsCodeSequence ||
  //   alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the heap.escape_sequences part
  this->set(STR("root.LexerDefs.EsCharWithSingleQuote"), SymbolDefinition::create({}, {
    {STR("term"), AlternateTerm::create({}, {
      {STR("terms"), List::create({}, {
         CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.AnyCharNoEsOrDoubleQuote")) }}),
         ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.EsSequence")) }})
       })}
    })}
  }));

  // @inner EsCharWithDoubleQuote : trule as {
  //   AnyCharNoEsOrSingleQuote || EsCodeSequence ||
  //   alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the root.TokenData.escapeSequences part
  this->set(STR("root.LexerDefs.EsCharWithDoubleQuote"), SymbolDefinition::create({}, {
    {STR("term"), AlternateTerm::create({}, {
      {STR("terms"), List::create({}, {
         CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.AnyCharNoEsOrSingleQuote")) }}),
         ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.EsSequence")) }})
       })}
    })}
  }));

  // @inner EsCharWithQuotes : trule as {
  //   AnyCharNoEs || EsCodeSequence || alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the heap.escape_sequences part
  this->set(STR("root.LexerDefs.EsCharWithQuotes"), SymbolDefinition::create({}, {
    {STR("term"), AlternateTerm::create({}, {
      {STR("terms"), List::create({}, {
         CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.AnyCharNoEs")) }}),
         ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.EsSequence")) }})
       })}
    })}
  }));

  // @inner EsSequence : trule as {
  //   '\\' + ('c' + HexDigit*(2,2) ||
  //           'u' + HexDigit*(4,4) ||
  //           'w' + HexDigit*(8,8) ||
  //           'n' || 't' || 'r' || '"' || '\'' || '\\')
  // };
  this->set(STR("root.LexerDefs.EsSequence"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
         ConstTerm::create({{ STR("matchString"), TiWStr(STR("\\")) }}),
         AlternateTerm::create({}, {
           {STR("terms"), List::create({}, {
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("\\")) }}),
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("\"")) }}),
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("'")) }}),
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("n")) }}),
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("t")) }}),
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("r")) }}),
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("ج")) }}),
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("ت")) }}),
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("ر")) }}),
              ConcatTerm::create({}, {
                {STR("terms"), List::create({}, {
                   ConstTerm::create({{ STR("matchString"), TiWStr(STR("c")) }}),
                   CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }}),
                   CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }})
                })}
              }),
              ConcatTerm::create({}, {
                {STR("terms"), List::create({}, {
                   ConstTerm::create({{ STR("matchString"), TiWStr(STR("u")) }}),
                   CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }}),
                   CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }}),
                   CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }}),
                   CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }})
                })}
              }),
              ConcatTerm::create({}, {
                {STR("terms"), List::create({}, {
                   ConstTerm::create({{ STR("matchString"), TiWStr(STR("w")) }}),
                   CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }}),
                   CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }}),
                   CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }}),
                   CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }}),
                   CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }}),
                   CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }}),
                   CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }}),
                   CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.HexDigit")) }})
                })}
              })
           })}
         })
       })}
    })}
  }));

  //// IGNORED TOKENS

  // ignore { Spacing*(1,endless) };
  this->set(STR("root.LexerDefs.IgnoredSpaces"), SymbolDefinition::create({
    {STR("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN|SymbolFlags::IGNORED_TOKEN)}
  }, {
    {STR("term"), MultiplyTerm::create({
      {STR("min"), std::make_shared<TiInt>(1)}
    }, {
      {STR("term"), CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.Spacing")) }})}
    })}
  }));

  // @minimum ignore { "//" + any*(0,endless) + "\n" }
  // For now this is implemented as:
  // ignore { "//" + AnyCharNoReturn*(0,endless) + "\n" }
  // because the lexer still doesn't support the @minimum modifier.
  this->set(STR("root.LexerDefs.LineComment"), SymbolDefinition::create({
    {STR("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN|SymbolFlags::IGNORED_TOKEN)}
  }, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
         AlternateTerm::create({}, {
           {STR("terms"), List::create({}, {
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("//")) }}),
              ConstTerm::create({{ STR("matchString"), TiWStr(STR("#")) }}),
           })}
         }),
         MultiplyTerm::create({}, {
           {STR("term"), CharGroupTerm::create({{ STR("charGroupReference"), PARSE_REF(STR("module.AnyCharNoReturn")) }})}
         }),
         ConstTerm::create({{ STR("matchString"), TiWStr(STR("\n")) }})
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
  //// TokenData module.

  this->set(STR("root.TokenData"), Module::create({}));
  // assignmentOpList : keywords = ("=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "$=", "<<=", ">>=");
  this->set(STR("root.TokenData.assignmentOpList"), Map::create(true, {}, {
   {STR("="), 0},
   {STR("+="), 0},
   {STR("-="), 0},
   {STR("*="), 0},
   {STR("/="), 0},
   {STR("÷="), std::make_shared<TiStr>(("/="))},
   {STR("%="), 0},
   {STR("&="), 0},
   {STR("|="), 0},
   {STR("$="), 0},
   {STR("<<="), 0},
   {STR(">>="), 0}
  }));
  // comparisonOpList : keywords = ("==", "!=", "<", ">", "<=", ">=");
  this->set(STR("root.TokenData.comparisonOpList"), Map::create(true, {}, {
   {STR("=="), 0},
   {STR("!="), 0},
   {STR("<"), 0},
   {STR(">"), 0},
   {STR("<="), 0},
   {STR(">="), 0}
  }));
  // addOpList : keywords = ("+", "-");
  this->set(STR("root.TokenData.addOpList"), Map::create(true, {}, {
    {STR("+"), 0},
    {STR("-"), 0}
  }));
  // mulOpList : keywords = ("*", "/", "%");
  this->set(STR("root.TokenData.mulOpList"), Map::create(true, {}, {
    {STR("*"), 0},
    {STR("/"), 0},
    {STR("÷"), std::make_shared<TiStr>(STR("/"))},
    {STR("%"), 0}
  }));
  // bitwiseOpList : keywords = ("|", "$", "&", "<<", ">>");
  this->set(STR("root.TokenData.bitwiseOpList"), Map::create(true, {}, {
    {STR("|"), 0},
    {STR("$"), 0},
    {STR("&"), 0},
    {STR("<<"), 0},
    {STR(">>"), 0}
  }));
  // logOpList : keywords = ("||", "$$", "&&", "or", "nor", "xor", "xnor", "and", "nand");
  this->set(STR("root.TokenData.logOpList"), Map::create(true, {}, {
    {STR("||"), 0},
    {STR("$$"), 0},
    {STR("&&"), 0},
    {STR("or"), 0},
    {STR("أو"), std::make_shared<TiStr>(STR("or"))},
    {STR("nor"), 0},
    {STR("وليس"), std::make_shared<TiStr>(STR("nor"))},
    {STR("xor"), 0},
    {STR("أو_حصرا"), std::make_shared<TiStr>(STR("xor"))},
    {STR("xnor"), 0},
    {STR("وليس_حصرا"), std::make_shared<TiStr>(STR("xnor"))},
    {STR("and"), 0},
    {STR("و"), std::make_shared<TiStr>(STR("and"))},
    {STR("nand"), 0},
    {STR("أو_ليس"), std::make_shared<TiStr>(STR("nand"))}
  }));
  // prefixOpList : keywords = ("++", "--", "+", "-", "!", "!!", "not");
  this->set(STR("root.TokenData.prefixOpList"), Map::create(true, {}, {
    {STR("++"), 0},
    {STR("--"), 0},
    {STR("+"), 0},
    {STR("-"), 0},
    {STR("!"), 0},
    {STR("!!"), 0},
    {STR("not"), 0},
    {STR("ليس"), std::make_shared<TiStr>(STR("not"))},
    {STR("..."), 0}
  }));
  // postfixOpList : keywords = ("++", "--");
  this->set(STR("root.TokenData.postfixOpList"), Map::create(true, {}, {
    {STR("++"), 0},
    {STR("--"), 0}
  }));
  // linkOpList : keywords = ("->", ".", ".>", "<.");
  this->set(STR("root.TokenData.linkOpList"), Map::create(true, {}, {
   {STR("->"), 0},
   {STR("."), 0},
   {STR(".>"), 0},
   {STR("<."), 0}
  }));
  // lowLinkOpList : keywords = ("=>", "..", "..>", "<..");
  this->set(STR("root.TokenData.lowLinkOpList"), Map::create(true, {}, {
   {STR("=>"), 0},
   {STR(".."), 0},
   {STR("..>"), 0},
   {STR("<.>"), 0}
  }));
  // lowerLinkOpList : keywords = (":", ":>", "<:");
  this->set(STR("root.TokenData.lowerLinkOpList"), Map::create(true, {}, {
   {STR(":"), 0},
   {STR(":>"), 0},
   {STR("<:"), 0}
  }));
  // lowestLinkOpList : keywords = ("::", ::>", "<::", "in");
  this->set(STR("root.TokenData.lowestLinkOpList"), Map::create(true, {}, {
    {STR("::"), 0},
    {STR("::>"), 0},
    {STR("<::"), 0},
    {STR("in"), 0},
    {STR("في"), std::make_shared<TiStr>(STR("in"))}
  }));

  this->generateConstTokensForStrings(this->get(STR("root.TokenData")));


  //// Main

  //// Program : StatementList.
  // Program : prod as StatementList(DefaultMain.DefaultStatement);
  if (exprOnly) {
    this->set(STR("root.Program"), SymbolDefinition::create({}, {
      {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("root.Main.Statement")) }})},
      {STR("handler"), this->parsingHandler}
    }));
  } else {
    this->set(STR("root.Program"), SymbolDefinition::create({}, {
      {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("root.Main.StatementList")) }})},
      {STR("handler"), this->parsingHandler}
    }));
  }

  this->set(STR("root.Main"), Module::create({}));

  //// StatementList : Statement { ";" Statement }.
  // StatementList : prod (stmt:production[Statement]) as
  //     stmt*(0, 1) + (lexer.Constant(";") + stmt*(0, 1))*(0, endless);
  this->set(STR("root.Main.StatementList"), SymbolDefinition::create({
    {STR("flags"), TiInt::create(ParsingFlags::ENFORCE_PROD_OBJ)}
  }, {
    {STR("term"), ConcatTerm::create({
      {STR("errorSyncPosId"), TiInt(1)},
    }, {
      {STR("terms"), List::create({}, {
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
          {STR("max"), std::make_shared<TiInt>(1)}
        }, {
          {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("args.stmt")) }})}
        }),
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP|TermFlags::ERROR_SYNC_TERM)}
        }, {
          {STR("term"), ConcatTerm::create({}, {
            {STR("terms"), List::create({}, {
              TokenTerm::create({
                {STR("flags"), TiInt::create(ParsingFlags::ENFORCE_TOKEN_OMIT)},
                {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {STR("tokenText"), Map::create(false, {}, {{STR(";"),0},{STR("؛"),0}})}
              }),
              MultiplyTerm::create({
                {STR("priority"), std::make_shared<TiInt>(1)},
                {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
                {STR("max"), std::make_shared<TiInt>(1)}
              }, {
                {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("args.stmt")) }})}
              })
            })}
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {
      {STR("stmt"), PARSE_REF(STR("module.Statement"))}
    })},
    {STR("handler"), ScopeParsingHandler<Data::Ast::Scope>::create(1)}
  }));

  //// Statement : (Phrase | Phrase | ...).
  // Statement : @limit[user.parent==self,child.terms=self] prule
  //   prefix self.id, DefaultModifier
  //   as (phrases:list[prule[Phrase]]=(ExpPhrase, CmdPhrase)=>{
  //     alternate (phrases:phrase)->( phrase )
  //   };
  this->set(STR("root.Main.Statement"), SymbolDefinition::create({}, {
    {STR("term"), AlternateTerm::create({
      {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)},
      {STR("targetRef"), PARSE_REF(STR("stack.phrase"))}
    }, {
      {STR("data"), PARSE_REF(STR("args.phrases"))},
      {STR("terms"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("stack.phrase")) }})}
    })},
    {STR("vars"), Map::create(false, {}, {
      {STR("phrases"), List::create({}, {
        PARSE_REF(STR("module.CmdPhrase")),
        PARSE_REF(STR("module.ExpPhrase"))
      })}
    })},
    {STR("handler"), this->parsingHandler}
  }));

  //// Phrase : { Subject }.
  //  Phrase : prule as (sections:list[map[prd:valid_subject, min:integer, max:integer, pty:integer]])=>{
  //    concat (sections:s)->( @priority(s.pty,0) s.prd*(s.min,s.max) )
  //  };
  this->set(STR("root.Main.Phrase"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({
      {STR("errorSyncPosId"), TiInt(1000)},
      {STR("targetRef"), PARSE_REF(STR("stack.subject"))}
    }, {
      {STR("data"), PARSE_REF(STR("args.subjects"))},
      {STR("terms"), MultiplyTerm::create({
        {STR("priority"), PARSE_REF(STR("stack.subject.pty"))},
        {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
        {STR("min"), PARSE_REF(STR("stack.subject.min"))},
        {STR("max"), PARSE_REF(STR("stack.subject.max"))}
      }, {
        {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("stack.subject.prd")) }})}
      })}
    })},
    {STR("handler"), this->parsingHandler}
  }));

  // CmdPhrase : prule ref Phrase(sections=((prd=LeadingCmdGroup,min=1,max=1,pty=1)));
  this->set(STR("root.Main.CmdPhrase"), SymbolDefinition::create({}, {
   {STR("term"), PARSE_REF(STR("module.Phrase"))},
   {STR("vars"), Map::create(false, {}, {
      {STR("subjects"), List::create({}, {
         Map::create(false, {}, {
           {STR("prd"), PARSE_REF(STR("module.LeadingCmdGrp"))},
           {STR("min"), std::make_shared<TiInt>(1)},
           {STR("max"), std::make_shared<TiInt>(1)},
           {STR("pty"), std::make_shared<TiInt>(1)}
         })
       })}
    })},
   {STR("handler"), this->parsingHandler}
  }));

  // ExpPhrase : prule ref Phrase(sections=((prd=root.Expression,min=1,max=1,pty=1)));
  this->set(STR("root.Main.ExpPhrase"), SymbolDefinition::create({}, {
   {STR("term"), PARSE_REF(STR("module.Phrase"))},
   {STR("vars"), Map::create(false, {}, {
      {STR("subjects"), List::create({}, {
         Map::create(false, {}, {
           {STR("prd"), PARSE_REF(STR("root.Expression"))},
           {STR("min"), std::make_shared<TiInt>(1)},
           {STR("max"), std::make_shared<TiInt>(1)},
           {STR("pty"), std::make_shared<TiInt>(1)}
         })
       })}
    })},
   {STR("handler"), this->parsingHandler}
  }));

  if (exprOnly) {
    // LeadingCommandGroup
    this->set(STR("root.Main.LeadingCmdGrp"), SymbolDefinition::create({}, {
      {STR("term"), AlternateTerm::create({
        {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)},
        {STR("targetRef"), PARSE_REF(STR("stack.cmd"))}
      }, {
        {STR("data"), PARSE_REF(STR("args.cmds"))},
        {STR("terms"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("stack.cmd")) }})}
      })},
      {STR("vars"), Map::create(false, {}, {
        {STR("cmds"), List::create()}
      })},
      {STR("handler"), this->parsingHandler}
    }));
  } else {
    // LeadingCommandGroup
    this->set(STR("root.Main.LeadingCmdGrp"), SymbolDefinition::create({}, {
      {STR("term"), AlternateTerm::create({
        {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)},
        {STR("targetRef"), PARSE_REF(STR("stack.cmd"))}
      }, {
        {STR("data"), PARSE_REF(STR("args.cmds"))},
        {STR("terms"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("stack.cmd")) }})}
      })},
      {STR("vars"), Map::create(false, {}, {
        {STR("cmds"), List::create({}, {
          PARSE_REF(STR("module.Do")),
          PARSE_REF(STR("module.Import")),
          PARSE_REF(STR("module.Def")),
          PARSE_REF(STR("module.DumpAst"))
        })}
      })},
      {STR("handler"), this->parsingHandler}
    }));

    //// Do = "do" + Subject
    this->set(STR("root.Main.Do"), SymbolDefinition::create({}, {
      {STR("term"), PARSE_REF(STR("root.Cmd"))},
      {STR("vars"), Map::create(false, {}, {
        {STR("kwd"), std::make_shared<TiStr>(STR("do"))},
        {STR("prms"), List::create({}, {
          Map::create(false, {}, {
            {STR("prd"), PARSE_REF(STR("root.Subject"))},
            {STR("min"), std::make_shared<TiInt>(1)},
            {STR("max"), std::make_shared<TiInt>(1)},
            {STR("pty"), std::make_shared<TiInt>(1)},
            {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
          })
        })}
      })},
      {STR("handler"), this->doCommandParsingHandler}
    }));

    //// Import = "import" + Subject
    this->set(STR("root.Main.Import"), SymbolDefinition::create({}, {
      {STR("term"), PARSE_REF(STR("root.Cmd"))},
      {STR("vars"), Map::create(false, {}, {
         {STR("kwd"), Map::create(false, {}, {{STR("import"),0},{STR("اشمل"),0}})},
         {STR("prms"), List::create({}, {
            Map::create(false, {}, {
              {STR("prd"), PARSE_REF(STR("root.Subject"))},
              {STR("min"), std::make_shared<TiInt>(1)},
              {STR("max"), std::make_shared<TiInt>(1)},
              {STR("pty"), std::make_shared<TiInt>(1)},
              {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
            })
          })}
       })},
      {STR("handler"), this->importHandler.s_cast<TiObject>()}
    }));

    //// Def = "def" + Subject
    this->set(STR("root.Main.Def"), SymbolDefinition::create({}, {
      { STR("term"), PARSE_REF(STR("root.Cmd")) },
      {
        STR("vars"), Map::create(false, {}, {
          { STR("kwd"), Map::create(false, {}, { { STR("def"), 0 }, { STR("عرّف"), 0 } }) },
          {
            STR("prms"), List::create({}, {
              Map::create(false, {}, {
                {STR("prd"), PARSE_REF(STR("root.Expression"))},
                {STR("min"), std::make_shared<TiInt>(1)},
                {STR("max"), std::make_shared<TiInt>(1)},
                {STR("pty"), std::make_shared<TiInt>(1)},
                {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
              })
            })
          }
        })
      },
      {STR("handler"), std::make_shared<DefParsingHandler>()}
    }));

    //// dump = "dump" + Subject
    this->set(STR("root.Main.DumpAst"), SymbolDefinition::create({}, {
      { STR("term"), PARSE_REF(STR("root.Cmd")) },
      {
        STR("vars"), Map::create(false, {}, {
          { STR("kwd"), Map::create(false, {}, { { STR("dump_ast"), 0 }, { STR("أدرج_ش_ب_م"), 0 } }) },
          {
            STR("prms"), List::create({}, {
              Map::create(false, {}, {
                {STR("prd"), PARSE_REF(STR("root.Expression"))},
                {STR("min"), std::make_shared<TiInt>(1)},
                {STR("max"), std::make_shared<TiInt>(1)},
                {STR("pty"), std::make_shared<TiInt>(1)},
                {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
              })
            })
          }
        })
      },
      { STR("handler"), this->dumpAstParsingHandler }
    }));
  }

  //// Cmd : keyword {Subject}.
  // Cmd : @limit[user.parent==root.Command] prule
  //   as (kwd:keywords, args:list[map[prd:valid_subject, min:integer, max:integer, pty:integer]])=>{
  //     root.KeywordGroup(kwd) + concat (args:a)->( @priority(a.pty,0) a.prd*(a.min,a.max) )
  //   };
  this->set(STR("root.Cmd"), SymbolDefinition::create({
    {STR("flags"), TiInt::create(ParsingFlags::ENFORCE_PROD_OBJ)}
  }, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        TokenTerm::create({
          {STR("tokenId"), std::make_shared<TiInt>(ID_GENERATOR->getId(STR("LexerDefs.Identifier")))},
          {STR("tokenText"), PARSE_REF(STR("args.kwd"))}
        }),
        ConcatTerm::create({
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
          {STR("targetRef"), PARSE_REF(STR("stack.p"))}
        }, {
          {STR("data"), PARSE_REF(STR("args.prms"))},
          {STR("terms"), MultiplyTerm::create({
            {STR("priority"), PARSE_REF(STR("stack.p.pty"))},
            {STR("flags"), PARSE_REF(STR("stack.p.flags"))},
            {STR("min"), PARSE_REF(STR("stack.p.min"))},
            {STR("max"), PARSE_REF(STR("stack.p.max"))}
          }, {
            {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("stack.p.prd")) }})}
          })}
        })
      })}
    })},
    {STR("handler"), this->parsingHandler}
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
  this->set(STR("root.MultiCmd"), SymbolDefinition::create({
    {STR("flags"), TiInt::create(ParsingFlags::ENFORCE_PROD_OBJ)}
  }, {
    {STR("term"), ConcatTerm::create({
      {STR("targetRef"), PARSE_REF(STR("stack.s"))}
    }, {
      {STR("data"), PARSE_REF(STR("args.sections"))},
      {STR("terms"), MultiplyTerm::create({
        {STR("priority"), PARSE_REF(STR("stack.s.pty"))},
        {STR("flags"), PARSE_REF(STR("stack.s.flags"))},
        {STR("min"), PARSE_REF(STR("stack.s.min"))},
        {STR("max"), PARSE_REF(STR("stack.s.max"))},
      }, {
        {STR("term"), ConcatTerm::create({
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        }, {
          {STR("terms"), List::create({}, {
            TokenTerm::create({
              {STR("tokenId"), std::make_shared<TiInt>(ID_GENERATOR->getId(STR("LexerDefs.Identifier")))},
              {STR("tokenText"), PARSE_REF(STR("stack.s.kwd"))}
            }),
            ConcatTerm::create({
              {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
              {STR("targetRef"), PARSE_REF(STR("stack.a"))}
            }, {
              {STR("data"), PARSE_REF(STR("stack.s.args"))},
              {STR("terms"), MultiplyTerm::create({
                {STR("priority"), PARSE_REF(STR("stack.a.pty"))},
                {STR("flags"), PARSE_REF(STR("stack.a.flags"))},
                {STR("min"), PARSE_REF(STR("stack.a.min"))},
                {STR("max"), PARSE_REF(STR("stack.a.max"))}
              }, {
                {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("stack.a.prd")) }})}
              })}
            })
          })}
        })}
      })}
    })},
    {STR("handler"), this->parsingHandler}
  }));


  // Expression :
  // operand {binaryOp operand}.
  // [unaryOp] operand.
  // operand [unaryOp].
  // operand {FunctionalOp}.
  this->set(
    STR("root.Expression"),
    Module::create({{STR("startRef"), PARSE_REF(STR("module.Exp"))}}).get()
  );

  // Exp : @single prod as LowestLinkExp + (@priority(in,0) lexer.Constant("\\")*(0,1));
  this->set(STR("root.Expression.Exp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.LowestLinkExp")) }}),
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
          {STR("max"), std::make_shared<TiInt>(1)},
        }, {
          {STR("term"), TokenTerm::create({
            {STR("tokenId"), TiInt::create(this->constTokenId)},
            {STR("tokenText"), TiStr::create(STR("\\"))}
          })}
        })
      })}
    })},
    {STR("handler"), this->parsingHandler}
  }));

  // LowestLinkExp : @single @prefix(heap.Modifiers.LowestLinkModifierCmd)
  //     prod (enable:integer=endless) as
  //     ConditionalExp + (@priority(in,0) (LowestLinkOp + ConditionalExp)*(0,enable));
  this->set(STR("root.Expression.LowestLinkExp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.ConditionalExp")) }}),
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {STR("max"), PARSE_REF(STR("args.enable"))},
        }, {
          {STR("term"), ConcatTerm::create({}, {
            {STR("terms"), List::create({}, {
              TokenTerm::create({
                {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {STR("tokenText"), PARSE_REF(STR("root.TokenData.lowestLinkOpList"))}
              }),
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.ConditionalExp")) }})
            })}
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {{STR("enable"), 0}})},
    {STR("handler"), std::make_shared<InfixParsingHandler<Ast::LinkOperator>>(false)}
  }));

  // ConditionalExp : @single @prefix(heap.Modifiers.ConditionalModifierCmd)
  //     prod (enable:integer[0<=n<=1]=1) as
  //     ListExp + (@priority(in,0) (lexer.Constant("?") + ListExp)*(0,enable));
  this->set(STR("root.Expression.ConditionalExp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.ListExp")) }}),
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {STR("max"), PARSE_REF(STR("args.enable"))}
        }, {
          {STR("term"), ConcatTerm::create({}, {
            {STR("terms"), List::create({}, {
              TokenTerm::create({
                {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {STR("tokenText"), Map::create(false, {}, {{STR("?"), 0}, {STR("؟"), 0}})}
              }),
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.ListExp")) }})
            })}
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {{STR("enable"), std::make_shared<TiInt>(1)}})},
    {STR("handler"), std::make_shared<InfixParsingHandler<Ast::ConditionalOperator>>(false)}
  }));

  // ListExp : @single @prefix(heap.Modifiers.ListModifierCmd)
  //   prod (enable:integer=1) as
  //     @filter(enable) LowerLinkExp ||
  //                     (LowerLinkExp || lexer.Constant(",") + LowerLinkExp*(0,1)) +
  //                       (@priority(in,0) (lexer.Constant(",") + LowerLinkExp*(0,1))*(0,endless));
  this->set(STR("root.Expression.ListExp"), SymbolDefinition::create({}, {
    {STR("term"), AlternateTerm::create({}, {
      {STR("data"), PARSE_REF(STR("args.enable"))},
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.LowerLinkExp")) }}),
        ConcatTerm::create({}, {
          {STR("terms"), List::create({}, {
            AlternateTerm::create({}, {
              {STR("terms"), List::create({}, {
                ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.LowerLinkExp")) }}),
                ConcatTerm::create({
                  {STR("flags"), TiInt::create(ParsingFlags::ENFORCE_LIST_ITEM)}
                }, {
                  {STR("terms"), List::create({}, {
                    TokenTerm::create({
                      {STR("flags"), TiInt::create(ParsingFlags::ENFORCE_TOKEN_OMIT)},
                      {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                      {STR("tokenText"), Map::create(false, {}, {{STR(","), 0}, {STR("،"), 0}})}
                    }),
                    MultiplyTerm::create({
                      {STR("priority"), std::make_shared<TiInt>(1)},
                      {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
                      {STR("max"), std::make_shared<TiInt>(1)},
                    }, {
                      {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.LowerLinkExp")) }})}
                    })
                  })}
                })
              })}
            }),
            MultiplyTerm::create({
              {STR("priority"), std::make_shared<TiInt>(1)},
              {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)}
            }, {
              {STR("term"), ConcatTerm::create({}, {
                {STR("terms"), List::create({}, {
                  TokenTerm::create({
                    {STR("flags"), TiInt::create(ParsingFlags::ENFORCE_TOKEN_OMIT)},
                    {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                    {STR("tokenText"), Map::create(false, {}, {{STR(","), 0}, {STR("،"), 0}})}
                  }),
                  MultiplyTerm::create({
                    {STR("priority"), std::make_shared<TiInt>(1)},
                    {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
                    {STR("max"), std::make_shared<TiInt>(1)}
                  }, {
                    {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.LowerLinkExp")) }})}
                  })
                })}
              })}
            })
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {{STR("enable"), std::make_shared<TiInt>(1)}})},
    {STR("handler"), std::make_shared<ListParsingHandler<Ast::List>>(-1, false)}
  }));

  // LowerLinkExp : @single @prefix(heap.Modifiers.LowerLinkModifierCmd)
  //     prod (enable:integer=endless) as
  //     AssignmentExp + (@priority(in,0) (LowerLinkOp + AssignmentExp)*(0,enable));
  this->set(STR("root.Expression.LowerLinkExp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.AssignmentExp")) }}),
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {STR("max"), PARSE_REF(STR("args.enable"))}
        }, {
          {STR("term"), ConcatTerm::create({}, {
            {STR("terms"), List::create({}, {
              TokenTerm::create({
                {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {STR("tokenText"), PARSE_REF(STR("root.TokenData.lowerLinkOpList"))}
              }),
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.AssignmentExp")) }})
            })}
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {{STR("enable"), 0}})},
    {STR("handler"), std::make_shared<InfixParsingHandler<Ast::LinkOperator>>(false)}
  }));

  // AssignmentExp : @single @prefix(heap.Modifiers.AssignmentModifierCmd)
  //     prod (enable:integer=endless) as
  //     LogExp + (@priority(in,0) (AssignmentOp + LogExp)*(0,enable));
  this->set(STR("root.Expression.AssignmentExp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.LogExp")) }}),
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {STR("max"), PARSE_REF(STR("args.enable"))}
        }, {
          {STR("term"), ConcatTerm::create({}, {
            {STR("terms"), List::create({}, {
              TokenTerm::create({
                {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {STR("tokenText"), PARSE_REF(STR("root.TokenData.assignmentOpList"))}
              }),
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.LogExp")) }})
            })}
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {{STR("enable"), 0}})},
    {STR("handler"), std::make_shared<InfixParsingHandler<Ast::AssignmentOperator>>(false)}
  }));

  // LogExp : @single @prefix(heap.Modifiers.LogModifierCmd)
  //     prod (enable:integer=endless) as
  //     ComparisonExp + (@priority(in,0) (LogOp + ComparisonExp)*(0,enable));
  this->set(STR("root.Expression.LogExp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.ComparisonExp")) }}),
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {STR("max"), PARSE_REF(STR("args.enable"))}
        }, {
          {STR("term"), ConcatTerm::create({}, {
            {STR("terms"), List::create({}, {
              TokenTerm::create({
                {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {STR("tokenText"), PARSE_REF(STR("root.TokenData.logOpList"))}
              }),
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.ComparisonExp")) }})
            })}
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {{STR("enable"), 0}})},
    {STR("handler"), std::make_shared<InfixParsingHandler<Ast::LogOperator>>(false)}
  }));

  // ComparisonExp : @single @prefix(heap.Modifiers.ComparisonModifierCmd)
  //     prod (enable:integer=endless) as
  //     LowLinkExp + (@priority(in,0) (ComparisonOp + LowLinkExp)*(0,enable));
  this->set(STR("root.Expression.ComparisonExp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.LowLinkExp")) }}),
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {STR("max"), PARSE_REF(STR("args.enable"))}
        }, {
          {STR("term"), ConcatTerm::create({}, {
            {STR("terms"), List::create({}, {
              TokenTerm::create({
                {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {STR("tokenText"), PARSE_REF(STR("root.TokenData.comparisonOpList"))}
              }),
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.LowLinkExp")) }})
            })}
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {{STR("enable"), 0}})},
    {STR("handler"), std::make_shared<InfixParsingHandler<Ast::ComparisonOperator>>(false)}
  }));

  // LowLinkExp : @single @prefix(heap.Modifiers.LowLinkModifierCmd)
  //     prod (enable:integer=endless) as
  //     AddExp + (@priority(in,0) (LowLinkOp + AddExp)*(0,enable));
  this->set(STR("root.Expression.LowLinkExp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.AddExp")) }}),
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {STR("max"), PARSE_REF(STR("args.enable"))}
        }, {
          {STR("term"), ConcatTerm::create({}, {
            {STR("terms"), List::create({}, {
              TokenTerm::create({
                {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {STR("tokenText"), PARSE_REF(STR("root.TokenData.lowLinkOpList"))}
              }),
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.AddExp")) }})
            })}
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {{STR("enable"), 0}})},
    {STR("handler"), std::make_shared<InfixParsingHandler<Ast::LinkOperator>>(false)}
  }));

  // AddExp : @single @prefix(heap.Modifiers.AddModifierCmd)
  //     prod (enable:integer=endless) as
  //     MulExp + (@priority(in,0) (AddOp + MulExp)*(0,enable));
  this->set(STR("root.Expression.AddExp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.MulExp")) }}),
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {STR("max"), PARSE_REF(STR("args.enable"))}
        }, {
          {STR("term"), ConcatTerm::create({}, {
            {STR("terms"), List::create({}, {
              TokenTerm::create({
                {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {STR("tokenText"), PARSE_REF(STR("root.TokenData.addOpList"))}
              }),
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.MulExp")) }})
            })}
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {{STR("enable"), 0}})},
    {STR("handler"), std::make_shared<InfixParsingHandler<Ast::AdditionOperator>>(false)}
  }));

  // MulExp : @single @prefix(heap.Modifiers.MulModifierCmd)
  //     prod (enable:integer=endless) as
  //     BitwiseExp + (@priority(in,0) (MulOp + BitwiseExp)*(0,enable));
  this->set(STR("root.Expression.MulExp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.BitwiseExp")) }}),
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {STR("max"), PARSE_REF(STR("args.enable"))}
        }, {
          {STR("term"), ConcatTerm::create({}, {
            {STR("terms"), List::create({}, {
              TokenTerm::create({
                {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {STR("tokenText"), PARSE_REF(STR("root.TokenData.mulOpList"))}
              }),
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.BitwiseExp")) }})
            })}
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {{STR("enable"), 0}})},
    {STR("handler"), std::make_shared<InfixParsingHandler<Ast::MultiplicationOperator>>(false)}
  }));

  // BitwiseExp : @single @prefix(heap.Modifiers.BitwiseModifierCmd)
  //     prod (enable:integer=endless) as
  //     UnaryExp + (@priority(in,0) (BitwiseOp + UnaryExp)*(0,enable));
  this->set(STR("root.Expression.BitwiseExp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.UnaryExp")) }}),
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {STR("max"), PARSE_REF(STR("args.enable"))},
        }, {
          {STR("term"), ConcatTerm::create({}, {
            {STR("terms"), List::create({}, {
              TokenTerm::create({
                {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
                {STR("tokenText"), PARSE_REF(STR("root.TokenData.bitwiseOpList"))}
              }),
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.UnaryExp")) }})
            })}
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {{STR("enable"), 0}})},
    {STR("handler"), std::make_shared<InfixParsingHandler<Ast::BitwiseOperator>>(false)}
  }));

  // UnaryExp : @single @prefix(heap.Modifiers.UnaryModifierCmd)
  //     prod (enable1:integer[0<=n<=1]=1, enable2:integer[0<=n<=1]=1) as
  //     (@priority(in,0) PrefixOp*(0,enable1)) + FunctionalExp + (@priority(in,0) PostfixOp*(0,enable2));
  this->set(STR("root.Expression.UnaryExp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {STR("max"), PARSE_REF(STR("args.enable1"))}
        }, {
          {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.PrefixOp")) }})}
        }),
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.FunctionalExp")) }}),
        MultiplyTerm::create({
          {STR("priority"), std::make_shared<TiInt>(1)},
          {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
          {STR("max"), PARSE_REF(STR("args.enable2"))}
        }, {
          {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.PostfixOp")) }})}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {
      {STR("enable1"), std::make_shared<TiInt>(1)},
      {STR("enable2"), std::make_shared<TiInt>(1)}
    })},
    {STR("handler"), std::make_shared<OutfixParsingHandler<Ast::PrefixOperator, Ast::PostfixOperator>>()}
  }));

  // FunctionalExp : @single @prefix(heap.Modifiers.FunctionalModifierCmd)
  //     prod (operand:production[heap.Subject]=heap.Subject, fltr1:filter=null,
  //           fltr2:filter=null, dup:integer=endless, pty1:integer=1, pty2:integer=in) as
  //     @filter(fltr1) @priority(pty1,0)
  //         (operand + (@priority(pty2,0) (@filter(fltr2) ParamPassExp ||
  //                                                       PostfixTildeExp ||
  //                                                       LinkExp(operand))*(0,dup))) ||
  //         PrefixTildeExp + operand;
  this->set(STR("root.Expression.FunctionalExp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("args.operand")) }}),
        MultiplyTerm::create({
          {STR("priority"), PARSE_REF(STR("args.pty2"))},
          {STR("flags"), PARSE_REF(STR("args.flags"))},
          {STR("max"), PARSE_REF(STR("args.dup"))}
        }, {
          {STR("term"), AlternateTerm::create({}, {
            {STR("data"), PARSE_REF(STR("args.fltr2"))},
            {STR("terms"), List::create({}, {
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.LinkExp")) }}),
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.PostfixTildeExp")) }}),
              ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.ParamPassExp")) }})
            })}
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {
      {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
      {STR("operand"), PARSE_REF(STR("root.Subject"))},
      {STR("pty2"), std::make_shared<TiInt>(1)},
      {STR("dup"), 0},
      {STR("fltr2"), 0}
    })},
    {STR("handler"), std::make_shared<ChainOpParsingHandler>()}
  }));

  // LinkExp : @single prod (operand:production[heap.Subject]=heap.Subject) as LinkOp + operand;
  this->set(STR("root.Expression.LinkExp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
       {STR("terms"), List::create({}, {
          TokenTerm::create({
            {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
            {STR("tokenText"), PARSE_REF(STR("root.TokenData.linkOpList"))}
          }),
          ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("args.operand")) }})
        })}
     })},
    {STR("vars"), Map::create(false, {}, {{STR("operand"), PARSE_REF(STR("root.Subject"))}})},
    {STR("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto currentList = state->getData().ti_cast_get<Containing<TiObject>>();
      auto metadata = ti_cast<Ast::MetaHaving>(currentList);
      auto token = tio_cast<Ast::Token>(currentList->getElement(0));
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
  this->set(STR("root.Expression.ParamPassExp"), SymbolDefinition::create({}, {
    {STR("term"), AlternateTerm::create({
      {STR("flags"), TiInt::create(ParsingFlags::ENFORCE_ROUTE_OBJ|TermFlags::ONE_ROUTE_TERM)},
    }, {
      {STR("data"), PARSE_REF(STR("args.fltr"))},
      {STR("terms"), List::create({}, {
        ConcatTerm::create({
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
        }, {
          {STR("terms"), List::create({}, {
            TokenTerm::create({
              {STR("tokenId"), TiInt::create(this->constTokenId)},
              {STR("tokenText"), TiStr::create(STR("("))}
            }),
            MultiplyTerm::create({
              {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
              {STR("max"), std::make_shared<TiInt>(1)},
            }, {
              {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("args.expr")) }})}
            }),
            TokenTerm::create({
              {STR("tokenId"), TiInt::create(this->constTokenId)},
              {STR("tokenText"), TiStr::create(STR(")"))}
            })
          })}
        }),
        ConcatTerm::create({
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
        }, {
          {STR("terms"), List::create({}, {
            TokenTerm::create({
              {STR("tokenId"), TiInt::create(this->constTokenId)},
              {STR("tokenText"), TiStr::create(STR("["))}
            }),
            MultiplyTerm::create({
              {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
              {STR("max"), std::make_shared<TiInt>(1)}
            }, {
              {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("args.expr")) }})}
            }),
            TokenTerm::create({
              {STR("tokenId"), TiInt::create(this->constTokenId)},
              {STR("tokenText"), TiStr::create(STR("]"))}
            })
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {
      {STR("expr"), PARSE_REF(STR("root.Expression"))},
      {STR("fltr"), 0}})},
    {STR("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto currentRoute = state->getData().tio_cast_get<Ast::Route>();
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
  this->set(STR("root.Expression.PostfixTildeExp"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        TokenTerm::create({
          {STR("tokenId"), TiInt::create(this->constTokenId)},
          {STR("tokenText"), TiStr::create(STR("~"))}
        }),
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("args.cmd")) }})
      })}
    })},
    {STR("vars"), Map::create(false, {}, {
      {STR("cmd"), PARSE_REF(STR("module.DefaultPostfixTildeCmd"))}
    })},
    {STR("handler"), this->parsingHandler}
  }));
  //OpenPostfixTildeCmd : @limit[user.parent==PostfixTildeCmd]
  //    prod (expr:production[Expression], args:list[hash[sbj:valid_subject, min:integer,
  //                                                      max:integer, pty:integer]]) as
  //    lexer.Constant("(") + expr + concat (args:a)->( @priority(pty,0) a.sbj*(a.min,a.max) ) +
  //    lexer.Constant(")");
  this->set(STR("root.Expression.OpenPostfixTildeCmd"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({}, {
      {STR("terms"), List::create({}, {
        TokenTerm::create({
          {STR("tokenId"), TiInt::create(this->constTokenId)},
          {STR("tokenText"), TiStr::create(STR("("))}
        }),
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("args.expr")) }}),
        ConcatTerm::create({
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
          {STR("targetRef"), PARSE_REF(STR("stack.p"))}
        }, {
          {STR("data"), PARSE_REF(STR("args.prms"))},
          {STR("terms"), MultiplyTerm::create({
            {STR("priority"), PARSE_REF(STR("stack.p.pty"))},
            {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)},
            {STR("min"), PARSE_REF(STR("stack.p.min"))},
            {STR("max"), PARSE_REF(STR("stack.p.max"))},
          }, {
            {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("stack.p.prd")) }})}
          })}
        }),
        TokenTerm::create({
          {STR("tokenId"), TiInt::create(this->constTokenId)},
          {STR("tokenText"), TiStr::create(STR(")"))}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {
      {STR("expr"), PARSE_REF(STR("root.Expression"))},
      {STR("prms"), List::create({}, {Map::create(false, {}, {
        {STR("pty"), std::make_shared<TiInt>(1)},
        {STR("min"), 0},
        {STR("max"), 0},
        {STR("prd"), PARSE_REF(STR("root.Expression"))}
      })})}
    })},
    {STR("handler"), std::make_shared<CustomParsingHandler>(
      [](Parser *parser, ParserState *state)
      {
        auto data = state->getData();
        auto metadata = data.ti_cast_get<Ast::MetaHaving>();
        auto linkOp = Ast::LinkOperator::create({
          { "prodId", metadata->getProdId() },
          { "sourceLocation", metadata->findSourceLocation() }
        });
        linkOp->setType(STR("~"));
        linkOp->setSecond(data);
        state->setData(linkOp);
      }
    )}
  }));
  //DefaultPostfixTildeCmd=>PostfixTildeCmd : prod_group;
  this->set(STR("root.Expression.DefaultPostfixTildeCmd"), SymbolDefinition::create({}, {
    {STR("term"), AlternateTerm::create({
      {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)},
      {STR("targetRef"), PARSE_REF(STR("stack.cmd"))}
    }, {
      {STR("data"), PARSE_REF(STR("args.cmds"))},
      {STR("terms"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("stack.cmd")) }})}
    })},
    {STR("vars"), Map::create(false, {}, {
      {STR("cmds"), List::create({}, {PARSE_REF(STR("module.OpenPostfixTildeCmd"))})}
    })},
    {STR("handler"), this->parsingHandler}
  }));

  //// Operators :
  // PrefixOp : prod ref heap.ConstantKeywordGroup(heap.TokenData.prefixOpList);
  this->set(STR("root.Expression.PrefixOp"), SymbolDefinition::create({}, {
    {STR("term"), TokenTerm::create({
      {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
      {STR("tokenText"), PARSE_REF(STR("root.TokenData.prefixOpList"))}
    })},
    {STR("handler"), this->parsingHandler}
  }));
  // PostfixOp : prod ref heap.ConstantKeywordGroup(heap.TokenData.postfixOpList);
  this->set(STR("root.Expression.PostfixOp"), SymbolDefinition::create({}, {
    {STR("term"), TokenTerm::create({
      {STR("tokenId"), std::make_shared<TiInt>(this->constTokenId)},
      {STR("tokenText"), PARSE_REF(STR("root.TokenData.postfixOpList"))}
    })},
    {STR("handler"), this->parsingHandler}
  }));


  // Subject : Parameter | Command | Expression | Statement | Set.
  this->set(
    STR("root.Subject"),
    Module::create({{STR("startRef"), PARSE_REF(STR("module.Subject1"))}}).get()
  );

  // Subject1 : @single prod (sbj1:list[production[Parameter||Command||Expression||Statement||Set]]
  //                              =(Parameter, SubjectCommandGroup, heap.Default_Set),
  //                          sbj2:list[production[Parameter||Command||Expression||Statement||Set]]
  //                              =(heap.Expression),
  //                          sbj3:list[production[Parameter||Command||Expression||Statement||Set]]
  //                              =(heap.Expression),
  //                          frc2:integer=0, frc3:integer=0) as
  //     alternate (sbj1:s)->( s ) ||
  //     lexer.Constant("(") + (alternate (sbj2:s)->( s ))*(frc2,1) + lexer.Constant(")") ||
  //     lexer.Constant("[") + (alternate (sbj3:s)->( s ))*(frc3,1) + lexer.Constant("]");
  this->set(STR("root.Subject.Subject1"), SymbolDefinition::create({}, {
    {STR("term"), AlternateTerm::create({
      {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)}
    }, {
      {STR("terms"), List::create({}, {
        AlternateTerm::create({
          {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)},
          {STR("targetRef"), PARSE_REF(STR("stack.s"))}
        }, {
          {STR("data"), PARSE_REF(STR("args.sbj1"))},
          {STR("terms"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("stack.s")) }})}
        }),
        ConcatTerm::create({
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        }, {
          {STR("terms"), List::create({}, {
            TokenTerm::create({
              {STR("tokenId"), TiInt::create(this->constTokenId)},
              {STR("tokenText"), TiStr::create(STR("("))}
            }),
            MultiplyTerm::create({
              {STR("min"), PARSE_REF(STR("args.frc2"))},
              {STR("max"), std::make_shared<TiInt>(1)}
            }, {
              {STR("term"), AlternateTerm::create({
                {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)},
                {STR("targetRef"), PARSE_REF(STR("stack.s2"))}
              }, {
                {STR("data"), PARSE_REF(STR("args.sbj2"))},
                {STR("terms"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("stack.s2")) }})}
              })}
            }),
            TokenTerm::create({
              {STR("tokenId"), TiInt::create(this->constTokenId)},
              {STR("tokenText"), TiStr::create(STR(")"))}
            })
          })}
        }),
        ConcatTerm::create({
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        }, {
          {STR("terms"), List::create({}, {
            TokenTerm::create({
              {STR("tokenId"), TiInt::create(this->constTokenId)},
              {STR("tokenText"), TiStr::create(STR("["))}
            }),
            MultiplyTerm::create({
              {STR("min"), PARSE_REF(STR("args.frc3"))},
              {STR("max"), std::make_shared<TiInt>(1)}
            }, {
              {STR("term"), AlternateTerm::create({
                {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)},
                {STR("targetRef"), PARSE_REF(STR("stack.s3"))}
              }, {
                {STR("data"), PARSE_REF(STR("args.sbj3"))},
                {STR("terms"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("stack.s3")) }})}
              })}
            }),
            TokenTerm::create({
              {STR("tokenId"), TiInt::create(this->constTokenId)},
              {STR("tokenText"), TiStr::create(STR("]"))}
            })
          })}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {
      {STR("sbj1"), List::create({}, {
        PARSE_REF(STR("module.SubjectCmdGrp")),
        PARSE_REF(STR("module.Parameter")),
        PARSE_REF(STR("root.Set"))
      })},
      {STR("sbj2"), List::create({}, {PARSE_REF(STR("root.Expression"))})},
      {STR("sbj3"), List::create({}, {PARSE_REF(STR("root.Expression"))})},
      {STR("frc2"), 0},
      {STR("frc3"), 0}
    })},
    {STR("handler"), std::make_shared<SubjectParsingHandler>()}
  }));

  // Subject2 : @limit[user.owner==Subject] prod (sbj:production[Parameter||Command||Expression||Statement||Set],
  //                                             fltr:filter, frc:integer) as
  //     @filter(fltr) sbj ||
  //                   lexer.Constant("(") + sbj*(frc,1) + lexer.Constant(")") ||
  //                   lexer.Constant("[") + sbj*(frc,1) + lexer.Constant("]");
  this->set(STR("root.Subject.Subject2"), SymbolDefinition::create({}, {
    {STR("term"), AlternateTerm::create({
      {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)}
    }, {
      {STR("data"), PARSE_REF(STR("args.fltr"))},
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("args.sbj")) }}),
        ConcatTerm::create({
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        }, {
          {STR("terms"), List::create({}, {
            TokenTerm::create({
              {STR("tokenId"), TiInt::create(this->constTokenId)},
              {STR("tokenText"), TiStr::create(STR("("))}
            }),
            MultiplyTerm::create({
              {STR("min"), PARSE_REF(STR("args.frc"))},
              {STR("max"), std::make_shared<TiInt>(1)}
            }, {
              {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("args.sbj")) }})}
            }),
            TokenTerm::create({
              {STR("tokenId"), TiInt::create(this->constTokenId)},
              {STR("tokenText"), TiStr::create(STR(")"))}
            })
          })}
        }),
        ConcatTerm::create({
          {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP)}
        }, {
          {STR("terms"), List::create({}, {
            TokenTerm::create({
              {STR("tokenId"), TiInt::create(this->constTokenId)},
              {STR("tokenText"), TiStr::create(STR("["))}
            }),
            MultiplyTerm::create({
              {STR("min"), PARSE_REF(STR("args.frc"))},
              {STR("max"), std::make_shared<TiInt>(1)}
            }, {
              {STR("term"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("args.sbj")) }})}
            }),
            TokenTerm::create({
              {STR("tokenId"), TiInt::create(this->constTokenId)},
              {STR("tokenText"), TiStr::create(STR("]"))}
            })
          })}
        })
      })}
    })},
    {STR("handler"), std::make_shared<SubjectParsingHandler>()}
  }));

  // SubjectCommandGroup
  this->set(STR("root.Subject.SubjectCmdGrp"), SymbolDefinition::create({}, {
    {STR("term"), AlternateTerm::create({
      {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)},
      {STR("targetRef"), PARSE_REF(STR("stack.cmd"))}
    }, {
      {STR("data"), PARSE_REF(STR("args.cmds"))},
      {STR("terms"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("stack.cmd")) }})}
    })},
    {STR("vars"), Map::create(false, {}, {{ STR("cmds"), List::create({}, {
      PARSE_REF(STR("module.Alias")),
    }) }} )},
    {STR("handler"), this->parsingHandler}
  }));

  //// Parameter: Identifier | Literal.
  // Parameter : @single @prefix(heap.Modifiers.ParameterModifierCmd)
  //     prod (fltr:filter=null, cnsts:keywords=null) as
  //     @filter(fltr) lexer.Identifier || Literal || heap.ConstantKeywordGroup(cnsts);
  this->set(STR("root.Subject.Parameter"), SymbolDefinition::create({}, {
    {STR("term"), AlternateTerm::create({
      {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)}
    }, {
      {STR("data"), PARSE_REF(STR("args.fltr"))},
      {STR("terms"), List::create({}, {
        TokenTerm::create({
          {STR("tokenId"), std::make_shared<TiInt>(ID_GENERATOR->getId(STR("LexerDefs.Identifier")))}
        }),
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.Literal")) }})
      })}
    })},
    {STR("vars"), Map::create(false, {}, {{ STR("fltr"), 0 }} )},
    {STR("handler"), std::make_shared<CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto current = state->getData().tio_cast_get<Ast::Token>();
      SharedPtr<Ast::Text> newObj;
      if (current->getId() == ID_GENERATOR->getId(STR("LexerDefs.Identifier"))) {
        newObj = std::make_shared<Ast::Identifier>();
      } else if (current->getId() == ID_GENERATOR->getId(STR("LexerDefs.IntLiteral"))) {
        newObj = std::make_shared<Ast::IntegerLiteral>();
      } else if (current->getId() == ID_GENERATOR->getId(STR("LexerDefs.FloatLiteral"))) {
        newObj = std::make_shared<Ast::FloatLiteral>();
      } else if (current->getId() == ID_GENERATOR->getId(STR("LexerDefs.CharLiteral"))) {
        newObj = std::make_shared<Ast::CharLiteral>();
      } else if (current->getId() == ID_GENERATOR->getId(STR("LexerDefs.StringLiteral"))) {
        newObj = std::make_shared<Ast::StringLiteral>();
      }
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
  this->set(STR("root.Subject.Literal"), SymbolDefinition::create({}, {
    {STR("term"), AlternateTerm::create({
      {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)}
    }, {
      {STR("data"), PARSE_REF(STR("args.fltr"))},
      {STR("terms"), List::create({}, {
        TokenTerm::create({{ STR("tokenId"), TiInt::create(ID_GENERATOR->getId(STR("LexerDefs.IntLiteral"))) }}),
        TokenTerm::create({{ STR("tokenId"), TiInt::create(ID_GENERATOR->getId(STR("LexerDefs.FloatLiteral"))) }}),
        TokenTerm::create({{ STR("tokenId"), TiInt::create(ID_GENERATOR->getId(STR("LexerDefs.CharLiteral"))) }}),
        TokenTerm::create({{ STR("tokenId"), TiInt::create(ID_GENERATOR->getId(STR("LexerDefs.StringLiteral"))) }})
      })}
    })},
    {STR("vars"), Map::create(false, {}, {{ STR("fltr"), 0 }} )},
    {STR("handler"), this->parsingHandler}
  }));

  //// Alias = "alias" + Subject
  this->set(STR("root.Subject.Alias"), SymbolDefinition::create({}, {
    {STR("term"), PARSE_REF(STR("root.Cmd"))},
    {STR("vars"), Map::create(false, {}, {
      {STR("kwd"), Map::create(false, {}, { { STR("alias"), 0 }, { STR("لقب"), 0 } }) },
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
    {STR("handler"), std::make_shared<CustomParsingHandler>(
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


  // Set : "{" StatementList "}".
  // Set : @limit[child.terms==self,user.parent==self] @prefix(heap.Modifiers.DefaultModifierCmd)
  //     prod (stmt:production[Statement], ovrd:grammar_override) as
  //     lexer.Constant("{") + @override(ovrd) StatementList(stmt) + lexer.Constant("}");
  this->set(STR("root.Set"), SymbolDefinition::create({}, {
    {STR("term"), ConcatTerm::create({
      {STR("errorSyncPosId"), TiInt(1)}
    }, {
      {STR("terms"), List::create({}, {
        TokenTerm::create({
          {STR("tokenId"), TiInt::create(this->constTokenId)},
          {STR("tokenText"), TiStr::create(STR("{"))}
        }),
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("args.stmt")) }}),
        TokenTerm::create({
          {STR("tokenId"), TiInt::create(this->constTokenId)},
          {STR("tokenText"), TiStr::create(STR("}"))}
        })
      })}
    })},
    {STR("vars"), Map::create(false, {}, {
       {STR("stmt"), PARSE_REF(STR("root.Main.StatementList"))}
     })},
    {STR("handler"), this->parsingHandler}
  }));


  // Modifiers
  this->set(STR("root.Modifier"), Module::create({}));
  // Modifier.Subject
  this->set(STR("root.Modifier.Subject"), Module::create({
    { STR("baseRef"), PARSE_REF(STR("root.Subject")) }
  }));
  this->set(STR("root.Modifier.Subject.SubjectCmdGrp"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("bmodule.SubjectCmdGrp")) }
  }, {
    {STR("vars"), Map::create(false, {}, {{ STR("cmds"), List::create() }} )},
  }));
  // Modifier.Expression
  this->set(STR("root.Modifier.Expression"), Module::create({
    { STR("startRef"), PARSE_REF(STR("module.FunctionalExp")) },
    { STR("baseRef"), PARSE_REF(STR("root.Expression")) }
  }));
  this->set(STR("root.Modifier.Expression.FunctionalExp"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("bmodule.FunctionalExp")) }
  }, {
    {STR("vars"), Map::create(false, {}, {
      {STR("flags"), TiInt::create(ParsingFlags::PASS_ITEMS_UP | TermFlags::ONE_ROUTE_TERM)},
      {STR("operand"), PARSE_REF(STR("root.Modifier.Subject"))},
      {STR("pty2"), std::make_shared<TiInt>(1)},
      {STR("dup"), TiInt::create(1)},
      {STR("fltr2"), TiInt::create(2)}
     })}
  }));
  // Modifier.CmdGroup
  this->set(STR("root.Modifier.CmdGroup"), SymbolDefinition::create({}, {
    {STR("term"), AlternateTerm::create({
      {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)},
      {STR("targetRef"), PARSE_REF(STR("stack.cmd"))}
    }, {
      {STR("data"), PARSE_REF(STR("args.cmds"))},
      {STR("terms"), ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("stack.cmd")) }})}
    })},
    {STR("vars"), Map::create(false, {}, {
      {STR("cmds"), List::create()}
    })},
    {STR("handler"), this->parsingHandler}
  }));
  // Modifier.Phrase
  this->set(STR("root.Modifier.Phrase"), SymbolDefinition::create({}, {
    {STR("term"), AlternateTerm::create({
      {STR("flags"), TiInt::create(TermFlags::ONE_ROUTE_TERM)}
    }, {
      {STR("terms"), List::create({}, {
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.CmdGroup")) }}),
        ReferenceTerm::create({{ STR("reference"), PARSE_REF(STR("module.Expression")) }})
      })}
    })}
  }));
  // Modifier.LeadingModifier
  this->set(STR("root.Modifier.LeadingModifier"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("module.Phrase")) }
  }, {
    {STR("handler"), this->leadingModifierHandler}
  }));
  // Modifier.TrailingModifier
  this->set(STR("root.Modifier.TrailingModifier"), SymbolDefinition::create({
    {STR("baseRef"), PARSE_REF(STR("module.Phrase")) }
  }, {
    {STR("handler"), this->trailingModifierHandler}
  }));

  // Modifiers parsing dimensions.
  this->set(STR("root.LeadingModifierDim"), ParsingDimension::create({
    {STR("entryTokenText"), std::make_shared<TiStr>(STR("@"))},
    {STR("startRef"), PARSE_REF(STR("root.Modifier.LeadingModifier"))}
  }));
  this->set(STR("root.TrailingModifierDim"), ParsingDimension::create({
    {STR("entryTokenText"), std::make_shared<TiStr>(STR("@<"))},
    {STR("startRef"), PARSE_REF(STR("root.Modifier.TrailingModifier"))}
  }));
}


SharedPtr<SymbolDefinition> StandardFactory::createConstTokenDef(Char const *text)
{
  if (SBSTR(text) == STR("@") || SBSTR(text) == STR("@<")) {
    return SymbolDefinition::create({
      {STR("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
    }, {
      {STR("term"), ConstTerm::create({{ STR("matchString"), TiWStr(text) }})}
    });
  } else {
    return SymbolDefinition::create({
      {STR("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
    }, {
      {STR("term"), ConstTerm::create({{ STR("matchString"), TiWStr(text) }})},
      {STR("handler"), this->constTokenHandler}
    });
  }
}

} // namespace
