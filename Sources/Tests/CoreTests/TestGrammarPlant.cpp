/**
 * @file Tests/CoreTests/TestGrammarPlant.cpp
 * Contains the implementation of class CoreTests::TestGrammarPlant.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core_tests.h"

namespace Tests { namespace CoreTests
{

using namespace Core::Data;
using namespace Core::Processing;
using namespace Core::Standard;

//==============================================================================
// Member Functions

void TestGrammarPlant::createGrammar()
{
  // Instantiate parsing handlers.
  this->parsingHandler = std::make_shared<GenericParsingHandler>();

  // Create lexer definitions.
  this->repository.setSharedValue(STR("root:LexerDefs"), GrammarModule::create({}));
  this->createCharGroupDefinitions();
  this->createTokenDefinitions();

  // Create parser definitions.
  this->createProductionDefinitions();

  // Set start production and lexer module.
  GrammarModule *root = this->repository.getRoot().get();
  root->setStartRef(ReferenceParser::parseQualifier(STR("module:Program")));
  root->setLexerModuleRef(ReferenceParser::parseQualifier(STR("root:LexerDefs")));

  // Generate const token definitions from production definitions.
  this->constTokenPrefix = STR("LexerDefs");
  this->generateConstTokenDefinitions();
}


/**
 * Create a CharGroupDefinitionList and add all the required definitions for
 * the Core's grammar.
 */
void TestGrammarPlant::createCharGroupDefinitions()
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
void TestGrammarPlant::createTokenDefinitions()
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
void TestGrammarPlant::createProductionDefinitions()
{
  // TODO: Replace the generic parsing handler for the root with the appropriate one.
  // Program = Statement*v1;
  this->repository.setSharedValue(STR("Program"), SymbolDefinition::create({
    {SymbolDefElement::TERM, MultiplyTerm::create({
       {TermElement::PRIORITY, std::make_shared<Integer>(1)},
       {TermElement::MIN, std::make_shared<Integer>(1)},
       {TermElement::TERM, ReferenceTerm::create(STR("root:Statement"))}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  // Statement = Command + ";" || Expression + ";";
  this->repository.setSharedValue(STR("Statement"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::ESPI, 1},
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("root:SubStatement")),
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR(";"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
  }));

  // Statement = Command || Expression;
  this->repository.setSharedValue(STR("SubStatement"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("root:Command")),
          ReferenceTerm::create(STR("root:Expression"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // Command = "do" + Expression;
  this->repository.setSharedValue(STR("Command"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR("do")),
          ReferenceTerm::create(STR("root:Expression"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
  }));

  // Set = "{" + Statement*v + "}";
  this->repository.setSharedValue(STR("Set"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR("{")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
            {TermElement::TERM, ReferenceTerm::create(STR("root:Statement"))}
          }),
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR("}"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
  }));

  // Expression = ListExp + ("?" + Expression)*o;
  this->repository.setSharedValue(STR("Expression"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("root:ListExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::MAX, std::make_shared<Integer>(1)},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
                {TermElement::TERM, SharedList::create({
                   TokenTerm::create(ParsingFlags::OMISSIBLE, STR("?")),
                   ReferenceTerm::create(STR("root:Expression"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
   }));

  // ListExp = ","*v + ColonPairExp + ("," + ColonPairExp*o)*v;
  this->repository.setSharedValue(STR("ListExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::TERM, TokenTerm::create(ParsingFlags::OMISSIBLE, STR(","))}
          }),
          ReferenceTerm::create(STR("root:ColonPairExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
               {TermElement::TERM, SharedList::create({
                  TokenTerm::create(ParsingFlags::OMISSIBLE, STR(",")),
                  MultiplyTerm::create({
                    {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                    {TermElement::FLAGS, ParsingFlags::PASS_UP},
                    {TermElement::MAX, std::make_shared<Integer>(1)},
                    {TermElement::TERM, ReferenceTerm::create(STR("root:ColonPairExp"))}
                  })
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // ColonPairExp = AssignmentExp + (":" + AssignmentExp)*o;
  this->repository.setSharedValue(STR("ColonPairExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("root:AssignmentExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::MAX, std::make_shared<Integer>(1)},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
                {TermElement::TERM, SharedList::create({
                   TokenTerm::create(ParsingFlags::OMISSIBLE, STR(":")),
                   ReferenceTerm::create(STR("root:AssignmentExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
     {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
   }));

  // AssignmentExp = LogExp + (AssignmentOp + AssignmentExp)*o;
  this->repository.setSharedValue(STR("AssignmentExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("root:LogExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::MAX, std::make_shared<Integer>(1)},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::TERM, SharedList::create({
                  ReferenceTerm::create(STR("root:AssignmentOp")),
                  ReferenceTerm::create(STR("root:AssignmentExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // LogExp = ComparisonExp + (LogOp + ComparisonExp)*v;
  this->repository.setSharedValue(STR("LogExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("root:ComparisonExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_UP},
                {TermElement::TERM, SharedList::create({
                   ReferenceTerm::create(STR("root:LogOp")),
                   ReferenceTerm::create(STR("root:ComparisonExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
     {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
   }));

  // ComparisonExp = AddExp + (ComparisonOp + AddExp)*v;
  this->repository.setSharedValue(STR("ComparisonExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("root:AddExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::TERM, SharedList::create({
                  ReferenceTerm::create(STR("root:ComparisonOp")),
                  ReferenceTerm::create(STR("root:AddExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // AddExp = MulExp + (AddOp + MulExp)*v;
  this->repository.setSharedValue(STR("AddExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("root:MulExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_UP},
                {TermElement::TERM, SharedList::create({
                   ReferenceTerm::create(STR("root:AddOp")),
                   ReferenceTerm::create(STR("root:MulExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
     {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
   }));

  // MulExp = BitwiseExp + (MulOp + BitwiseExp)*v;
  this->repository.setSharedValue(STR("MulExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("root:BitwiseExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_UP},
                {TermElement::TERM, SharedList::create({
                   ReferenceTerm::create(STR("root:MulOp")),
                   ReferenceTerm::create(STR("root:BitwiseExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
     {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
   }));

  // BitwiseExp = UnaryExp + (BitwiseOp + UnaryExp)*v;
  this->repository.setSharedValue(STR("BitwiseExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, SharedList::create({
           ReferenceTerm::create(STR("root:UnaryExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_UP},
                {TermElement::TERM, SharedList::create({
                   ReferenceTerm::create(STR("root:BitwiseOp")),
                   ReferenceTerm::create(STR("root:UnaryExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
     {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
   }));

  // UnaryExp = PrefixOp*o + FunctionalExp + PostfixOp*o;
  this->repository.setSharedValue(STR("UnaryExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, SharedList::create({
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::MAX, std::make_shared<Integer>(1)},
             {TermElement::TERM, ReferenceTerm::create(STR("root:PrefixOp"))}
           }),
           ReferenceTerm::create(STR("root:FunctionalExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::MAX, std::make_shared<Integer>(1)},
             {TermElement::TERM, ReferenceTerm::create(STR("root:PostfixOp"))}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
     {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
   }));

  // FunctionalExp = Operand + (LinkExp || ParamPassExp)*v;
  this->repository.setSharedValue(STR("FunctionalExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("root:Operand")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::TERM, AlternateTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::TERM, SharedList::create({
                  ReferenceTerm::create(STR("root:LinkExp")),
                  ReferenceTerm::create(STR("root:ParamPassExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // LinkExp = LinkOp + Operand;
  this->repository.setSharedValue(STR("LinkExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("root:LinkOp")),
          ReferenceTerm::create(STR("root:Operand"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
  }));

  // ParamPassExp = "(" + Expression*o + ")" || "[" + Expression*o + "]";
  this->repository.setSharedValue(STR("ParamPassExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::TERM, SharedList::create({
           ConcatTerm::create({
             {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
             {TermElement::TERM, SharedList::create({
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR("(")),
                MultiplyTerm::create({
                  {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                  {TermElement::FLAGS, ParsingFlags::PASS_UP},
                  {TermElement::MAX, std::make_shared<Integer>(1)},
                  {TermElement::TERM, ReferenceTerm::create(STR("root:Expression"))}
                }),
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR(")"))
              })}
           }),
           ConcatTerm::create({
             {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
             {TermElement::TERM, SharedList::create({
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR("[")),
                MultiplyTerm::create({
                  {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                  {TermElement::FLAGS, ParsingFlags::PASS_UP},
                  {TermElement::MAX, std::make_shared<Integer>(1)},
                  {TermElement::TERM, ReferenceTerm::create(STR("root:Expression"))}
                }),
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR("]"))
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
   }));

  // Operand = Parameter || Command || Set || "(" + Expression*o + ")" || "[" + Expression*o + "]";
  this->repository.setSharedValue(STR("Operand"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::TERM, SharedList::create({
          ReferenceTerm::create(STR("root:Parameter")),
          ReferenceTerm::create(STR("root:Set")),
          ConcatTerm::create({
            {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
            {TermElement::TERM, SharedList::create({
               TokenTerm::create(ParsingFlags::OMISSIBLE, STR("(")),
               MultiplyTerm::create({
                 {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                 {TermElement::FLAGS, ParsingFlags::PASS_UP},
                 {TermElement::MAX, std::make_shared<Integer>(1)},
                 {TermElement::TERM, ReferenceTerm::create(STR("root:SubStatement"))}
               }),
               TokenTerm::create(ParsingFlags::OMISSIBLE, STR(")"))
             })}
          }),
          ConcatTerm::create({
            {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
            {TermElement::TERM, SharedList::create({
               TokenTerm::create(ParsingFlags::OMISSIBLE, STR("[")),
               MultiplyTerm::create({
                 {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                 {TermElement::FLAGS, ParsingFlags::PASS_UP},
                 {TermElement::MAX, std::make_shared<Integer>(1)},
                 {TermElement::TERM, ReferenceTerm::create(STR("root:SubStatement"))}
               }),
               TokenTerm::create(ParsingFlags::OMISSIBLE, STR("]"))
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
  }));

  // Parameter = identifier || Literal;
  this->repository.setSharedValue(STR("Parameter"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.Identifier")))),
          ReferenceTerm::create(STR("root:Literal"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // Literal = int_literal || float_literal || char_literal || string_literal;
  this->repository.setSharedValue(STR("Literal"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.IntLiteral")))),
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.FloatLiteral")))),
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.CharLiteral")))),
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("LexerDefs.StringLiteral"))))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // AssignmentOp = ":=" || "+=" || "-=" || "*=" || "/=" || "%=" || "&=" || "|=" || "$=" || "^:=" || "<<=" || ">>=";
  this->repository.setSharedValue(STR("AssignmentOp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::FLAGS, ParsingFlags::PASS_UP},
        {TermElement::TERM, SharedList::create({
           TokenTerm::create(0, STR(":=")),
           TokenTerm::create(0, STR("+=")),
           TokenTerm::create(0, STR("-=")),
           TokenTerm::create(0, STR("*=")),
           TokenTerm::create(0, STR("/=")),
           TokenTerm::create(0, STR("%=")),
           TokenTerm::create(0, STR("&=")),
           TokenTerm::create(0, STR("|=")),
           TokenTerm::create(0, STR("$=")),
           TokenTerm::create(0, STR("^:=")),
           TokenTerm::create(0, STR("<<=")),
           TokenTerm::create(0, STR(">>="))
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
   }));

  // ComparisonOp = "=" || "^=" || "==" || "^==" || "<" || ">" || "<=" || ">=";
  this->repository.setSharedValue(STR("ComparisonOp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::FLAGS, ParsingFlags::PASS_UP},
        {TermElement::TERM, SharedList::create({
           TokenTerm::create(0, STR("=")),
           TokenTerm::create(0, STR("^=")),
           TokenTerm::create(0, STR("==")),
           TokenTerm::create(0, STR("^==")),
           TokenTerm::create(0, STR("<")),
           TokenTerm::create(0, STR(">")),
           TokenTerm::create(0, STR("<=")),
           TokenTerm::create(0, STR(">="))
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
   }));

  // AddOp = "+" || "-";
  this->repository.setSharedValue(STR("AddOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, STR("+")),
          TokenTerm::create(0, STR("-"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  // MulOp = "*" || "/" || "%";
  this->repository.setSharedValue(STR("MulOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, STR("*")),
          TokenTerm::create(0, STR("/")),
          TokenTerm::create(0, STR("%"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  // BitwiseOp = "|" || "^|" || "$" || "^$" || "&" || "^&" || "<<" || ">>";
  this->repository.setSharedValue(STR("BitwiseOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, STR("|")),
          TokenTerm::create(0, STR("^|")),
          TokenTerm::create(0, STR("$")),
          TokenTerm::create(0, STR("^$")),
          TokenTerm::create(0, STR("&")),
          TokenTerm::create(0, STR("^&")),
          TokenTerm::create(0, STR("<<")),
          TokenTerm::create(0, STR(">>"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  // LogOp = "||" || "^||" || "$$" || "^$$" || "&&" || "^&&";
  this->repository.setSharedValue(STR("LogOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, STR("||")),
          TokenTerm::create(0, STR("^||")),
          TokenTerm::create(0, STR("$$")),
          TokenTerm::create(0, STR("^$$")),
          TokenTerm::create(0, STR("&&")),
          TokenTerm::create(0, STR("^&&"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  // PrefixOp = "++" || "--" || "+" || "-" || "^" || "^^";
  this->repository.setSharedValue(STR("PrefixOp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::FLAGS, ParsingFlags::PASS_UP},
        {TermElement::TERM, SharedList::create({
           TokenTerm::create(0, STR("++")),
           TokenTerm::create(0, STR("--")),
           TokenTerm::create(0, STR("+")),
           TokenTerm::create(0, STR("-")),
           TokenTerm::create(0, STR("^")),
           TokenTerm::create(0, STR("^^"))
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
   }));

  // PostfixOp = "++" || "--";
  this->repository.setSharedValue(STR("PostfixOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, STR("++")),
          TokenTerm::create(0, STR("--"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  // LinkOp = "." || "->" || "=>";
  this->repository.setSharedValue(STR("LinkOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, SharedList::create({
          TokenTerm::create(0, STR(".")),
          TokenTerm::create(0, STR("->")),
          TokenTerm::create(0, STR("=>"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));
}

} } // namespace
