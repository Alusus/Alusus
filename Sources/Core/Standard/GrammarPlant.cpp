/**
 * @file Core/Standard/GrammarPlant.cpp
 * Contains the implementation of class Core::Standard::GrammarPlant.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
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

//==============================================================================
// Member Functions

/**
 * Creates the entire list of definitions for productions, tokens, and char
 * groups for the Core's grammar. This function will give you the complete
 * grammar definitions with all the required handlers.
 */
void GrammarPlant::createGrammar(RootManager *root)
{
  this->constTokenPrefix = STR("LexerDefs");

  // Instantiate parsing handlers.
  this->parsingHandler = std::make_shared<GenericParsingHandler>();
  this->importHandler = std::make_shared<ImportParsingHandler>(root);

  // Create lexer definitions.
  this->repository.setSharedValue(STR("root:LexerDefs"), GrammarModule::create({}));
  this->createCharGroupDefinitions();
  this->createTokenDefinitions();

  // Create parser definitions.
  this->createProductionDefinitions();

  // Set start production and lexer module.
  GrammarModule *rootModule = this->repository.getRoot().get();
  rootModule->setStartRef(ReferenceParser::parseQualifier(STR("module:Program")));
  rootModule->setLexerModuleRef(ReferenceParser::parseQualifier(STR("root:LexerDefs")));

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
  this->repository.setSharedValue(STR("root:LexerDefs.BinDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(CHR('0'), CHR('1'))));

  // OctDigit : char '0'..'7';
  this->repository.setSharedValue(STR("root:LexerDefs.OctDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(CHR('0'), CHR('7'))));

  // DecDigit : char '0'..'9';
  this->repository.setSharedValue(STR("root:LexerDefs.DecDigit"), CharGroupDefinition::create(
    SequenceCharGroupUnit::create(CHR('0'), CHR('9'))));

  // HexDigit : char '0'..'9', 'a'..'f', 'A'..'F';
  this->repository.setSharedValue(STR("root:LexerDefs.HexDigit"), CharGroupDefinition::create(
    UnionCharGroupUnit::create({
      SequenceCharGroupUnit::create(CHR('0'), CHR('9')),
      SequenceCharGroupUnit::create(CHR('a'), CHR('f')),
      SequenceCharGroupUnit::create(CHR('A'), CHR('F'))
    })));

  // Letter : char 'a'..'z', 'A'..'Z', '_';
  this->repository.setSharedValue(STR("root:LexerDefs.Letter"), CharGroupDefinition::create(
    UnionCharGroupUnit::create({
      SequenceCharGroupUnit::create(CHR('a'), CHR('z')),
      SequenceCharGroupUnit::create(CHR('A'), CHR('Z')),
      SequenceCharGroupUnit::create(CHR('_'), CHR('_'))
    })));

  // AnyCharNoEs : char ^('\\');
  this->repository.setSharedValue(STR("root:LexerDefs.AnyCharNoEs"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      SequenceCharGroupUnit::create(CHR('\\'), CHR('\\')))));

  // AnyCharNoEsOrSingleQuote : char ^("\\'");
  this->repository.setSharedValue(STR("root:LexerDefs.AnyCharNoEsOrSingleQuote"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      RandomCharGroupUnit::create(STR("\\'")))));

  // AnyCharNoEsOrDoubleQuote : char ^("\\\"");
  this->repository.setSharedValue(STR("root:LexerDefs.AnyCharNoEsOrDoubleQuote"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      RandomCharGroupUnit::create(STR("\\\"")))));

  // AnyCharNoReturn = ^('\\');
  this->repository.setSharedValue(STR("root:LexerDefs.AnyCharNoReturn"), CharGroupDefinition::create(
    InvertCharGroupUnit::create(
      SequenceCharGroupUnit::create(CHR('\n'), CHR('\n')))));

  // Spacing : char " \n\r\t";
  this->repository.setSharedValue(STR("root:LexerDefs.Spacing"), CharGroupDefinition::create(
    RandomCharGroupUnit::create(STR(" \r\n\t"))));
}


/**
 * Create a TokenDefinitionList and add all the required definitions for
 * the Core's grammar.
 */
void GrammarPlant::createTokenDefinitions()
{
  // Identifier : trule as { Letter + (Letter || DecDigit)*(0,endless) };
  this->repository.setSharedValue(STR("root:LexerDefs.Identifier"), SymbolDefinition::create({
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
   {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN}
  }));

  // IntLiteral : trule as {
  //   (DecIntLiteral || BinIntLiteral || OctIntLiteral || HexIntLiteral) +
  //   ("u" || "U")*(0,1) + (("i" || "I") + DecIntLiteral)*(0,1)
  // };
  this->repository.setSharedValue(STR("root:LexerDefs.IntLiteral"), SymbolDefinition::create({
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
           {TermElement::MAX, Integer::create(1)},
           {TermElement::TERM, AlternateTerm::create({
              {TermElement::TERM, SharedList::create({
                 ConstTerm::create(0, STR("u")),
                 ConstTerm::create(0, STR("U"))
              })}
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MAX, Integer::create(1)},
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
   {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN}
  }));

  // @inner DecIntLiteral : trule as { DecDigit*(1,endless) };
  this->repository.setSharedValue(STR("root:LexerDefs.DecIntLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, MultiplyTerm::create({
      {TermElement::MIN, Integer::create(1)},
      {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
    })}
  }));

  // @inner BinIntLiteral : trule as { ("0b" || "0B") + BinDigit*(1,endless) };
  this->repository.setSharedValue(STR("root:LexerDefs.BinIntLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("0b")),
              ConstTerm::create(0, STR("0B"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MIN, Integer::create(1)},
           {TermElement::TERM, CharGroupTerm::create(STR("module:BinDigit"))}
         })
       })}
    })}
  }));

  // @inner OctIntLiteral : trule as { ("0o" || "0O") + OctDigit*(1,endless) };
  this->repository.setSharedValue(STR("root:LexerDefs.OctIntLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("0o")),
              ConstTerm::create(0, STR("0O"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MIN, Integer::create(1)},
           {TermElement::TERM, CharGroupTerm::create(STR("module:OctDigit"))}
         })
       })}
    })}
  }));

  // @inner HexIntLiteral : trule as { ("0h" || "0H") + HexDigit*(1,endless) };
  this->repository.setSharedValue(STR("root:LexerDefs.HexIntLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("0h")),
              ConstTerm::create(0, STR("0H"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MIN, Integer::create(1)},
           {TermElement::TERM, CharGroupTerm::create(STR("module:HexDigit"))}
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
  this->repository.setSharedValue(STR("root:LexerDefs.FloatLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::TERM, SharedList::create({
         ConcatTerm::create({
           {TermElement::TERM, SharedList::create({
              MultiplyTerm::create({
                {TermElement::MIN, Integer::create(1)},
                {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
              }),
              ReferenceTerm::create(STR("module:FloatPostfix"))
           })}
         }),
         ConcatTerm::create({
           {TermElement::TERM, SharedList::create({
              MultiplyTerm::create({
                {TermElement::MIN, Integer::create(1)},
                {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
              }),
              ReferenceTerm::create(STR("module:FloatExponent")),
              MultiplyTerm::create({
                {TermElement::MAX, Integer::create(1)},
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
                {TermElement::MIN, Integer::create(1)},
                {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
              }),
              MultiplyTerm::create({
                {TermElement::MAX, Integer::create(1)},
                {TermElement::TERM, ReferenceTerm::create(STR("module:FloatExponent"))}
              }),
              MultiplyTerm::create({
                {TermElement::MAX, Integer::create(1)},
                {TermElement::TERM, ReferenceTerm::create(STR("module:FloatPostfix"))}
              })
           })}
         })
       })}
    })},
   {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN}
  }));

  // @inner FloatExponent : trule as { ("e" || "E") + ("+" || "-")*(0,1) + DecDigit*(1,endless) };
  this->repository.setSharedValue(STR("root:LexerDefs.FloatExponent"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("e")),
              ConstTerm::create(0, STR("E"))
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MAX, Integer::create(1)},
           {TermElement::TERM, AlternateTerm::create({
              {TermElement::TERM, SharedList::create({
                 ConstTerm::create(0, STR("+")),
                 ConstTerm::create(0, STR("-"))
              })}
           })}
         }),
         MultiplyTerm::create({
           {TermElement::MIN, Integer::create(1)},
           {TermElement::TERM, CharGroupTerm::create(STR("module:DecDigit"))}
         })
       })}
    })}
  }));

  // @inner FloatPostfix : trule as { ("f" || "F") + DecDigit*(0,endless) };
  this->repository.setSharedValue(STR("root:LexerDefs.FloatPostfix"), SymbolDefinition::create({
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
  }));

  // CharLiteral : trule as { "'" + EsCharWithDoubleQuote + "'" + CharCodePostfix*(0,1) };
  // TODO: Add the char_code_postfix part
  this->repository.setSharedValue(STR("root:LexerDefs.CharLiteral"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ConstTerm::create(0, STR("'")),
         ReferenceTerm::create(STR("module:EsCharWithDoubleQuote")),
         ConstTerm::create(0, STR("'"))
       })}
    })},
    {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN}
  }));

  // StringLiteral : trule as {
  //   StringLiteralPart + (Spacing*(0,endless) + StringLiteralPart)*(0,endless) +
  //   CharCodePostfix*(0,1)
  // };
  // TODO: Add the CharCodePostfix part
  this->repository.setSharedValue(STR("root:LexerDefs.StringLiteral"), SymbolDefinition::create({
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
    {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN}
  }));

  // @inner StringLiteralPart : trule as { "\"" + EsCharWithSingleQuote*(0,endless) + "\"" };
  this->repository.setSharedValue(STR("root:LexerDefs.StringLiteralPart"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ConstTerm::create(0, STR("\"")),
         MultiplyTerm::create({
           {TermElement::TERM, ReferenceTerm::create(STR("module:EsCharWithSingleQuote"))}
         }),
         ConstTerm::create(0, STR("\""))
       })}
    })}
  }));

  // @inner EsCharWithSingleQuote : trule as {
  //   AnyCharNoEsOrDoubleQuote || EsSequence ||
  //   alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the heap.escape_sequences part
  this->repository.setSharedValue(STR("root:LexerDefs.EsCharWithSingleQuote"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::TERM, SharedList::create({
         CharGroupTerm::create(STR("module:AnyCharNoEsOrDoubleQuote")),
         ReferenceTerm::create(STR("module:EsSequence"))
       })}
    })}
  }));

  // @inner EsCharWithDoubleQuote : trule as {
  //   AnyCharNoEsOrSingleQuote || EsSequence ||
  //   alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the root.TokenData.escapeSequences part
  this->repository.setSharedValue(STR("root:LexerDefs.EsCharWithDoubleQuote"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::TERM, SharedList::create({
         CharGroupTerm::create(STR("module:AnyCharNoEsOrSingleQuote")),
         ReferenceTerm::create(STR("module:EsSequence"))
       })}
    })}
  }));

  // @inner EsCharWithQuotes : trule as {
  //   AnyCharNoEs || EsSequence || alternate (root.TokenData.escapeSequences:es)->( es )
  // };
  // TODO: Add the heap.escape_sequences part
  this->repository.setSharedValue(STR("root:LexerDefs.EsCharWithQuotes"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::TERM, SharedList::create({
         CharGroupTerm::create(STR("module:AnyCharNoEs")),
         ReferenceTerm::create(STR("module:EsSequence"))
       })}
    })}
  }));

  // @inner EsSequence : trule as {
  //   '\\' + ('c' + HexDigit*(2,2) ||
  //           'u' + HexDigit*(4,4) ||
  //           'w' + HexDigit*(8,8) ||
  //           'n' || 't' || 'r')
  // };
  this->repository.setSharedValue(STR("root:LexerDefs.EsSequence"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ConstTerm::create(0, STR("\\")),
         AlternateTerm::create({
           {TermElement::TERM, SharedList::create({
              ConstTerm::create(0, STR("n")),
              ConstTerm::create(0, STR("t")),
              ConstTerm::create(0, STR("r")),
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
  }));

  //// IGNORED TOKENS

  // ignore { Spacing*(1,endless) };
  this->repository.setSharedValue(STR("root:LexerDefs.IgnoredSpaces"), SymbolDefinition::create({
    {SymbolDefElement::TERM, MultiplyTerm::create({
      {TermElement::MIN, Integer::create(1)},
      {TermElement::TERM, CharGroupTerm::create(STR("module:Spacing"))}
    })},
    {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN|SymbolFlags::IGNORED_TOKEN}
  }));

  // @minimum ignore { "//" + any*(0,endless) + "\n" }
  // For now this is implemented as:
  // ignore { "//" + AnyCharNoReturn*(0,endless) + "\n" }
  // because the lexer still doesn't support the @minimum modifier.
  this->repository.setSharedValue(STR("root:LexerDefs.LineComment"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::TERM, SharedList::create({
         ConstTerm::create(0, STR("//")),
         MultiplyTerm::create({
           {TermElement::TERM, CharGroupTerm::create(STR("module:AnyCharNoReturn"))}
         }),
         ConstTerm::create(0, STR("\n"))
       })}
    })},
    {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN|SymbolFlags::IGNORED_TOKEN}
  }));
}


/**
 * Create a Production_Definition_List and add all the required definitions for
 * the Core's grammar. The created list will have assigned parsing handlers.
 */
void GrammarPlant::createProductionDefinitions()
{
  // TODO: Replace the generic parsing handler for the root with the appropriate one.

  //// TokenData module.

  this->repository.setSharedValue(STR("root:TokenData"), GrammarModule::create({}));
  // assignmentOpList : keywords := (":=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "$=", "^:=", "<<=", ">>=");
  this->repository.setSharedValue(STR("root:TokenData.assignmentOpList"), SharedMap::create(true, {
   {STR(":="), 0},
   {STR("+="), 0},
   {STR("-="), 0},
   {STR("*="), 0},
   {STR("/="), 0},
   {STR("%="), 0},
   {STR("&="), 0},
   {STR("|="), 0},
   {STR("$="), 0},
   {STR("^:="), 0},
   {STR("<<="), 0},
   {STR(">>="), 0}
  }));
  // comparisonOpList : keywords := ("=", "^=", "==", "^==", "<", ">", "<=", ">=");
  this->repository.setSharedValue(STR("root:TokenData.comparisonOpList"), SharedMap::create(true, {
   {STR("="), 0},
   {STR("^="), 0},
   {STR("=="), 0},
   {STR("^=="), 0},
   {STR("<"), 0},
   {STR(">"), 0},
   {STR("<="), 0},
   {STR(">="), 0}
  }));
  // addOpList : keywords := ("+", "-");
  this->repository.setSharedValue(STR("root:TokenData.addOpList"), SharedMap::create(true, {
    {STR("+"), 0},
    {STR("-"), 0}
  }));
  // mulOpList : keywords := ("*", "/", "%");
  this->repository.setSharedValue(STR("root:TokenData.mulOpList"), SharedMap::create(true, {
    {STR("*"), 0},
    {STR("/"), 0},
    {STR("%"), 0}
  }));
  // bitwiseOpList : keywords := ("|", "^|", "$", "^$", "&", "^&", "<<", ">>");
  this->repository.setSharedValue(STR("root:TokenData.bitwiseOpList"), SharedMap::create(true, {
    {STR("|"), 0},
    {STR("^|"), 0},
    {STR("$"), 0},
    {STR("^$"), 0},
    {STR("&"), 0},
    {STR("^&"), 0},
    {STR("<<"), 0},
    {STR(">>"), 0}
  }));
  // logOpList : keywords := ("||", "^||", "$$", "^$$", "&&", "^&&", "or", "nor", "xor", "xnor", "and", "nand");
  this->repository.setSharedValue(STR("root:TokenData.logOpList"), SharedMap::create(true, {
    {STR("||"), 0},
    {STR("^||"), 0},
    {STR("$$"), 0},
    {STR("^$$"), 0},
    {STR("&&"), 0},
    {STR("^&&"), 0},
    {STR("or"), 0},
    {STR("nor"), 0},
    {STR("xor"), 0},
    {STR("xnor"), 0},
    {STR("and"), 0},
    {STR("nand"), 0}
  }));
  // prefixOpList : keywords := ("++", "--", "+", "-", "^", "^^", "not");
  this->repository.setSharedValue(STR("root:TokenData.prefixOpList"), SharedMap::create(true, {
   {STR("++"), 0},
   {STR("--"), 0},
   {STR("+"), 0},
   {STR("-"), 0},
   {STR("^"), 0},
   {STR("^^"), 0},
   {STR("not"), 0}
  }));
  // postfixOpList : keywords := ("++", "--");
  this->repository.setSharedValue(STR("root:TokenData.postfixOpList"), SharedMap::create(true, {
    {STR("++"), 0},
    {STR("--"), 0}
  }));
  // linkOpList : keywords := ("->", ".", ".>", "<.");
  this->repository.setSharedValue(STR("root:TokenData.linkOpList"), SharedMap::create(true, {
   {STR("->"), 0},
   {STR("."), 0},
   {STR(".>"), 0},
   {STR("<."), 0}
  }));
  // lowLinkOpList : keywords := ("=>", "..", "..>", "<..");
  this->repository.setSharedValue(STR("root:TokenData.lowLinkOpList"), SharedMap::create(true, {
   {STR("=>"), 0},
   {STR(".."), 0},
   {STR("..>"), 0},
   {STR("<.>"), 0}
  }));
  // lowerLinkOpList : keywords := (":", ":>", "<:");
  this->repository.setSharedValue(STR("root:TokenData.lowerLinkOpList"), SharedMap::create(true, {
   {STR(":"), 0},
   {STR(":>"), 0},
   {STR("<:"), 0}
  }));
  // lowestLinkOpList : keywords := ("::", ::>", "<::", "in");
  this->repository.setSharedValue(STR("root:TokenData.lowestLinkOpList"), SharedMap::create(true, {
    {STR("::"), 0},
    {STR("::>"), 0},
    {STR("<::"), 0},
    {STR("in"), 0}
  }));

  this->generateConstTokensForStrings(this->repository.getPlainValue(STR("root:TokenData")));


  //// Main

  //// Program : StatementList.
  // Program : prod as StatementList(DefaultMain.DefaultStatement);
  this->repository.setSharedValue(STR("root:Program"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ReferenceTerm::create(STR("root:Main.StatementList"))},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  this->repository.setSharedValue(STR("root:Main"), GrammarModule::create({}));

  //// StatementList : Statement { ";" Statement }.
  // StatementList : prod (stmt:production[Statement]) as
  //     stmt*(0, 1) + (lexer.Constant(";") + stmt*(0, 1))*(0, endless);
  this->repository.setSharedValue(STR("root:Main.StatementList"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::ESPI, 1},
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::MAX, std::make_shared<Integer>(1)},
            {TermElement::TERM, ReferenceTerm::create(STR("args:stmt"))}}),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ERROR_SYNC_TERM},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
               {TermElement::TERM, SharedList::create({
                  TokenTerm::create(ParsingFlags::OMISSIBLE, STR(";")),
                  MultiplyTerm::create({
                    {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                    {TermElement::FLAGS, ParsingFlags::PASS_UP},
                    {TermElement::MAX, std::make_shared<Integer>(1)},
                    {TermElement::TERM, ReferenceTerm::create(STR("args:stmt"))}
                  })
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {
       {STR("stmt"), ReferenceParser::parseQualifier(STR("module:Statement"))}})},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  //// Statement : { Phrase }.
  // Statement : @prefix(heap.Modifiers.(DefaultModifierCmd|StmtModifierCmd))
  //             @limit[user.parent=self,child.terms=self]
  //     prod (phrases:list[hash[prd:prod[Phrase], min:integer, max:integer, pty:integer]]) as
  //         concat(phrases:phrase)->( @priority(phrase.pty,0) phrase.prd*(phrase.min, phrase.max) );
  this->repository.setSharedValue(STR("root:Main.Statement"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::ESPI, 1000},
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::REF, ReferenceParser::parseQualifier(STR("stack:phrase"))},
      {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:phrases"))},
      {TermElement::TERM, MultiplyTerm::create({
         {TermElement::PRIORITY, ReferenceParser::parseQualifier(STR("stack:phrase.pty"))},
         {TermElement::FLAGS, ParsingFlags::PASS_UP},
         {TermElement::MIN, ReferenceParser::parseQualifier(STR("stack:phrase.min"))},
         {TermElement::MAX, ReferenceParser::parseQualifier(STR("stack:phrase.max"))},
         {TermElement::TERM, ReferenceTerm::create(STR("stack:phrase.prd"))}
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("phrases"), SharedList::create({
         SharedMap::create(false, {
           {STR("prd"), ReferenceParser::parseQualifier(STR("module:Phrase"))},
           {STR("min"), std::make_shared<Integer>(1)},
           {STR("max"), std::make_shared<Integer>(1)},
           {STR("pty"), std::make_shared<Integer>(1)}
         })
       })}
    })},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  //// Phrase : Command | Expression.
  // Phrase : @prefix(heap.Modifiers.(DefaultModifierCmd|PhraseModifierCmd))
  //     prod (cmd:production[Command], expr:production[Expression], pty:integer) as
  //     @priority(pty,0) cmd || expr;
  this->repository.setSharedValue(STR("root:Main.Phrase"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
       {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:pty"))},
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("args:cmd")),
          ReferenceTerm::create(STR("args:exp"))
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {
       {STR("cmd"), ReferenceParser::parseQualifier("module:LeadingCmdGrp")},
       {STR("exp"), ReferenceParser::parseQualifier("root:Expression")},
       {STR("pty"), 0}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // LeadingCommandGroup
  this->repository.setSharedValue(STR("root:Main.LeadingCmdGrp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
      {TermElement::REF, ReferenceParser::parseQualifier(STR("stack:cmd"))},
      {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:cmds"))},
      {TermElement::TERM, ReferenceTerm::create(STR("stack:cmd"))}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("cmds"), SharedList::create({
         ReferenceParser::parseQualifier(STR("module:Do")),
         ReferenceParser::parseQualifier(STR("module:Import"))
       })}
    })},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  //// Do = "do" + Subject
  this->repository.setSharedValue(STR("root:Main.Do"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
    {SymbolDefElement::VARS, SharedMap::create(false, {
       {STR("kwd"), std::make_shared<String>(STR("do"))},
       {STR("prms"), SharedList::create({
          SharedMap::create(false, {
            {STR("prd"), ReferenceParser::parseQualifier(STR("root:Subject"))},
            {STR("min"), std::make_shared<Integer>(1)},
            {STR("max"), std::make_shared<Integer>(1)},
            {STR("pty"), std::make_shared<Integer>(1)}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  //// Import = "import" + Subject
  this->repository.setSharedValue(STR("root:Main.Import"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
    {SymbolDefElement::VARS, SharedMap::create(false, {
       {STR("kwd"), std::make_shared<String>(STR("import"))},
       {STR("prms"), SharedList::create({
          SharedMap::create(false, {
            {STR("prd"), ReferenceParser::parseQualifier(STR("root:Subject"))},
            {STR("min"), std::make_shared<Integer>(1)},
            {STR("max"), std::make_shared<Integer>(1)},
            {STR("pty"), std::make_shared<Integer>(1)}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->importHandler}
  }));

  //// Cmd : keyword {Subject}.
  // IdentCmd : @limit[user.parent=Command] prod (kwd:keywords, args:list[hash[prd:production[Subject], min:integer,
  //                                                                           max:integer, pty:integer]]) as
  //     IdentifierKeywordGroup(kwd) + concat (args:a)->( @priority(a.pty,0) a.prd*(a.min,a.max) );
  this->repository.setSharedValue(STR("root:Cmd"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(ParsingFlags::OMISSIBLE, std::make_shared<Integer>(0), ReferenceParser::parseQualifier(STR("args:kwd"))),
          ConcatTerm::create({
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::REF, ReferenceParser::parseQualifier(STR("stack:p"))},
            {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:prms"))},
            {TermElement::TERM, MultiplyTerm::create({
               {TermElement::PRIORITY, ReferenceParser::parseQualifier(STR("stack:p.pty"))},
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::MIN, ReferenceParser::parseQualifier(STR("stack:p.min"))},
               {TermElement::MAX, ReferenceParser::parseQualifier(STR("stack:p.max"))},
               {TermElement::TERM, ReferenceTerm::create(STR("stack:p.prd"))}
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));


  // Expression :
  // operand {binaryOp operand}.
  // [unaryOp] operand.
  // operand [unaryOp].
  // operand {FunctionalOp}.
  this->repository.setSharedValue(STR("root:Expression"), GrammarModule::create({{STR("@start"), ReferenceParser::parseQualifier(STR("module:Exp"))}}));

  // Exp : @single prod as LowestLinkExp + (@priority(in,0) lexer.Constant("\\")*(0,1));
  this->repository.setSharedValue(STR("root:Expression.Exp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:LowestLinkExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, ParsingFlags::PASS_UP},
           {TermElement::MAX, std::make_shared<Integer>(1)},
           {TermElement::TERM, TokenTerm::create(ParsingFlags::OMISSIBLE, STR("\\"))}
         })
       })}
    })},
   {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  // LowestLinkExp : @single @prefix(heap.Modifiers.LowestLinkModifierCmd)
  //     prod (enable:integer:=endless) as
  //     ConditionalExp + (@priority(in,0) (LowestLinkOp + ConditionalExp)*(0,enable));
  this->repository.setSharedValue(STR("root:Expression.LowestLinkExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:ConditionalExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, ParsingFlags::PASS_UP},
           {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
              {TermElement::TERM, SharedList::create({
                 TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("lowestLinkOp"))),
                                   ReferenceParser::parseQualifier(STR("root:TokenData.lowestLinkOpList"))),
                 ReferenceTerm::create(STR("module:ConditionalExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // ConditionalExp : @single @prefix(heap.Modifiers.ConditionalModifierCmd)
  //     prod (enable:integer[0<=n<=1]:=1) as
  //     ListExp + (@priority(in,0) (lexer.Constant("?") + ListExp)*(0,enable));
  this->repository.setSharedValue(STR("root:Expression.ConditionalExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:ListExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, ParsingFlags::PASS_UP},
           {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
              {TermElement::TERM, SharedList::create({
                 TokenTerm::create(ParsingFlags::OMISSIBLE, STR("?")),
                 ReferenceTerm::create(STR("module:ListExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), std::make_shared<Integer>(1)}})},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // ListExp : @single @prefix(heap.Modifiers.ListModifierCmd)
  //     prod (enable:integer:=endless) as
  //     (@priority(in,0) lexer.Constant(",")*(0,enable)) + LowerLinkExp +
  //     (@priority(in,0) (lexer.Constant(",") + LowerLinkExp*(0,1))*(0,enable));
  this->repository.setSharedValue(STR("root:Expression.ListExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(0)},
            {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
            {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:enable"))},
            {TermElement::TERM, TokenTerm::create(ParsingFlags::OMISSIBLE, STR(","))}
          }),
          ReferenceTerm::create(STR("module:LowerLinkExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
            {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:enable"))},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
               {TermElement::TERM, SharedList::create({
                  TokenTerm::create(ParsingFlags::OMISSIBLE, STR(",")),
                  MultiplyTerm::create({
                    {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                    {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
                    {TermElement::MAX, std::make_shared<Integer>(1)},
                    {TermElement::TERM, ReferenceTerm::create(STR("module:LowerLinkExp"))}
                  })
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // LowerLinkExp : @single @prefix(heap.Modifiers.LowerLinkModifierCmd)
  //     prod (enable:integer:=endless) as
  //     AssignmentExp + (@priority(in,0) (LowerLinkOp + AssignmentExp)*(0,enable));
  this->repository.setSharedValue(STR("root:Expression.LowerLinkExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("module:AssignmentExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
            {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:enable"))},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
               {TermElement::TERM, SharedList::create({
                  TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("lowerLinkOp"))),
                                    ReferenceParser::parseQualifier(STR("root:TokenData.lowerLinkOpList"))),
                  ReferenceTerm::create(STR("module:AssignmentExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // AssignmentExp : @single @prefix(heap.Modifiers.AssignmentModifierCmd)
  //     prod (enable:integer:=endless) as
  //     LogExp + (@priority(in,0) (AssignmentOp + LogExp)*(0,enable));
  this->repository.setSharedValue(STR("root:Expression.AssignmentExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("module:LogExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
            {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:enable"))},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
               {TermElement::TERM, SharedList::create({
                  TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("assignmentOp"))),
                                    ReferenceParser::parseQualifier(STR("root:TokenData.assignmentOpList"))),
                  ReferenceTerm::create(STR("module:LogExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // LogExp : @single @prefix(heap.Modifiers.LogModifierCmd)
  //     prod (enable:integer:=endless) as
  //     ComparisonExp + (@priority(in,0) (LogOp + ComparisonExp)*(0,enable));
  this->repository.setSharedValue(STR("root:Expression.LogExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("module:ComparisonExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
             {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:enable"))},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
                {TermElement::TERM, SharedList::create({
                   TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("logOp"))),
                                     ReferenceParser::parseQualifier(STR("root:TokenData.logOpList"))),
                   ReferenceTerm::create(STR("module:ComparisonExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
     {SymbolDefElement::HANDLER, this->parsingHandler},
     {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
   }));

  // ComparisonExp : @single @prefix(heap.Modifiers.ComparisonModifierCmd)
  //     prod (enable:integer:=endless) as
  //     LowLinkExp + (@priority(in,0) (ComparisonOp + LowLinkExp)*(0,enable));
  this->repository.setSharedValue(STR("root:Expression.ComparisonExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("module:LowLinkExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
            {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:enable"))},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
               {TermElement::TERM, SharedList::create({
                  TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("comparisonOp"))),
                                    ReferenceParser::parseQualifier(STR("root:TokenData.comparisonOpList"))),
                  ReferenceTerm::create(STR("module:LowLinkExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // LowLinkExp : @single @prefix(heap.Modifiers.LowLinkModifierCmd)
  //     prod (enable:integer:=endless) as
  //     AddExp + (@priority(in,0) (LowLinkOp + AddExp)*(0,enable));
  this->repository.setSharedValue(STR("root:Expression.LowLinkExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("module:AddExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
            {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:enable"))},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
               {TermElement::TERM, SharedList::create({
                  TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("lowLinkOp"))),
                                    ReferenceParser::parseQualifier(STR("root:TokenData.lowLinkOpList"))),
                  ReferenceTerm::create(STR("module:AddExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // AddExp : @single @prefix(heap.Modifiers.AddModifierCmd)
  //     prod (enable:integer:=endless) as
  //     MulExp + (@priority(in,0) (AddOp + MulExp)*(0,enable));
  this->repository.setSharedValue(STR("root:Expression.AddExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:MulExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
           {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
              {TermElement::TERM, SharedList::create({
                 TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("addOp"))),
                                   ReferenceParser::parseQualifier(STR("root:TokenData.addOpList"))),
                 ReferenceTerm::create(STR("module:MulExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // MulExp : @single @prefix(heap.Modifiers.MulModifierCmd)
  //     prod (enable:integer:=endless) as
  //     BitwiseExp + (@priority(in,0) (MulOp + BitwiseExp)*(0,enable));
  this->repository.setSharedValue(STR("root:Expression.MulExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:BitwiseExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
           {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
              {TermElement::TERM, SharedList::create({
                 TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("mulOp"))),
                                   ReferenceParser::parseQualifier(STR("root:TokenData.mulOpList"))),
                 ReferenceTerm::create(STR("module:BitwiseExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // BitwiseExp : @single @prefix(heap.Modifiers.BitwiseModifierCmd)
  //     prod (enable:integer:=endless) as
  //     UnaryExp + (@priority(in,0) (BitwiseOp + UnaryExp)*(0,enable));
  this->repository.setSharedValue(STR("root:Expression.BitwiseExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, SharedList::create({
         ReferenceTerm::create(STR("module:UnaryExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
           {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
              {TermElement::TERM, SharedList::create({
                 TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("bitwiseOp"))),
                                   ReferenceParser::parseQualifier(STR("root:TokenData.bitwiseOpList"))),
                 ReferenceTerm::create(STR("module:UnaryExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {{STR("enable"), 0}})},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // UnaryExp : @single @prefix(heap.Modifiers.UnaryModifierCmd)
  //     prod (enable1:integer[0<=n<=1]:=1, enable2:integer[0<=n<=1]:=1) as
  //     (@priority(in,0) PrefixOp*(0,enable1)) + FunctionalExp + (@priority(in,0) PostfixOp*(0,enable2));
  this->repository.setSharedValue(STR("root:Expression.UnaryExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, SharedList::create({
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, ParsingFlags::PASS_UP},
           {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:enable1"))},
           {TermElement::TERM, ReferenceTerm::create(STR("module:PrefixOp"))}
         }),
         ReferenceTerm::create(STR("module:FunctionalExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, ParsingFlags::PASS_UP},
           {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:enable2"))},
           {TermElement::TERM, ReferenceTerm::create(STR("module:PostfixOp"))}
         })
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("enable1"), std::make_shared<Integer>(1)},
      {STR("enable2"), std::make_shared<Integer>(1)}
    })},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // FunctionalExp : @single @prefix(heap.Modifiers.FunctionalModifierCmd)
  //     prod (operand:production[heap.Subject]:=heap.Subject, fltr1:filter:=null,
  //           fltr2:filter:=null, dup:integer:=endless, pty1:integer:=1, pty2:integer:=in) as
  //     @filter(fltr1) @priority(pty1,0)
  //         (operand + (@priority(pty2,0) (@filter(fltr2) ParamPassExp ||
  //                                                       PostfixTildeExp ||
  //                                                       LinkExp(operand))*(0,dup))) ||
  //         PrefixTildeExp + operand;
  this->repository.setSharedValue(STR("root:Expression.FunctionalExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("args:operand")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, ReferenceParser::parseQualifier(STR("args:pty2"))},
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::MAX, ReferenceParser::parseQualifier(STR("args:dup"))},
            {TermElement::TERM, AlternateTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:fltr2"))},
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
       {STR("operand"), ReferenceParser::parseQualifier(STR("root:Subject"))},
       {STR("pty2"), std::make_shared<Integer>(1)},
       {STR("dup"), 0},
       {STR("fltr2"), 0}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // LinkExp : @single prod (operand:production[heap.Subject]:=heap.Subject) as LinkOp + operand;
  this->repository.setSharedValue(STR("root:Expression.LinkExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("linkOp"))),
                            ReferenceParser::parseQualifier(STR("root:TokenData.linkOpList"))),
          ReferenceTerm::create(STR("args:operand"))
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{STR("operand"), ReferenceParser::parseQualifier(STR("root:Subject"))}})},
    {SymbolDefElement::HANDLER, this->parsingHandler}}));

  //// ParamPassExp : "(" [Expression] ")" | "[" [Expression] "]".
  // ParamPassExp : @single prod (expr:production[Expression||Statement]:=heap.Expression, fltr:filter:=null) as
  //     @filter(fltr) lexer.Constant("(") + expr*(0,1) + lexer.Constant(")") ||
  //                   lexer.Constant("[") + expr*(0,1) + lexer.Constant("]");
  this->repository.setSharedValue(STR("root:Expression.ParamPassExp"), SymbolDefinition::create({
  {SymbolDefElement::TERM, AlternateTerm::create({
     {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM},
     {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:fltr"))},
     {TermElement::TERM, SharedList::create({
        ConcatTerm::create({
          {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
          {TermElement::TERM, SharedList::create({
             TokenTerm::create(ParsingFlags::OMISSIBLE, STR("(")),
             MultiplyTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::MAX, std::make_shared<Integer>(1)},
               {TermElement::TERM, ReferenceTerm::create(STR("args:expr"))}
             }),
             TokenTerm::create(ParsingFlags::OMISSIBLE, STR(")"))
           })}
        }),
        ConcatTerm::create({
          {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
          {TermElement::TERM, SharedList::create({
             TokenTerm::create(ParsingFlags::OMISSIBLE, STR("[")),
             MultiplyTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::MAX, std::make_shared<Integer>(1)},
               {TermElement::TERM, ReferenceTerm::create(STR("args:expr"))}
             }),
             TokenTerm::create(ParsingFlags::OMISSIBLE, STR("]"))
           })}
        })
      })}
   })},
  {SymbolDefElement::VARS, SharedMap::create(false, {
     {STR("expr"), ReferenceParser::parseQualifier(STR("root:Expression"))},
     {STR("fltr"), 0}})},
  {SymbolDefElement::HANDLER, this->parsingHandler}}));

  //// PostfixTildeExp :
  //// "~" keyword {Subject}.
  //// "~(" Expression {Subject} ")".
  //PostfixTildeExp : @single prod (cmd:production[PostfixTildeCmd]:=DefaultPostfixTildeCmd) as
  //    lexer.Constant("~") + cmd;
  this->repository.setSharedValue(STR("root:Expression.PostfixTildeExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, SharedList::create({
         TokenTerm::create(ParsingFlags::OMISSIBLE, STR("~")),
         ReferenceTerm::create(STR("args:cmd"))
      })}
    })},
    {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("cmd"), ReferenceParser::parseQualifier(STR("module:DefaultPostfixTildeCmd"))}
    })},
    {SymbolDefElement::HANDLER, this->parsingHandler}}));
  //OpenPostfixTildeCmd : @limit[user.parent=PostfixTildeCmd]
  //    prod (expr:production[Expression], args:list[hash[sbj:valid_subject, min:integer,
  //                                                      max:integer, pty:integer]]) as
  //    lexer.Constant("(") + expr + concat (args:a)->( @priority(pty,0) a.sbj*(a.min,a.max) ) +
  //    lexer.Constant(")");
  this->repository.setSharedValue(STR("root:Expression.OpenPostfixTildeCmd"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR("(")),
          ReferenceTerm::create(STR("args:expr")),
          ConcatTerm::create({
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::REF, ReferenceParser::parseQualifier(STR("stack:p"))},
            {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:prms"))},
            {TermElement::TERM, MultiplyTerm::create({
               {TermElement::PRIORITY, ReferenceParser::parseQualifier(STR("stack:p.pty"))},
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::MIN, ReferenceParser::parseQualifier(STR("stack:p.min"))},
               {TermElement::MAX, ReferenceParser::parseQualifier(STR("stack:p.max"))},
               {TermElement::TERM, ReferenceTerm::create(STR("stack:p.prd"))}
             })}
          }),
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR(")"))
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {
       {STR("expr"), ReferenceParser::parseQualifier(STR("root:Expression"))},
       {STR("prms"), SharedList::create({SharedMap::create(false, {
                                     {STR("pty"), std::make_shared<Integer>(1)},
                                     {STR("min"), 0},
                                     {STR("max"), 0},
                                     {STR("prd"), ReferenceParser::parseQualifier(STR("root:Expression"))}
                                   })})}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}}));
  //DefaultPostfixTildeCmd=>PostfixTildeCmd : prod_group;
  this->repository.setSharedValue(STR("root:Expression.DefaultPostfixTildeCmd"), SymbolDefinition::create({
   {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
      {TermElement::REF, ReferenceParser::parseQualifier(STR("stack:cmd"))},
      {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:cmds"))},
      {TermElement::TERM, ReferenceTerm::create(STR("stack:cmd"))}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("cmds"), SharedList::create({ReferenceParser::parseQualifier(STR("module:OpenPostfixTildeCmd"))})}
    })},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  //// Operators :
  // PrefixOp : prod ref heap.ConstantKeywordGroup(heap.TokenData.prefixOpList);
  this->repository.setSharedValue(STR("root:Expression.PrefixOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("prefixOp"))),
                                               ReferenceParser::parseQualifier(STR("root:TokenData.prefixOpList")))},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));
  // PostfixOp : prod ref heap.ConstantKeywordGroup(heap.TokenData.postfixOpList);
  this->repository.setSharedValue(STR("root:Expression.PostfixOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("postfixOp"))),
                                               ReferenceParser::parseQualifier(STR("root:TokenData.postfixOpList")))},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));


  // Subject : Parameter | Command | Expression | Statement | Set.
  this->repository.setSharedValue(STR("root:Subject"), GrammarModule::create({{STR("@start"), ReferenceParser::parseQualifier(STR("module:Subject1"))}}));

  // Subject1 : @single prod (sbj1:list[production[Parameter||Command||Expression||Statement||Set]]
  //                              :=(Parameter, SubjectCommandGroup, heap.Default_Set),
  //                          sbj2:list[production[Parameter||Command||Expression||Statement||Set]]
  //                              :=(heap.Expression),
  //                          sbj3:list[production[Parameter||Command||Expression||Statement||Set]]
  //                              :=(heap.Expression),
  //                          frc2:integer:=0, frc3:integer:=0) as
  //     alternate (sbj1:s)->( s ) ||
  //     lexer.Constant("(") + (alternate (sbj2:s)->( s ))*(frc2,1) + lexer.Constant(")") ||
  //     lexer.Constant("[") + (alternate (sbj3:s)->( s ))*(frc3,1) + lexer.Constant("]");
  this->repository.setSharedValue(STR("root:Subject.Subject1"), SymbolDefinition::create({
   {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM},
      {TermElement::TERM, SharedList::create({
         AlternateTerm::create({
           {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
           {TermElement::REF, ReferenceParser::parseQualifier(STR("stack:s"))},
           {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:sbj1"))},
           {TermElement::TERM, ReferenceTerm::create(STR("stack:s"))}
         }),
         ConcatTerm::create({
           {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
           {TermElement::TERM, SharedList::create({
              TokenTerm::create(ParsingFlags::OMISSIBLE, STR("(")),
              MultiplyTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_UP},
                {TermElement::MIN, ReferenceParser::parseQualifier(STR("args:frc2"))},
                {TermElement::MAX, std::make_shared<Integer>(1)},
                {TermElement::TERM, AlternateTerm::create({
                   {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
                   {TermElement::REF, ReferenceParser::parseQualifier(STR("stack:s2"))},
                   {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:sbj2"))},
                   {TermElement::TERM, ReferenceTerm::create(STR("stack:s2"))}
                 })}
              }),
              TokenTerm::create(ParsingFlags::OMISSIBLE, STR(")"))
            })}
         }),
         ConcatTerm::create({
           {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
           {TermElement::TERM, SharedList::create({
              TokenTerm::create(ParsingFlags::OMISSIBLE, STR("[")),
              MultiplyTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_UP},
                {TermElement::MIN, ReferenceParser::parseQualifier(STR("args:frc3"))},
                {TermElement::MAX, std::make_shared<Integer>(1)},
                {TermElement::TERM, AlternateTerm::create({
                   {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
                   {TermElement::REF, ReferenceParser::parseQualifier(STR("stack:s3"))},
                   {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:sbj3"))},
                   {TermElement::TERM, ReferenceTerm::create(STR("stack:s3"))}
                 })}
              }),
              TokenTerm::create(ParsingFlags::OMISSIBLE, STR("]"))
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, SharedMap::create(false, {
      {STR("sbj1"), SharedList::create({
         ReferenceParser::parseQualifier(STR("module:SubjectCmdGrp")),
         ReferenceParser::parseQualifier(STR("module:Parameter")),
         ReferenceParser::parseQualifier(STR("root:Set"))
       })},
      {STR("sbj2"), SharedList::create({ReferenceParser::parseQualifier(STR("root:Expression"))})},
      {STR("sbj3"), SharedList::create({ReferenceParser::parseQualifier(STR("root:Expression"))})},
      {STR("frc2"), 0},
      {STR("frc3"), 0}
    })},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // Subject2 : @limit[user.owner=Subject] prod (sbj:production[Parameter||Command||Expression||Statement||Set],
  //                                             fltr:filter, frc:integer) as
  //     @filter(fltr) sbj ||
  //                   lexer.Constant("(") + sbj*(frc,1) + lexer.Constant(")") ||
  //                   lexer.Constant("[") + sbj*(frc,1) + lexer.Constant("]");
  this->repository.setSharedValue(STR("root:Subject.Subject2"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
        {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:fltr"))},
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("args:sbj")),
           ConcatTerm::create({
             {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
             {TermElement::TERM, SharedList::create({
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR("(")),
                MultiplyTerm::create({
                  {TermElement::FLAGS, ParsingFlags::PASS_UP},
                  {TermElement::MIN, ReferenceParser::parseQualifier(STR("args:frc"))},
                  {TermElement::MAX, std::make_shared<Integer>(1)},
                  {TermElement::TERM, ReferenceTerm::create(STR("args:sbj"))}
                }),
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR(")"))
              })}
           }),
           ConcatTerm::create({
             {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
             {TermElement::TERM, SharedList::create({
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR("[")),
                MultiplyTerm::create({
                  {TermElement::FLAGS, ParsingFlags::PASS_UP},
                  {TermElement::MIN, ReferenceParser::parseQualifier(STR("args:frc"))},
                  {TermElement::MAX, std::make_shared<Integer>(1)},
                  {TermElement::TERM, ReferenceTerm::create(STR("args:sbj"))}
                }),
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR("]"))
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
   }));

  // SubjectCommandGroup
  this->repository.setSharedValue(STR("root:Subject.SubjectCmdGrp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
       {TermElement::REF, ReferenceParser::parseQualifier(STR("stack:cmd"))},
       {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:cmds"))},
       {TermElement::TERM, ReferenceTerm::create(STR("stack:cmd"))}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{ STR("cmds"), SharedList::create({}) }} )},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  //// Parameter: Identifier | Literal.
  // Parameter : @single @prefix(heap.Modifiers.ParameterModifierCmd)
  //     prod (fltr:filter:=null, cnsts:keywords:=null) as
  //     @filter(fltr) lexer.Identifier || Literal || heap.ConstantKeywordGroup(cnsts);
  this->repository.setSharedValue(STR("root:Subject.Parameter"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
       {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:fltr"))},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.Identifier")))),
          ReferenceTerm::create(STR("module:Literal"))
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{ STR("fltr"), 0 }} )},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // Literal:
  // Literal : @single prod (fltr:filter:=null) as
  //     @filter(fltr) lexer.IntLiteral || lexer.FloatLiteral || lexer.CharLiteral || lexer.StringLiteral ||
  //                   lexer.CustomLiteral;
  this->repository.setSharedValue(STR("root:Subject.Literal"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
       {TermElement::DATA, ReferenceParser::parseQualifier(STR("args:fltr"))},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.IntLiteral")))),
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.FloatLiteral")))),
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.CharLiteral")))),
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.StringLiteral"))))
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {{ STR("fltr"), 0 }} )},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));


  // Set : "{" StatementList "}".
  // Set : @limit[child.terms=self,user.parent=self] @prefix(heap.Modifiers.DefaultModifierCmd)
  //     prod (stmt:production[Statement], ovrd:grammar_override) as
  //     lexer.Constant("{") + @override(ovrd) StatementList(stmt) + lexer.Constant("}");
  this->repository.setSharedValue(STR("root:Set"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR("{")),
          ReferenceTerm::create(STR("args:stmt")),
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR("}"))
        })}
     })},
    {SymbolDefElement::VARS, SharedMap::create(false, {
       {STR("stmt"), ReferenceParser::parseQualifier(STR("root:Main.StatementList"))}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));
}

} } // namespace
