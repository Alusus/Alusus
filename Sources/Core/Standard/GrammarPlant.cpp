/**
 * @file Core/Standard/GrammarPlant.cpp
 * Contains the implementation of class Core::Standard::GrammarPlant.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Standard
{

using namespace Core::Data;
using namespace Core::Processing;

//==============================================================================
// Member Functions

/**
 * Creates the entire list of definitions for productions, tokens, and char
 * groups for the Core's grammar. This function will give you the complete
 * grammar definitions with all the required handlers.
 */
void GrammarPlant::createGrammar(RootManager *root, Bool exprOnly)
{
  this->constTokenPrefix = STR("LexerDefs");
  this->constTokenId = ID_GENERATOR->getId(STR("CONSTTOKEN"));

  // Instantiate handlers.
  this->stringLiteralHandler = std::make_shared<StringLiteralTokenizingHandler>();
  this->constTokenHandler = std::make_shared<ConstTokenizingHandler>(this->constTokenId);
  this->parsingHandler = std::make_shared<Handlers::GenericParsingHandler>();
  this->importHandler = std::make_shared<ImportParsingHandler>(root);
  this->dumpParsingHandler = std::make_shared<DumpParsingHandler>(root);
  this->leadingModifierHandler = std::make_shared<Handlers::ModifierParsingHandler>(true);
  this->trailingModifierHandler = std::make_shared<Handlers::ModifierParsingHandler>(false);
  this->doCommandParsingHandler = std::make_shared<Handlers::GenericCommandParsingHandler>(STR("do"));

  // Create lexer definitions.
  this->repository.set(STR("root:LexerDefs"), GrammarModule::create({}).get());
  this->createCharGroupDefinitions();
  this->createTokenDefinitions();

  // Create parser definitions.
  this->createProductionDefinitions(exprOnly);

  // Create error sync block pairs.
  this->repository.set(STR("root:ErrorSyncBlockPairs"), SharedList::create({
    TokenTerm::create(this->constTokenId, STR("(")),
    TokenTerm::create(this->constTokenId, STR(")")),
    TokenTerm::create(this->constTokenId, STR("[")),
    TokenTerm::create(this->constTokenId, STR("]")),
    TokenTerm::create(this->constTokenId, STR("{")),
    TokenTerm::create(this->constTokenId, STR("}"))
  }).get());

  // Set start production and lexer module.
  GrammarModule *rootModule = this->repository.getRoot().get();
  rootModule->setStartRef(REF_PARSER->parseQualifier(STR("module:Program")));
  rootModule->setLexerModuleRef(REF_PARSER->parseQualifier(STR("root:LexerDefs")));
  rootModule->setErrorSyncBlockPairsRef(REF_PARSER->parseQualifier(STR("root:ErrorSyncBlockPairs")));

  // Generate const token definitions from production definitions.
  this->generateConstTokenDefinitions();
}


/**
 * Create a CharGroupDefinitionList and add all the required definitions for
 * the Core's grammar.
 */
void GrammarPlant::createCharGroupDefinitions()
{
  // BinDigit : char '0'..'1';
  this->repository.set(STR("root:LexerDefs.BinDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(STR("0"), STR("1"))).get());

  // OctDigit : char '0'..'7';
  this->repository.set(STR("root:LexerDefs.OctDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(STR("0"), STR("7"))).get());

  // DecDigit : char '0'..'9';
  this->repository.set(STR("root:LexerDefs.DecDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(STR("0"), STR("9"))).get());

  // HexDigit : char '0'..'9', 'a'..'f', 'A'..'F';
  this->repository.set(STR("root:LexerDefs.HexDigit"), CharGroupDefinition::create(
    UnionCharGroupUnit::create({
      SequenceCharGroupUnit::create(STR("0"), STR("9")),
      SequenceCharGroupUnit::create(STR("a"), STR("f")),
      SequenceCharGroupUnit::create(STR("A"), STR("F"))
    })).get());

  // Letter : char 'a'..'z', 'A'..'Z', '_';
  SharedPtr<CharGroupUnit> letterCharGroup =
      UnionCharGroupUnit::create({
        SequenceCharGroupUnit::create(STR("a"), STR("z")),
        SequenceCharGroupUnit::create(STR("A"), STR("Z")),
        SequenceCharGroupUnit::create(STR("_"), STR("_")),
        SequenceCharGroupUnit::create(STR("ؠ"), STR("ٟ")),
        SequenceCharGroupUnit::create(STR("ٮ"), STR("ۜ"))
      });
  this->repository.set(STR("root:LexerDefs.Letter"), CharGroupDefinition::create(letterCharGroup).get());
  ReferenceParser::setSharedLetterCharGroup(letterCharGroup);

  // AnyCharNoEs : char !('\\');
  this->repository.set(STR("root:LexerDefs.AnyCharNoEs"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      SequenceCharGroupUnit::create(STR("\\"), STR("\\")))).get());

  // AnyCharNoEsOrSingleQuote : char !("\\'");
  this->repository.set(STR("root:LexerDefs.AnyCharNoEsOrSingleQuote"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      RandomCharGroupUnit::create(STR("\\'")))).get());

  // AnyCharNoEsOrDoubleQuote : char !("\\\"");
  this->repository.set(STR("root:LexerDefs.AnyCharNoEsOrDoubleQuote"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      RandomCharGroupUnit::create(STR("\\\"")))).get());

  // AnyCharNoReturn = !('\\');
  this->repository.set(STR("root:LexerDefs.AnyCharNoReturn"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      SequenceCharGroupUnit::create(STR("\n"), STR("\n")))).get());

  // Spacing : char " \n\r\t";
  this->repository.set(STR("root:LexerDefs.Spacing"), CharGroupDefinition::create(
    RandomCharGroupUnit::create(STR(" \r\n\t"))).get());
}


/**
 * Create a TokenDefinitionList and add all the required definitions for
 * the Core's grammar.
 */
void GrammarPlant::createTokenDefinitions()
{
  // Identifier : trule as { Letter + (Letter || DecDigit)*(0,endless) };
  this->repository.set(STR("root:LexerDefs.Identifier"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         CharGroupTerm::create(STR("module:Letter")),
         MultiplyTerm::create({
           {TermElement::TERM, AlternateTerm::create({
              {TermElement::TERM, SharedList::create({
                 CharGroupTerm::create(STR("module:Letter")),
                 CharGroupTerm::create(STR("module:DecDigit"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::FLAGS, Integer::create(SymbolFlags::ROOT_TOKEN)}
  }).get());

  // IntLiteral : trule as {
  //   (DecIntLiteral || BinIntLiteral || OctIntLiteral || HexIntLiteral) +
  //   ("u" || "U")*(0,1) + (("i" || "I") + DecIntLiteral)*(0,1)
  // };
  this->repository.set(STR("root:LexerDefs.IntLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ReferenceTerm::create(STR("module:DecIntLiteral")),
              ReferenceTerm::create(STR("module:BinIntLiteral")),
              ReferenceTerm::create(STR("module:OctIntLiteral")),
              ReferenceTerm::create(STR("module:HexIntLiteral"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MAX, std::make_shared<Integer>(1)},
           {TermElement::TERM, AlternateTerm::create({
              {TermElement::TERM, SharedList::create({
                 ConstTerm::create(0, STR("u")),
                 ConstTerm::create(0, STR("U"))
              })}
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MAX, std::make_shared<Integer>(1)},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::TERM, SharedList::create({
                AlternateTerm::create({
                  {TermElement::TERM, SharedList::create({
                     ConstTerm::create(0, STR("i")),
                     ConstTerm::create(0, STR("I"))
                  })}
                }),
                ReferenceTerm::create(STR("module:DecIntLiteral"))
              })}
           })}
         })
       })}
    })},
   {SymbolDefElement::FLAGS, Integer::create(SymbolFlags::ROOT_TOKEN)}
  }).get());

  // @inner DecIntLiteral : trule as { DecDigit*(1,endless) };
  this->repository.set(STR("root:LexerDefs.DecIntLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, MultiplyTerm::create({
      {TermElement::MIN, std::make_shared<Integer>(1)},
      {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
    })}
  }).get());

  // @inner BinIntLiteral : trule as { ("0b" || "0B") + BinDigit*(1,endless) };
  this->repository.set(STR("root:LexerDefs.BinIntLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("0b")),
              ConstTerm::create(0, STR("0B"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MIN, std::make_shared<Integer>(1)},
           {TermElement::TERM, CharGroupTerm::create(STR("module:BinDigit"))}
         })
       })}
    })}
  }).get());

  // @inner OctIntLiteral : trule as { ("0o" || "0O") + OctDigit*(1,endless) };
  this->repository.set(STR("root:LexerDefs.OctIntLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("0o")),
              ConstTerm::create(0, STR("0O"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MIN, std::make_shared<Integer>(1)},
           {TermElement::TERM, CharGroupTerm::create(STR("module:OctDigit"))}
         })
       })}
    })}
  }).get());

  // @inner HexIntLiteral : trule as { ("0h" || "0H") + HexDigit*(1,endless) };
  this->repository.set(STR("root:LexerDefs.HexIntLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("0h")),
              ConstTerm::create(0, STR("0H"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MIN, std::make_shared<Integer>(1)},
           {TermElement::TERM, CharGroupTerm::create(STR("module:HexDigit"))}
         })
       })}
    })}
  }).get());

  // FloatLiteral : trule as {
  //   DecDigit*(1,endless) + FloatPostfix ||
  //   DecDigit*(1,endless) + FloatExponent + FloatPostfix*(0,1) ||
  //   DecDigit*(1,endless) + "." + DecDigit*(1,endless) +
  //     FloatExponent*(0,1) + FloatPostfix*(1,1)
  // };
  this->repository.set(STR("root:LexerDefs.FloatLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::TERM, SharedList::create({
         ConcatTerm::create({
           {TermElement::TERM, SharedList::create({
              MultiplyTerm::create({
                {TermElement::MIN, std::make_shared<Integer>(1)},
                {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
              }),
              ReferenceTerm::create(STR("module:FloatPostfix"))
           })}
         }),
         ConcatTerm::create({
           {TermElement::TERM, SharedList::create({
              MultiplyTerm::create({
                {TermElement::MIN, std::make_shared<Integer>(1)},
                {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
              }),
              ReferenceTerm::create(STR("module:FloatExponent")),
              MultiplyTerm::create({
                {TermElement::MAX, std::make_shared<Integer>(1)},
                {TermElement::TERM, ReferenceTerm::create(STR("module:FloatPostfix"))}
              })
           })}
         }),
         ConcatTerm::create({
           {TermElement::TERM, SharedList::create({
              MultiplyTerm::create({
                {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
              }),
              ConstTerm::create(0, STR(".")),
              MultiplyTerm::create({
                {TermElement::MIN, std::make_shared<Integer>(1)},
                {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
              }),
              MultiplyTerm::create({
                {TermElement::MAX, std::make_shared<Integer>(1)},
                {TermElement::TERM, ReferenceTerm::create(STR("module:FloatExponent"))}
              }),
              MultiplyTerm::create({
                {TermElement::MAX, std::make_shared<Integer>(1)},
                {TermElement::TERM, ReferenceTerm::create(STR("module:FloatPostfix"))}
              })
           })}
         })
       })}
    })},
   {SymbolDefElement::FLAGS, Integer::create(SymbolFlags::ROOT_TOKEN)}
  }).get());

  // @inner FloatExponent : trule as { ("e" || "E") + ("+" || "-")*(0,1) + DecDigit*(1,endless) };
  this->repository.set(STR("root:LexerDefs.FloatExponent"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
        AlternateTerm::create({
          {TermElement::TERM, SharedList::create({
            ConstTerm::create(0, STR("e")),
            ConstTerm::create(0, STR("E"))
          })}
        }),
        MultiplyTerm::create({
          {TermElement::MAX, std::make_shared<Integer>(1)},
          {TermElement::TERM, AlternateTerm::create({
            {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("+")),
              ConstTerm::create(0, STR("-"))
            })}
          })}
        }),
        MultiplyTerm::create({
          {TermElement::MIN, std::make_shared<Integer>(1)},
          {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
        })
      })}
    })}
  }).get());

  // @inner FloatPostfix : trule as { ("f" || "F") + DecDigit*(0,endless) };
  this->repository.set(STR("root:LexerDefs.FloatPostfix"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("f")),
              ConstTerm::create(0, STR("F"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
         })
       })}
    })}
  }).get());

  // CharLiteral : trule as { "'" + EsCharWithDoubleQuote + "'" + CharCodePostfix*(0,1) };
  // TODO: Add the char_code_postfix part
  this->repository.set(STR("root:LexerDefs.CharLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ConstTerm::create(0, STR("'")),
         ReferenceTerm::create(STR("module:EsCharWithDoubleQuote")),
         ConstTerm::create(0, STR("'"))
       })}
    })},
    {SymbolDefElement::FLAGS, Integer::create(SymbolFlags::ROOT_TOKEN)}
  }).get());

  // StringLiteral : trule as {
  //   StringLiteralPart + (Spacing*(0,endless) + StringLiteralPart)*(0,endless) +
  //   CharCodePostfix*(0,1)
  // };
  // TODO: Add the CharCodePostfix part
  this->repository.set(STR("root:LexerDefs.StringLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:StringLiteralPart")),
         MultiplyTerm::create({
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::TERM, SharedList::create({
                 MultiplyTerm::create({
                   {TermElement::TERM, CharGroupTerm::create(STR("module:Spacing"))}
                 }),
                 ReferenceTerm::create(STR("module:StringLiteralPart"))
              })}
           })}
         })
       })}
    })},
    {SymbolDefElement::HANDLER, this->stringLiteralHandler},
    {SymbolDefElement::FLAGS, Integer::create(SymbolFlags::ROOT_TOKEN)}
  }).get());

  // @inner StringLiteralPart : trule as { "\"" + EsCharWithSingleQuote*(0,endless) + "\"" };
  this->repository.set(STR("root:LexerDefs.StringLiteralPart"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ConstTerm::create(0, STR("\"")),
         MultiplyTerm::create({
           {TermElement::TERM, ReferenceTerm::create(STR("module:EsCharWithSingleQuote"))}
         }),
         ConstTerm::create(0, STR("\""))
       })}
    })}
  }).get());

  // @inner EsCharWithSingleQuote : trule as {
  //   AnyCharNoEsOrDoubleQuote || EsCodeSequence ||
  //   alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the heap.escape_sequences part
  this->repository.set(STR("root:LexerDefs.EsCharWithSingleQuote"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::TERM, SharedList::create({
         CharGroupTerm::create(STR("module:AnyCharNoEsOrDoubleQuote")),
         ReferenceTerm::create(STR("module:EsSequence"))
       })}
    })}
  }).get());

  // @inner EsCharWithDoubleQuote : trule as {
  //   AnyCharNoEsOrSingleQuote || EsCodeSequence ||
  //   alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the root.TokenData.escapeSequences part
  this->repository.set(STR("root:LexerDefs.EsCharWithDoubleQuote"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::TERM, SharedList::create({
         CharGroupTerm::create(STR("module:AnyCharNoEsOrSingleQuote")),
         ReferenceTerm::create(STR("module:EsSequence"))
       })}
    })}
  }).get());

  // @inner EsCharWithQuotes : trule as {
  //   AnyCharNoEs || EsCodeSequence || alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the heap.escape_sequences part
  this->repository.set(STR("root:LexerDefs.EsCharWithQuotes"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::TERM, SharedList::create({
         CharGroupTerm::create(STR("module:AnyCharNoEs")),
         ReferenceTerm::create(STR("module:EsSequence"))
       })}
    })}
  }).get());

  // @inner EsSequence : trule as {
  //   '\\' + ('c' + HexDigit*(2,2) ||
  //           'u' + HexDigit*(4,4) ||
  //           'w' + HexDigit*(8,8) ||
  //           'n' || 't' || 'r' || '"' || '\'' || '\\')
  // };
  this->repository.set(STR("root:LexerDefs.EsSequence"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ConstTerm::create(0, STR("\\")),
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("\\")),
              ConstTerm::create(0, STR("\"")),
              ConstTerm::create(0, STR("'")),
              ConstTerm::create(0, STR("n")),
              ConstTerm::create(0, STR("t")),
              ConstTerm::create(0, STR("r")),
              ConstTerm::create(0, STR("ج")),
              ConstTerm::create(0, STR("ت")),
              ConstTerm::create(0, STR("ر")),
              ConcatTerm::create({
                {TermElement::TERM, SharedList::create({
                   ConstTerm::create(0, STR("c")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit"))
                })}
              }),
              ConcatTerm::create({
                {TermElement::TERM, SharedList::create({
                   ConstTerm::create(0, STR("u")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit"))
                })}
              }),
              ConcatTerm::create({
                {TermElement::TERM, SharedList::create({
                   ConstTerm::create(0, STR("w")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit")),
                   CharGroupTerm::create(STR("module:HexDigit"))
                })}
              })
           })}
         })
       })}
    })}
  }).get());

  //// IGNORED TOKENS

  // ignore { Spacing*(1,endless) };
  this->repository.set(STR("root:LexerDefs.IgnoredSpaces"), SymbolDefinition::create({
    {SymbolDefElement::TERM, MultiplyTerm::create({
      {TermElement::MIN, std::make_shared<Integer>(1)},
      {TermElement::TERM, CharGroupTerm::create(STR("module:Spacing"))}
    })},
    {SymbolDefElement::FLAGS, Integer::create(SymbolFlags::ROOT_TOKEN|SymbolFlags::IGNORED_TOKEN)}
  }).get());

  // @minimum ignore { "//" + any*(0,endless) + "\n" }
  // For now this is implemented as:
  // ignore { "//" + AnyCharNoReturn*(0,endless) + "\n" }
  // because the lexer still doesn't support the @minimum modifier.
  this->repository.set(STR("root:LexerDefs.LineComment"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("//")),
              ConstTerm::create(0, STR("#")),
           })}
         }),
         MultiplyTerm::create({
           {TermElement::TERM, CharGroupTerm::create(STR("module:AnyCharNoReturn"))}
         }),
         ConstTerm::create(0, STR("\n"))
       })}
    })},
    {SymbolDefElement::FLAGS, Integer::create(SymbolFlags::ROOT_TOKEN|SymbolFlags::IGNORED_TOKEN)}
  }).get());
}


/**
 * Create a Production_Definition_List and add all the required definitions for
 * the Core's grammar. The created list will have assigned parsing handlers.
 */
void GrammarPlant::createProductionDefinitions(Bool exprOnly)
{
  //// TokenData module.

  this->repository.set(STR("root:TokenData"), GrammarModule::create({}).get());
  // assignmentOpList : keywords = ("=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "$=", "<<=", ">>=");
  this->repository.set(STR("root:TokenData.assignmentOpList"), SharedMap::create(true, {
   {STR("="), 0},
   {STR("+="), 0},
   {STR("-="), 0},
   {STR("*="), 0},
   {STR("/="), 0},
   {STR("÷="), std::make_shared<String>(("/="))},
   {STR("%="), 0},
   {STR("&="), 0},
   {STR("|="), 0},
   {STR("$="), 0},
   {STR("<<="), 0},
   {STR(">>="), 0}
  }).get());
  // comparisonOpList : keywords = ("==", "!=", "<", ">", "<=", ">=");
  this->repository.set(STR("root:TokenData.comparisonOpList"), SharedMap::create(true, {
   {STR("=="), 0},
   {STR("!="), 0},
   {STR("<"), 0},
   {STR(">"), 0},
   {STR("<="), 0},
   {STR(">="), 0}
  }).get());
  // addOpList : keywords = ("+", "-");
  this->repository.set(STR("root:TokenData.addOpList"), SharedMap::create(true, {
    {STR("+"), 0},
    {STR("-"), 0}
  }).get());
  // mulOpList : keywords = ("*", "/", "%");
  this->repository.set(STR("root:TokenData.mulOpList"), SharedMap::create(true, {
    {STR("*"), 0},
    {STR("/"), 0},
    {STR("÷"), std::make_shared<String>(STR("/"))},
    {STR("%"), 0}
  }).get());
  // bitwiseOpList : keywords = ("|", "$", "&", "<<", ">>");
  this->repository.set(STR("root:TokenData.bitwiseOpList"), SharedMap::create(true, {
    {STR("|"), 0},
    {STR("$"), 0},
    {STR("&"), 0},
    {STR("<<"), 0},
    {STR(">>"), 0}
  }).get());
  // logOpList : keywords = ("||", "$$", "&&", "or", "nor", "xor", "xnor", "and", "nand");
  this->repository.set(STR("root:TokenData.logOpList"), SharedMap::create(true, {
    {STR("||"), 0},
    {STR("$$"), 0},
    {STR("&&"), 0},
    {STR("or"), 0},
    {STR("أو"), std::make_shared<String>(STR("or"))},
    {STR("nor"), 0},
    {STR("وليس"), std::make_shared<String>(STR("nor"))},
    {STR("xor"), 0},
    {STR("أو_حصرا"), std::make_shared<String>(STR("xor"))},
    {STR("xnor"), 0},
    {STR("وليس_حصرا"), std::make_shared<String>(STR("xnor"))},
    {STR("and"), 0},
    {STR("و"), std::make_shared<String>(STR("and"))},
    {STR("nand"), 0},
    {STR("أو_ليس"), std::make_shared<String>(STR("nand"))}
  }).get());
  // prefixOpList : keywords = ("++", "--", "+", "-", "!", "!!", "not");
  this->repository.set(STR("root:TokenData.prefixOpList"), SharedMap::create(true, {
    {STR("++"), 0},
    {STR("--"), 0},
    {STR("+"), 0},
    {STR("-"), 0},
    {STR("!"), 0},
    {STR("!!"), 0},
    {STR("not"), 0},
    {STR("ليس"), std::make_shared<String>(STR("not"))},
    {STR("..."), 0}
  }).get());
  // postfixOpList : keywords = ("++", "--");
  this->repository.set(STR("root:TokenData.postfixOpList"), SharedMap::create(true, {
    {STR("++"), 0},
    {STR("--"), 0}
  }).get());
  // linkOpList : keywords = ("->", ".", ".>", "<.");
  this->repository.set(STR("root:TokenData.linkOpList"), SharedMap::create(true, {
   {STR("->"), 0},
   {STR("."), 0},
   {STR(".>"), 0},
   {STR("<."), 0}
  }).get());
  // lowLinkOpList : keywords = ("=>", "..", "..>", "<..");
  this->repository.set(STR("root:TokenData.lowLinkOpList"), SharedMap::create(true, {
   {STR("=>"), 0},
   {STR(".."), 0},
   {STR("..>"), 0},
   {STR("<.>"), 0}
  }).get());
  // lowerLinkOpList : keywords = (":", ":>", "<:");
  this->repository.set(STR("root:TokenData.lowerLinkOpList"), SharedMap::create(true, {
   {STR(":"), 0},
   {STR(":>"), 0},
   {STR("<:"), 0}
  }).get());
  // lowestLinkOpList : keywords = ("::", ::>", "<::", "in");
  this->repository.set(STR("root:TokenData.lowestLinkOpList"), SharedMap::create(true, {
    {STR("::"), 0},
    {STR("::>"), 0},
    {STR("<::"), 0},
    {STR("in"), 0},
    {STR("في"), std::make_shared<String>(STR("in"))}
  }).get());

  this->generateConstTokensForStrings(this->repository.get(STR("root:TokenData")));


  //// Main

  //// Program : StatementList.
  // Program : prod as StatementList(DefaultMain.DefaultStatement);
  if (exprOnly) {
    this->repository.set(STR("root:Program"), SymbolDefinition::create({
      {SymbolDefElement::TERM, ReferenceTerm::create(STR("root:Main.Statement"))},
      {SymbolDefElement::HANDLER, this->parsingHandler}
    }).get());
  } else {
    this->repository.set(STR("root:Program"), SymbolDefinition::create({
      {SymbolDefElement::TERM, ReferenceTerm::create(STR("root:Main.StatementList"))},
      {SymbolDefElement::HANDLER, this->parsingHandler}
    }).get());
  }

  this->repository.set(STR("root:Main"), GrammarModule::create({}).get());

  //// StatementList : Statement { ";" Statement }.
  // StatementList : prod (stmt:production[Statement]) as
  //     stmt*(0, 1) + (lexer.Constant(";") + stmt*(0, 1))*(0, endless);
  this->repository.set(STR("root:Main.StatementList"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::ESPI, 1},
       {TermElement::TERM, SharedList::create({
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
            {TermElement::MAX, std::make_shared<Integer>(1)},
            {TermElement::TERM, ReferenceTerm::create(STR("args:stmt"))}}),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP|TermFlags::ERROR_SYNC_TERM)},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::TERM, SharedList::create({
                  TokenTerm::create(Integer::create(ParsingFlags::ENFORCE_TOKEN_OMIT),
                                    std::make_shared<Integer>(this->constTokenId),
                                    SharedMap::create(0, {{STR(";"),0},{STR("؛"),0}})),
                  MultiplyTerm::create({
                    {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                    {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
                    {TermElement::MAX, std::make_shared<Integer>(1)},
                    {TermElement::TERM, ReferenceTerm::create(STR("args:stmt"))}
                  })
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {
       {STR("stmt"), REF_PARSER->parseQualifier(STR("module:Statement"))}})},
    {SymbolDefElement::HANDLER, Handlers::ScopeParsingHandler<Data::Ast::Scope>::create(1)},
    {SymbolDefElement::FLAGS, Integer::create(ParsingFlags::ENFORCE_PROD_OBJ)}
  }).get());

  //// Statement : (Phrase | Phrase | ...).
  // Statement : @limit[user.parent==self,child.terms=self] prule
  //   prefix self.id, DefaultModifier
  //   as (phrases:list[prule[Phrase]]=(ExpPhrase, CmdPhrase)=>{
  //     alternate (phrases:phrase)->( phrase )
  //   };
  this->repository.set(STR("root:Main.Statement"), SymbolDefinition::create({
   {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM)},
      {TermElement::REF, REF_PARSER->parseQualifier(STR("stack:phrase"))},
      {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:phrases"))},
      {TermElement::TERM, ReferenceTerm::create(STR("stack:phrase"))}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("phrases"), SharedList::create({
         REF_PARSER->parseQualifier(STR("module:CmdPhrase")),
         REF_PARSER->parseQualifier(STR("module:ExpPhrase"))
       })}
    })},
   {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  //// Phrase : { Subject }.
  //  Phrase : prule as (sections:list[map[prd:valid_subject, min:integer, max:integer, pty:integer]])=>{
  //    concat (sections:s)->( @priority(s.pty,0) s.prd*(s.min,s.max) )
  //  };
  this->repository.set(STR("root:Main.Phrase"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::ESPI, 1000},
      {TermElement::REF, REF_PARSER->parseQualifier(STR("stack:subject"))},
      {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:subjects"))},
      {TermElement::TERM, MultiplyTerm::create({
         {TermElement::PRIORITY, REF_PARSER->parseQualifier(STR("stack:subject.pty"))},
         {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
         {TermElement::MIN, REF_PARSER->parseQualifier(STR("stack:subject.min"))},
         {TermElement::MAX, REF_PARSER->parseQualifier(STR("stack:subject.max"))},
         {TermElement::TERM, ReferenceTerm::create(STR("stack:subject.prd"))}
       })}
    })},
   {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // CmdPhrase : prule ref Phrase(sections=((prd=LeadingCmdGroup,min=1,max=1,pty=1)));
  this->repository.set(STR("root:Main.CmdPhrase"), SymbolDefinition::create({
   {SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("module:Phrase"))},
   {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("subjects"), SharedList::create({
         SharedMap::create(false, {
           {STR("prd"), REF_PARSER->parseQualifier(STR("module:LeadingCmdGrp"))},
           {STR("min"), std::make_shared<Integer>(1)},
           {STR("max"), std::make_shared<Integer>(1)},
           {STR("pty"), std::make_shared<Integer>(1)}
         })
       })}
    })},
   {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // ExpPhrase : prule ref Phrase(sections=((prd=root.Expression,min=1,max=1,pty=1)));
  this->repository.set(STR("root:Main.ExpPhrase"), SymbolDefinition::create({
   {SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("module:Phrase"))},
   {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("subjects"), SharedList::create({
         SharedMap::create(false, {
           {STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression"))},
           {STR("min"), std::make_shared<Integer>(1)},
           {STR("max"), std::make_shared<Integer>(1)},
           {STR("pty"), std::make_shared<Integer>(1)}
         })
       })}
    })},
   {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  if (exprOnly) {
    // LeadingCommandGroup
    this->repository.set(STR("root:Main.LeadingCmdGrp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM)},
        {TermElement::REF, REF_PARSER->parseQualifier(STR("stack:cmd"))},
        {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:cmds"))},
        {TermElement::TERM, ReferenceTerm::create(STR("stack:cmd"))}
      })},
     {SymbolDefElement::VARS, SharedMap::create(false, {
        {STR("cmds"), SharedList::create()}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
    }).get());
  } else {
    // LeadingCommandGroup
    this->repository.set(STR("root:Main.LeadingCmdGrp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM)},
        {TermElement::REF, REF_PARSER->parseQualifier(STR("stack:cmd"))},
        {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:cmds"))},
        {TermElement::TERM, ReferenceTerm::create(STR("stack:cmd"))}
      })},
     {SymbolDefElement::VARS, SharedMap::create(false, {
        {STR("cmds"), SharedList::create({
           REF_PARSER->parseQualifier(STR("module:Do")),
           REF_PARSER->parseQualifier(STR("module:Import")),
           REF_PARSER->parseQualifier(STR("module:Def")),
           REF_PARSER->parseQualifier(STR("module:Dump"))
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
    }).get());

    //// Do = "do" + Subject
    this->repository.set(STR("root:Main.Do"), SymbolDefinition::create({
      {SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd"))},
      {SymbolDefElement::VARS, SharedMap::create(false, {
         {STR("kwd"), std::make_shared<String>(STR("do"))},
         {STR("prms"), SharedList::create({
            SharedMap::create(false, {
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:Subject"))},
              {STR("min"), std::make_shared<Integer>(1)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)}
            })
          })}
       })},
      {SymbolDefElement::HANDLER, this->doCommandParsingHandler}
    }).get());

    //// Import = "import" + Subject
    this->repository.set(STR("root:Main.Import"), SymbolDefinition::create({
      {SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd"))},
      {SymbolDefElement::VARS, SharedMap::create(false, {
         {STR("kwd"), SharedMap::create(false, {{STR("import"),0},{STR("اشمل"),0}})},
         {STR("prms"), SharedList::create({
            SharedMap::create(false, {
              {STR("prd"), REF_PARSER->parseQualifier(STR("root:Subject"))},
              {STR("min"), std::make_shared<Integer>(1)},
              {STR("max"), std::make_shared<Integer>(1)},
              {STR("pty"), std::make_shared<Integer>(1)},
              {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP)}
            })
          })}
       })},
      {SymbolDefElement::HANDLER, this->importHandler}
    }).get());

    //// Def = "def" + Subject
    this->repository.set(STR("root:Main.Def"), SymbolDefinition::create({
      { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
      {
        SymbolDefElement::VARS, SharedMap::create(false, {
          { STR("kwd"), SharedMap::create(false, { { STR("def"), 0 }, { STR("عرّف"), 0 } }) },
          {
            STR("prms"), SharedList::create({
              SharedMap::create(false, {
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
      {SymbolDefElement::HANDLER, std::make_shared<Processing::Handlers::DefParsingHandler>()}
    }).get());

    //// dump = "dump" + Subject
    this->repository.set(STR("root:Main.Dump"), SymbolDefinition::create({
      { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
      {
        SymbolDefElement::VARS, Core::Data::SharedMap::create(false, {
          { STR("kwd"), SharedMap::create(false, { { STR("dump"), 0 }, { STR("فرّغ"), 0 } }) },
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
      { SymbolDefElement::HANDLER, this->dumpParsingHandler }
    }).get());
  }

  //// Cmd : keyword {Subject}.
  // Cmd : @limit[user.parent==root.Command] prule
  //   as (kwd:keywords, args:list[map[prd:valid_subject, min:integer, max:integer, pty:integer]])=>{
  //     root.KeywordGroup(kwd) + concat (args:a)->( @priority(a.pty,0) a.prd*(a.min,a.max) )
  //   };
  this->repository.set(STR("root:Cmd"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.Identifier"))),
                            REF_PARSER->parseQualifier(STR("args:kwd"))),
          ConcatTerm::create({
            {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
            {TermElement::REF, REF_PARSER->parseQualifier(STR("stack:p"))},
            {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:prms"))},
            {TermElement::TERM, MultiplyTerm::create({
               {TermElement::PRIORITY, REF_PARSER->parseQualifier(STR("stack:p.pty"))},
               {TermElement::FLAGS, REF_PARSER->parseQualifier(STR("stack:p.flags"))},
               {TermElement::MIN, REF_PARSER->parseQualifier(STR("stack:p.min"))},
               {TermElement::MAX, REF_PARSER->parseQualifier(STR("stack:p.max"))},
               {TermElement::TERM, ReferenceTerm::create(STR("stack:p.prd"))}
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, Integer::create(ParsingFlags::ENFORCE_PROD_OBJ)}
  }).get());

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
  this->repository.set(STR("root:MultiCmd"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::REF, REF_PARSER->parseQualifier(STR("stack:s"))},
      {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:sections"))},
      {TermElement::TERM, MultiplyTerm::create({
        {TermElement::PRIORITY, REF_PARSER->parseQualifier(STR("stack:s.pty"))},
        {TermElement::FLAGS, REF_PARSER->parseQualifier(STR("stack:s.flags"))},
        {TermElement::MIN, REF_PARSER->parseQualifier(STR("stack:s.min"))},
        {TermElement::MAX, REF_PARSER->parseQualifier(STR("stack:s.max"))},
        {TermElement::TERM, ConcatTerm::create({
          {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
          {TermElement::TERM, SharedList::create({
            TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.Identifier"))),
                              REF_PARSER->parseQualifier(STR("stack:s.kwd"))),
            ConcatTerm::create({
              {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
              {TermElement::REF, REF_PARSER->parseQualifier(STR("stack:a"))},
              {TermElement::DATA, REF_PARSER->parseQualifier(STR("stack:s.args"))},
              {TermElement::TERM, MultiplyTerm::create({
                {TermElement::PRIORITY, REF_PARSER->parseQualifier(STR("stack:a.pty"))},
                {TermElement::FLAGS, REF_PARSER->parseQualifier(STR("stack:a.flags"))},
                {TermElement::MIN, REF_PARSER->parseQualifier(STR("stack:a.min"))},
                {TermElement::MAX, REF_PARSER->parseQualifier(STR("stack:a.max"))},
                {TermElement::TERM, ReferenceTerm::create(STR("stack:a.prd"))}
              })}
            })
          })}
        })}
      })}
    })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, Integer::create(ParsingFlags::ENFORCE_PROD_OBJ)}
  }).get());


  // Expression :
  // operand {binaryOp operand}.
  // [unaryOp] operand.
  // operand [unaryOp].
  // operand {FunctionalOp}.
  this->repository.set(
    STR("root:Expression"),
    GrammarModule::create({{STR("@start"), REF_PARSER->parseQualifier(STR("module:Exp"))}}).get()
  );

  // Exp : @single prod as LowestLinkExp + (@priority(in,0) lexer.Constant("\\")*(0,1));
  this->repository.set(STR("root:Expression.Exp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:LowestLinkExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
           {TermElement::MAX, std::make_shared<Integer>(1)},
           {TermElement::TERM, TokenTerm::create(0, this->constTokenId, STR("\\"))}
         })
       })}
    })},
   {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  // LowestLinkExp : @single @prefix(heap.Modifiers.LowestLinkModifierCmd)
  //     prod (enable:integer=endless) as
  //     ConditionalExp + (@priority(in,0) (LowestLinkOp + ConditionalExp)*(0,enable));
  this->repository.set(STR("root:Expression.LowestLinkExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:ConditionalExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
           {TermElement::MAX, REF_PARSER->parseQualifier(STR("args:enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::TERM, SharedList::create({
                 TokenTerm::create(0, std::make_shared<Integer>(this->constTokenId),
                                   REF_PARSER->parseQualifier(STR("root:TokenData.lowestLinkOpList"))),
                 ReferenceTerm::create(STR("module:ConditionalExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
   {SymbolDefElement::HANDLER, std::make_shared<Handlers::InfixParsingHandler<Ast::LinkOperator>>(false)}
  }).get());

  // ConditionalExp : @single @prefix(heap.Modifiers.ConditionalModifierCmd)
  //     prod (enable:integer[0<=n<=1]=1) as
  //     ListExp + (@priority(in,0) (lexer.Constant("?") + ListExp)*(0,enable));
  this->repository.set(STR("root:Expression.ConditionalExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:ListExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
           {TermElement::MAX, REF_PARSER->parseQualifier(STR("args:enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::TERM, SharedList::create({
                 TokenTerm::create(0, std::make_shared<Integer>(this->constTokenId),
                                   SharedMap::create(false, {{STR("?"), 0}, {STR("؟"), 0}})),
                 ReferenceTerm::create(STR("module:ListExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), std::make_shared<Integer>(1)}})},
   {SymbolDefElement::HANDLER, std::make_shared<Handlers::InfixParsingHandler<Ast::ConditionalOperator>>(false)}
  }).get());

  // ListExp : @single @prefix(heap.Modifiers.ListModifierCmd)
  //   prod (enable:integer=1) as
  //     @filter(enable) LowerLinkExp ||
  //                     (LowerLinkExp || lexer.Constant(",") + LowerLinkExp*(0,1)) +
  //                       (@priority(in,0) (lexer.Constant(",") + LowerLinkExp*(0,1))*(0,endless));
  this->repository.set(STR("root:Expression.ListExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:enable"))},
       {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:LowerLinkExp")),
         ConcatTerm::create({
            {TermElement::TERM, SharedList::create({
               AlternateTerm::create({
                 {TermElement::TERM, SharedList::create({
                    ReferenceTerm::create(STR("module:LowerLinkExp")),
                    ConcatTerm::create({
                      {TermElement::FLAGS, Integer::create(ParsingFlags::ENFORCE_LIST_ITEM)},
                      {TermElement::TERM, SharedList::create({
                         TokenTerm::create(Integer::create(ParsingFlags::ENFORCE_TOKEN_OMIT),
                                           std::make_shared<Integer>(this->constTokenId),
                                           SharedMap::create(false, {{STR(","), 0}, {STR("،"), 0}})),
                         MultiplyTerm::create({
                           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                           {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
                           {TermElement::MAX, std::make_shared<Integer>(1)},
                           {TermElement::TERM, ReferenceTerm::create(STR("module:LowerLinkExp"))}
                         })
                       })}
                    })
                  })}
               }),
               MultiplyTerm::create({
                 {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                 {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
                 {TermElement::TERM, ConcatTerm::create({
                    {TermElement::TERM, SharedList::create({
                       TokenTerm::create(Integer::create(ParsingFlags::ENFORCE_TOKEN_OMIT),
                                         std::make_shared<Integer>(this->constTokenId),
                                         SharedMap::create(false, {{STR(","), 0}, {STR("،"), 0}})),
                       MultiplyTerm::create({
                         {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                         {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
                         {TermElement::MAX, std::make_shared<Integer>(1)},
                         {TermElement::TERM, ReferenceTerm::create(STR("module:LowerLinkExp"))}
                       })
                     })}
                  })}
               })
             })}
         })
       })}
    })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), std::make_shared<Integer>(1)}})},
    {SymbolDefElement::HANDLER, std::make_shared<Handlers::ListParsingHandler<Ast::ExpressionList>>(-1, false)}
  }).get());

  // LowerLinkExp : @single @prefix(heap.Modifiers.LowerLinkModifierCmd)
  //     prod (enable:integer=endless) as
  //     AssignmentExp + (@priority(in,0) (LowerLinkOp + AssignmentExp)*(0,enable));
  this->repository.set(STR("root:Expression.LowerLinkExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("module:AssignmentExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
            {TermElement::MAX, REF_PARSER->parseQualifier(STR("args:enable"))},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::TERM, SharedList::create({
                  TokenTerm::create(0, std::make_shared<Integer>(this->constTokenId),
                                    REF_PARSER->parseQualifier(STR("root:TokenData.lowerLinkOpList"))),
                  ReferenceTerm::create(STR("module:AssignmentExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
    {SymbolDefElement::HANDLER, std::make_shared<Handlers::InfixParsingHandler<Ast::LinkOperator>>(false)}
  }).get());

  // AssignmentExp : @single @prefix(heap.Modifiers.AssignmentModifierCmd)
  //     prod (enable:integer=endless) as
  //     LogExp + (@priority(in,0) (AssignmentOp + LogExp)*(0,enable));
  this->repository.set(STR("root:Expression.AssignmentExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("module:LogExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
            {TermElement::MAX, REF_PARSER->parseQualifier(STR("args:enable"))},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::TERM, SharedList::create({
                  TokenTerm::create(0, std::make_shared<Integer>(this->constTokenId),
                                    REF_PARSER->parseQualifier(STR("root:TokenData.assignmentOpList"))),
                  ReferenceTerm::create(STR("module:LogExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
    {SymbolDefElement::HANDLER, std::make_shared<Handlers::InfixParsingHandler<Ast::AssignmentOperator>>(false)}
  }).get());

  // LogExp : @single @prefix(heap.Modifiers.LogModifierCmd)
  //     prod (enable:integer=endless) as
  //     ComparisonExp + (@priority(in,0) (LogOp + ComparisonExp)*(0,enable));
  this->repository.set(STR("root:Expression.LogExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("module:ComparisonExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
             {TermElement::MAX, REF_PARSER->parseQualifier(STR("args:enable"))},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::TERM, SharedList::create({
                   TokenTerm::create(0, std::make_shared<Integer>(this->constTokenId),
                                     REF_PARSER->parseQualifier(STR("root:TokenData.logOpList"))),
                   ReferenceTerm::create(STR("module:ComparisonExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
     {SymbolDefElement::HANDLER, std::make_shared<Handlers::InfixParsingHandler<Ast::LogOperator>>(false)}
   }).get());

  // ComparisonExp : @single @prefix(heap.Modifiers.ComparisonModifierCmd)
  //     prod (enable:integer=endless) as
  //     LowLinkExp + (@priority(in,0) (ComparisonOp + LowLinkExp)*(0,enable));
  this->repository.set(STR("root:Expression.ComparisonExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("module:LowLinkExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
            {TermElement::MAX, REF_PARSER->parseQualifier(STR("args:enable"))},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::TERM, SharedList::create({
                  TokenTerm::create(0, std::make_shared<Integer>(this->constTokenId),
                                    REF_PARSER->parseQualifier(STR("root:TokenData.comparisonOpList"))),
                  ReferenceTerm::create(STR("module:LowLinkExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
    {SymbolDefElement::HANDLER, std::make_shared<Handlers::InfixParsingHandler<Ast::ComparisonOperator>>(false)}
  }).get());

  // LowLinkExp : @single @prefix(heap.Modifiers.LowLinkModifierCmd)
  //     prod (enable:integer=endless) as
  //     AddExp + (@priority(in,0) (LowLinkOp + AddExp)*(0,enable));
  this->repository.set(STR("root:Expression.LowLinkExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("module:AddExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
            {TermElement::MAX, REF_PARSER->parseQualifier(STR("args:enable"))},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::TERM, SharedList::create({
                  TokenTerm::create(0, std::make_shared<Integer>(this->constTokenId),
                                    REF_PARSER->parseQualifier(STR("root:TokenData.lowLinkOpList"))),
                  ReferenceTerm::create(STR("module:AddExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
    {SymbolDefElement::HANDLER, std::make_shared<Handlers::InfixParsingHandler<Ast::LinkOperator>>(false)}
  }).get());

  // AddExp : @single @prefix(heap.Modifiers.AddModifierCmd)
  //     prod (enable:integer=endless) as
  //     MulExp + (@priority(in,0) (AddOp + MulExp)*(0,enable));
  this->repository.set(STR("root:Expression.AddExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:MulExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
           {TermElement::MAX, REF_PARSER->parseQualifier(STR("args:enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::TERM, SharedList::create({
                 TokenTerm::create(0, std::make_shared<Integer>(this->constTokenId),
                                   REF_PARSER->parseQualifier(STR("root:TokenData.addOpList"))),
                 ReferenceTerm::create(STR("module:MulExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
   {SymbolDefElement::HANDLER, std::make_shared<Handlers::InfixParsingHandler<Ast::AdditionOperator>>(false)}
  }).get());

  // MulExp : @single @prefix(heap.Modifiers.MulModifierCmd)
  //     prod (enable:integer=endless) as
  //     BitwiseExp + (@priority(in,0) (MulOp + BitwiseExp)*(0,enable));
  this->repository.set(STR("root:Expression.MulExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:BitwiseExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
           {TermElement::MAX, REF_PARSER->parseQualifier(STR("args:enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::TERM, SharedList::create({
                 TokenTerm::create(0, std::make_shared<Integer>(this->constTokenId),
                                   REF_PARSER->parseQualifier(STR("root:TokenData.mulOpList"))),
                 ReferenceTerm::create(STR("module:BitwiseExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
   {SymbolDefElement::HANDLER, std::make_shared<Handlers::InfixParsingHandler<Ast::MultiplicationOperator>>(false)}
  }).get());

  // BitwiseExp : @single @prefix(heap.Modifiers.BitwiseModifierCmd)
  //     prod (enable:integer=endless) as
  //     UnaryExp + (@priority(in,0) (BitwiseOp + UnaryExp)*(0,enable));
  this->repository.set(STR("root:Expression.BitwiseExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:UnaryExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
           {TermElement::MAX, REF_PARSER->parseQualifier(STR("args:enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::TERM, SharedList::create({
                 TokenTerm::create(0, std::make_shared<Integer>(this->constTokenId),
                                   REF_PARSER->parseQualifier(STR("root:TokenData.bitwiseOpList"))),
                 ReferenceTerm::create(STR("module:UnaryExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
   {SymbolDefElement::HANDLER, std::make_shared<Handlers::InfixParsingHandler<Ast::BitwiseOperator>>(false)}
  }).get());

  // UnaryExp : @single @prefix(heap.Modifiers.UnaryModifierCmd)
  //     prod (enable1:integer[0<=n<=1]=1, enable2:integer[0<=n<=1]=1) as
  //     (@priority(in,0) PrefixOp*(0,enable1)) + FunctionalExp + (@priority(in,0) PostfixOp*(0,enable2));
  this->repository.set(STR("root:Expression.UnaryExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
           {TermElement::MAX, REF_PARSER->parseQualifier(STR("args:enable1"))},
           {TermElement::TERM, ReferenceTerm::create(STR("module:PrefixOp"))}
         }),
         ReferenceTerm::create(STR("module:FunctionalExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
           {TermElement::MAX, REF_PARSER->parseQualifier(STR("args:enable2"))},
           {TermElement::TERM, ReferenceTerm::create(STR("module:PostfixOp"))}
         })
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("enable1"), std::make_shared<Integer>(1)},
      {STR("enable2"), std::make_shared<Integer>(1)}
    })},
   {SymbolDefElement::HANDLER, std::make_shared<Handlers::OutfixParsingHandler<Ast::PrefixOperator, Ast::PostfixOperator>>()}
  }).get());

  // FunctionalExp : @single @prefix(heap.Modifiers.FunctionalModifierCmd)
  //     prod (operand:production[heap.Subject]=heap.Subject, fltr1:filter=null,
  //           fltr2:filter=null, dup:integer=endless, pty1:integer=1, pty2:integer=in) as
  //     @filter(fltr1) @priority(pty1,0)
  //         (operand + (@priority(pty2,0) (@filter(fltr2) ParamPassExp ||
  //                                                       PostfixTildeExp ||
  //                                                       LinkExp(operand))*(0,dup))) ||
  //         PrefixTildeExp + operand;
  this->repository.set(STR("root:Expression.FunctionalExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
        ReferenceTerm::create(STR("args:operand")),
        MultiplyTerm::create({
          {TermElement::PRIORITY, REF_PARSER->parseQualifier(STR("args:pty2"))},
          {TermElement::FLAGS, REF_PARSER->parseQualifier(STR("args:flags"))},
          {TermElement::MAX, REF_PARSER->parseQualifier(STR("args:dup"))},
          {TermElement::TERM, AlternateTerm::create({
            {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:fltr2"))},
            {TermElement::TERM, SharedList::create({
              ReferenceTerm::create(STR("module:LinkExp")),
              ReferenceTerm::create(STR("module:PostfixTildeExp")),
              ReferenceTerm::create(STR("module:ParamPassExp"))
            })}
          })}
        })
      })}
    })},
    {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("flags"), Integer::create(TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_ITEMS_UP)},
      {STR("operand"), REF_PARSER->parseQualifier(STR("root:Subject"))},
      {STR("pty2"), std::make_shared<Integer>(1)},
      {STR("dup"), 0},
      {STR("fltr2"), 0}
    })},
    {SymbolDefElement::HANDLER, std::make_shared<Handlers::ChainOpParsingHandler>()}
  }).get());

  // LinkExp : @single prod (operand:production[heap.Subject]=heap.Subject) as LinkOp + operand;
  this->repository.set(STR("root:Expression.LinkExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, std::make_shared<Integer>(this->constTokenId),
                            REF_PARSER->parseQualifier(STR("root:TokenData.linkOpList"))),
          ReferenceTerm::create(STR("args:operand"))
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{STR("operand"), REF_PARSER->parseQualifier(STR("root:Subject"))}})},
    {SymbolDefElement::HANDLER, std::make_shared<Handlers::CustomParsingHandler>([](Parser *parser, ParserState *state) {
      auto currentList = state->getData().ti_cast_get<Data::Container>();
      auto metadata = ti_cast<Ast::Metadata>(currentList);
      auto token = tio_cast<Ast::Token>(currentList->get(0));
      auto linkOp = Ast::LinkOperator::create({
        { "prodId", metadata->getProdId() },
        { "sourceLocation", metadata->getSourceLocation() }
      });
      linkOp->setType(token->getText());
      linkOp->setSecond(getSharedPtr(currentList->get(1)));
      state->setData(linkOp);
    })}
  }).get());

  //// ParamPassExp : "(" [Expression] ")" | "[" [Expression] "]".
  // ParamPassExp : @single prod (expr:production[Expression||Statement]=heap.Expression, fltr:filter=null) as
  //     @filter(fltr) lexer.Constant("(") + expr*(0,1) + lexer.Constant(")") ||
  //                   lexer.Constant("[") + expr*(0,1) + lexer.Constant("]");
  this->repository.set(STR("root:Expression.ParamPassExp"), SymbolDefinition::create({
  {SymbolDefElement::TERM, AlternateTerm::create({
     {TermElement::FLAGS, Integer::create(ParsingFlags::ENFORCE_ROUTE_OBJ|TermFlags::ONE_ROUTE_TERM)},
     {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:fltr"))},
     {TermElement::TERM, SharedList::create({
        ConcatTerm::create({
          {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
          {TermElement::TERM, SharedList::create({
             TokenTerm::create(0, this->constTokenId, STR("(")),
             MultiplyTerm::create({
               {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
               {TermElement::MAX, std::make_shared<Integer>(1)},
               {TermElement::TERM, ReferenceTerm::create(STR("args:expr"))}
             }),
             TokenTerm::create(0, this->constTokenId, STR(")"))
           })}
        }),
        ConcatTerm::create({
          {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
          {TermElement::TERM, SharedList::create({
             TokenTerm::create(0, this->constTokenId, STR("[")),
             MultiplyTerm::create({
               {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
               {TermElement::MAX, std::make_shared<Integer>(1)},
               {TermElement::TERM, ReferenceTerm::create(STR("args:expr"))}
             }),
             TokenTerm::create(0, this->constTokenId, STR("]"))
           })}
        })
      })}
    })},
    {SymbolDefElement::VARS, SharedMap::create(false, {
       {STR("expr"), REF_PARSER->parseQualifier(STR("root:Expression"))},
       {STR("fltr"), 0}})},
    {SymbolDefElement::HANDLER, std::make_shared<Handlers::CustomParsingHandler>([](Parser *parser, ParserState *state) {
       auto currentRoute = state->getData().tio_cast_get<Ast::Route>();
       auto paramPass = Ast::ParamPass::create({
         { "prodId", currentRoute->getProdId() },
         { "sourceLocation", currentRoute->getSourceLocation() }
       });
       paramPass->setType(currentRoute->getRoute()==0 ? Ast::BracketType::ROUND : Ast::BracketType::SQUARE);
       paramPass->setParam(currentRoute->getData());
       state->setData(paramPass);
    })}
  }).get());

  //// PostfixTildeExp :
  //// "~" keyword {Subject}.
  //// "~(" Expression {Subject} ")".
  //PostfixTildeExp : @single prod (cmd:production[PostfixTildeCmd]=DefaultPostfixTildeCmd) as
  //    lexer.Constant("~") + cmd;
  this->repository.set(STR("root:Expression.PostfixTildeExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         TokenTerm::create(0, this->constTokenId, STR("~")),
         ReferenceTerm::create(STR("args:cmd"))
      })}
    })},
    {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("cmd"), REF_PARSER->parseQualifier(STR("module:DefaultPostfixTildeCmd"))}
    })},
    {SymbolDefElement::HANDLER, std::make_shared<Handlers::CustomParsingHandler>([](Parser *parser, ParserState *state) {
       auto currentList = state->getData().tii_cast_get<Data::Container>();
       auto metadata = ti_cast<Ast::Metadata>(currentList);
       auto token = tio_cast<Ast::Token>(currentList->get(0));
       auto linkOp = Ast::LinkOperator::create({
         { "prodId", metadata->getProdId() },
         { "sourceLocation", metadata->getSourceLocation() }
       });
       linkOp->setType(token->getText());
       linkOp->setSecond(getSharedPtr(currentList->get(1)));
       state->setData(linkOp);
     })}
  }).get());
  //OpenPostfixTildeCmd : @limit[user.parent==PostfixTildeCmd]
  //    prod (expr:production[Expression], args:list[hash[sbj:valid_subject, min:integer,
  //                                                      max:integer, pty:integer]]) as
  //    lexer.Constant("(") + expr + concat (args:a)->( @priority(pty,0) a.sbj*(a.min,a.max) ) +
  //    lexer.Constant(")");
  this->repository.set(STR("root:Expression.OpenPostfixTildeCmd"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, this->constTokenId, STR("(")),
          ReferenceTerm::create(STR("args:expr")),
          ConcatTerm::create({
            {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
            {TermElement::REF, REF_PARSER->parseQualifier(STR("stack:p"))},
            {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:prms"))},
            {TermElement::TERM, MultiplyTerm::create({
               {TermElement::PRIORITY, REF_PARSER->parseQualifier(STR("stack:p.pty"))},
               {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
               {TermElement::MIN, REF_PARSER->parseQualifier(STR("stack:p.min"))},
               {TermElement::MAX, REF_PARSER->parseQualifier(STR("stack:p.max"))},
               {TermElement::TERM, ReferenceTerm::create(STR("stack:p.prd"))}
             })}
          }),
          TokenTerm::create(0, this->constTokenId, STR(")"))
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {
       {STR("expr"), REF_PARSER->parseQualifier(STR("root:Expression"))},
       {STR("prms"), SharedList::create({SharedMap::create(false, {
                                     {STR("pty"), std::make_shared<Integer>(1)},
                                     {STR("min"), 0},
                                     {STR("max"), 0},
                                     {STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression"))}
                                   })})}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());
  //DefaultPostfixTildeCmd=>PostfixTildeCmd : prod_group;
  this->repository.set(STR("root:Expression.DefaultPostfixTildeCmd"), SymbolDefinition::create({
   {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM)},
      {TermElement::REF, REF_PARSER->parseQualifier(STR("stack:cmd"))},
      {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:cmds"))},
      {TermElement::TERM, ReferenceTerm::create(STR("stack:cmd"))}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("cmds"), SharedList::create({REF_PARSER->parseQualifier(STR("module:OpenPostfixTildeCmd"))})}
    })},
   {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  //// Operators :
  // PrefixOp : prod ref heap.ConstantKeywordGroup(heap.TokenData.prefixOpList);
  this->repository.set(STR("root:Expression.PrefixOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, TokenTerm::create(0, std::make_shared<Integer>(this->constTokenId),
                                               REF_PARSER->parseQualifier(STR("root:TokenData.prefixOpList")))},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());
  // PostfixOp : prod ref heap.ConstantKeywordGroup(heap.TokenData.postfixOpList);
  this->repository.set(STR("root:Expression.PostfixOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, TokenTerm::create(0, std::make_shared<Integer>(this->constTokenId),
                                               REF_PARSER->parseQualifier(STR("root:TokenData.postfixOpList")))},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());


  // Subject : Parameter | Command | Expression | Statement | Set.
  this->repository.set(
    STR("root:Subject"),
    GrammarModule::create({{STR("@start"), REF_PARSER->parseQualifier(STR("module:Subject1"))}}).get()
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
  this->repository.set(STR("root:Subject.Subject1"), SymbolDefinition::create({
   {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM)},
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM)},
           {TermElement::REF, REF_PARSER->parseQualifier(STR("stack:s"))},
           {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:sbj1"))},
           {TermElement::TERM, ReferenceTerm::create(STR("stack:s"))}
         }),
         ConcatTerm::create({
           {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
           {TermElement::TERM, SharedList::create({
              TokenTerm::create(0, this->constTokenId, STR("(")),
              MultiplyTerm::create({
                {TermElement::MIN, REF_PARSER->parseQualifier(STR("args:frc2"))},
                {TermElement::MAX, std::make_shared<Integer>(1)},
                {TermElement::TERM, AlternateTerm::create({
                   {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM)},
                   {TermElement::REF, REF_PARSER->parseQualifier(STR("stack:s2"))},
                   {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:sbj2"))},
                   {TermElement::TERM, ReferenceTerm::create(STR("stack:s2"))}
                 })}
              }),
              TokenTerm::create(0, this->constTokenId, STR(")"))
            })}
         }),
         ConcatTerm::create({
           {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
           {TermElement::TERM, SharedList::create({
              TokenTerm::create(0, this->constTokenId, STR("[")),
              MultiplyTerm::create({
                {TermElement::MIN, REF_PARSER->parseQualifier(STR("args:frc3"))},
                {TermElement::MAX, std::make_shared<Integer>(1)},
                {TermElement::TERM, AlternateTerm::create({
                   {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM)},
                   {TermElement::REF, REF_PARSER->parseQualifier(STR("stack:s3"))},
                   {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:sbj3"))},
                   {TermElement::TERM, ReferenceTerm::create(STR("stack:s3"))}
                 })}
              }),
              TokenTerm::create(0, this->constTokenId, STR("]"))
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("sbj1"), SharedList::create({
         REF_PARSER->parseQualifier(STR("module:SubjectCmdGrp")),
         REF_PARSER->parseQualifier(STR("module:Parameter")),
         REF_PARSER->parseQualifier(STR("root:Set"))
       })},
      {STR("sbj2"), SharedList::create({REF_PARSER->parseQualifier(STR("root:Expression"))})},
      {STR("sbj3"), SharedList::create({REF_PARSER->parseQualifier(STR("root:Expression"))})},
      {STR("frc2"), 0},
      {STR("frc3"), 0}
    })},
   {SymbolDefElement::HANDLER, std::make_shared<Handlers::SubjectParsingHandler>()}
  }).get());

  // Subject2 : @limit[user.owner==Subject] prod (sbj:production[Parameter||Command||Expression||Statement||Set],
  //                                             fltr:filter, frc:integer) as
  //     @filter(fltr) sbj ||
  //                   lexer.Constant("(") + sbj*(frc,1) + lexer.Constant(")") ||
  //                   lexer.Constant("[") + sbj*(frc,1) + lexer.Constant("]");
  this->repository.set(STR("root:Subject.Subject2"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM)},
        {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:fltr"))},
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("args:sbj")),
           ConcatTerm::create({
             {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
             {TermElement::TERM, SharedList::create({
                TokenTerm::create(0, this->constTokenId, STR("(")),
                MultiplyTerm::create({
                  {TermElement::MIN, REF_PARSER->parseQualifier(STR("args:frc"))},
                  {TermElement::MAX, std::make_shared<Integer>(1)},
                  {TermElement::TERM, ReferenceTerm::create(STR("args:sbj"))}
                }),
                TokenTerm::create(0, this->constTokenId, STR(")"))
              })}
           }),
           ConcatTerm::create({
             {TermElement::FLAGS, Integer::create(ParsingFlags::PASS_ITEMS_UP)},
             {TermElement::TERM, SharedList::create({
                TokenTerm::create(0, this->constTokenId, STR("[")),
                MultiplyTerm::create({
                  {TermElement::MIN, REF_PARSER->parseQualifier(STR("args:frc"))},
                  {TermElement::MAX, std::make_shared<Integer>(1)},
                  {TermElement::TERM, ReferenceTerm::create(STR("args:sbj"))}
                }),
                TokenTerm::create(0, this->constTokenId, STR("]"))
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, std::make_shared<Handlers::SubjectParsingHandler>()}
   }).get());

  // SubjectCommandGroup
  this->repository.set(STR("root:Subject.SubjectCmdGrp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM)},
       {TermElement::REF, REF_PARSER->parseQualifier(STR("stack:cmd"))},
       {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:cmds"))},
       {TermElement::TERM, ReferenceTerm::create(STR("stack:cmd"))}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{ STR("cmds"), SharedList::create({}) }} )},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());

  //// Parameter: Identifier | Literal.
  // Parameter : @single @prefix(heap.Modifiers.ParameterModifierCmd)
  //     prod (fltr:filter=null, cnsts:keywords=null) as
  //     @filter(fltr) lexer.Identifier || Literal || heap.ConstantKeywordGroup(cnsts);
  this->repository.set(STR("root:Subject.Parameter"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM)},
       {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:fltr"))},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.Identifier")))),
          ReferenceTerm::create(STR("module:Literal"))
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{ STR("fltr"), 0 }} )},
    {SymbolDefElement::HANDLER, std::make_shared<Handlers::CustomParsingHandler>([](Parser *parser, ParserState *state) {
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
       newObj->setSourceLocation(current->getSourceLocation());
       state->setData(newObj);
    })}
  }).get());

  // Literal:
  // Literal : @single prod (fltr:filter=null) as
  //     @filter(fltr) lexer.IntLiteral || lexer.FloatLiteral || lexer.CharLiteral || lexer.StringLiteral ||
  //                   lexer.CustomLiteral;
  this->repository.set(STR("root:Subject.Literal"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM)},
       {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:fltr"))},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.IntLiteral")))),
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.FloatLiteral")))),
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.CharLiteral")))),
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.StringLiteral"))))
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{ STR("fltr"), 0 }} )},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());


  // Set : "{" StatementList "}".
  // Set : @limit[child.terms==self,user.parent==self] @prefix(heap.Modifiers.DefaultModifierCmd)
  //     prod (stmt:production[Statement], ovrd:grammar_override) as
  //     lexer.Constant("{") + @override(ovrd) StatementList(stmt) + lexer.Constant("}");
  this->repository.set(STR("root:Set"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::ESPI, 1},
      {TermElement::TERM, SharedList::create({
        TokenTerm::create(0, this->constTokenId, STR("{")),
        ReferenceTerm::create(STR("args:stmt")),
        TokenTerm::create(0, this->constTokenId, STR("}"))
      })}
    })},
    {SymbolDefElement::VARS, SharedMap::create(false, {
       {STR("stmt"), REF_PARSER->parseQualifier(STR("root:Main.StatementList"))}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());


  // Modifiers
  this->repository.set(STR("root:Modifier"), GrammarModule::create({}).get());
  // Modifier.Subject
  this->repository.set(STR("root:Modifier.Subject"), GrammarModule::create({
    { STR("@parent"), REF_PARSER->parseQualifier(STR("root:Subject")) }
  }).get());
  this->repository.set(STR("root:Modifier.Subject.SubjectCmdGrp"), SymbolDefinition::create({
    {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:SubjectCmdGrp")) },
    {SymbolDefElement::VARS, SharedMap::create(false, {{ STR("cmds"), SharedList::create({}) }} )},
  }).get());
  // Modifier.Expression
  this->repository.set(STR("root:Modifier.Expression"), GrammarModule::create({
    { STR("@start"), REF_PARSER->parseQualifier(STR("module:FunctionalExp")) },
    { STR("@parent"), REF_PARSER->parseQualifier(STR("root:Expression")) }
  }).get());
  this->repository.set(STR("root:Modifier.Expression.FunctionalExp"), SymbolDefinition::create({
    {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:FunctionalExp")) },
    {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("flags"), Integer::create(ParsingFlags::PASS_ITEMS_UP | TermFlags::ONE_ROUTE_TERM)},
      {STR("operand"), REF_PARSER->parseQualifier(STR("root:Modifier.Subject"))},
      {STR("pty2"), std::make_shared<Integer>(1)},
      {STR("dup"), Integer::create(1)},
      {STR("fltr2"), Integer::create(2)}
     })}
  }).get());
  // Modifier.CmdGroup
  this->repository.set(STR("root:Modifier.CmdGroup"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM)},
      {TermElement::REF, REF_PARSER->parseQualifier(STR("stack:cmd"))},
      {TermElement::DATA, REF_PARSER->parseQualifier(STR("args:cmds"))},
      {TermElement::TERM, ReferenceTerm::create(STR("stack:cmd"))}
    })},
    {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("cmds"), SharedList::create()}
    })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }).get());
  // Modifier.Phrase
  this->repository.set(STR("root:Modifier.Phrase"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::FLAGS, Integer::create(TermFlags::ONE_ROUTE_TERM)},
      {TermElement::TERM, SharedList::create({
        ReferenceTerm::create(STR("module:CmdGroup")),
        ReferenceTerm::create(STR("module:Expression"))
      })}
    })}
  }).get());
  // Modifier.LeadingModifier
  this->repository.set(STR("root:Modifier.LeadingModifier"), SymbolDefinition::create({
    {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("module:Phrase")) },
    {SymbolDefElement::HANDLER, this->leadingModifierHandler}
  }).get());
  // Modifier.TrailingModifier
  this->repository.set(STR("root:Modifier.TrailingModifier"), SymbolDefinition::create({
    {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("module:Phrase")) },
    {SymbolDefElement::HANDLER, this->trailingModifierHandler}
  }).get());

  // Modifiers parsing dimensions.
  this->repository.set(STR("root:LeadingModifierDim"), ParsingDimension::create(
    std::make_shared<String>(STR("@")),
    REF_PARSER->parseQualifier(STR("root:Modifier.LeadingModifier"))
  ).get());
  this->repository.set(STR("root:TrailingModifierDim"), ParsingDimension::create(
    std::make_shared<String>(STR("@<")),
    REF_PARSER->parseQualifier(STR("root:Modifier.TrailingModifier"))
  ).get());
}


SharedPtr<Data::SymbolDefinition> GrammarPlant::createConstTokenDef(Char const *text)
{
  if (SBSTR(text) == STR("@") || SBSTR(text) == STR("@<")) {
    return SymbolDefinition::create({
          {SymbolDefElement::TERM, ConstTerm::create(0, text)},
          {SymbolDefElement::FLAGS, Integer::create(SymbolFlags::ROOT_TOKEN)}});
  } else {
    return SymbolDefinition::create({{SymbolDefElement::TERM, ConstTerm::create(0, text)},
                                     {SymbolDefElement::FLAGS, Integer::create(SymbolFlags::ROOT_TOKEN)},
                                     {SymbolDefElement::HANDLER, this->constTokenHandler}});
  }
}

} } // namespace
