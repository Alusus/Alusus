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

//==============================================================================
// Constructor

TestGrammarPlant::TestGrammarPlant() : Data::GrammarPlant(false),
  BIN_DIGIT_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("BIN_DIGIT_CHAR_GROUP")),
  OCT_DIGIT_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("OCT_DIGIT_CHAR_GROUP")),
  DEC_DIGIT_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("DEC_DIGIT_CHAR_GROUP")),
  HEX_DIGIT_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("HEX_DIGIT_CHAR_GROUP")),
  LETTER_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("LETTER_CHAR_GROUP")),
  ANY_CHAR_NO_ES_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("ANY_CHAR_NO_ES_CHAR_GROUP")),
  ANY_CHAR_NO_ES_OR_SINGLE_QUOTE_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("ANY_CHAR_NO_ES_OR_SINGLE_QUOTE_CHAR_GROUP")),
  ANY_CHAR_NO_ES_OR_DOUBLE_QUOTE_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("ANY_CHAR_NO_ES_OR_DOUBLE_QUOTE_CHAR_GROUP")),
  SPACING_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("SPACING_CHAR_GROUP")),

  IDENTIFIER_TOKEN(Data::IdGenerator::getSingleton()->getId("IDENTIFIER_TOKEN")),
  INT_LITERAL_TOKEN(Data::IdGenerator::getSingleton()->getId("INT_LITERAL_TOKEN")),
  _DEC_INT_LITERAL_TOKEN(Data::IdGenerator::getSingleton()->getId("_DEC_INT_LITERAL_TOKEN")),
  _BIN_INT_LITERAL_TOKEN(Data::IdGenerator::getSingleton()->getId("_BIN_INT_LITERAL_TOKEN")),
  _OCT_INT_LITERAL_TOKEN(Data::IdGenerator::getSingleton()->getId("_OCT_INT_LITERAL_TOKEN")),
  _HEX_INT_LITERAL_TOKEN(Data::IdGenerator::getSingleton()->getId("_HEX_INT_LITERAL_TOKEN")),
  FLOAT_LITERAL_TOKEN(Data::IdGenerator::getSingleton()->getId("FLOAT_LITERAL_TOKEN")),
  _FLOAT_EXPONENT_TOKEN(Data::IdGenerator::getSingleton()->getId("_FLOAT_EXPONENT_TOKEN")),
  _FLOAT_POSTFIX_TOKEN(Data::IdGenerator::getSingleton()->getId("_FLOAT_POSTFIX_TOKEN")),
  CHAR_LITERAL_TOKEN(Data::IdGenerator::getSingleton()->getId("CHAR_LITERAL_TOKEN")),
  STRING_LITERAL_TOKEN(Data::IdGenerator::getSingleton()->getId("STRING_LITERAL_TOKEN")),
  _STRING_LITERAL_PART_TOKEN(Data::IdGenerator::getSingleton()->getId("_STRING_LITERAL_PART_TOKEN")),
  _CHAR_CODE_POSTFIX_TOKEN(Data::IdGenerator::getSingleton()->getId("_CHAR_CODE_POSTFIX_TOKEN")),
  _ES_CHAR_WITH_SINGLE_QUOTE_TOKEN(Data::IdGenerator::getSingleton()->getId("_ES_CHAR_WITH_SINGLE_QUOTE_TOKEN")),
  _ES_CHAR_WITH_DOUBLE_QUOTE_TOKEN(Data::IdGenerator::getSingleton()->getId("_ES_CHAR_WITH_DOUBLE_QUOTE_TOKEN")),
  _ES_CHAR_WITH_QUOTES_TOKEN(Data::IdGenerator::getSingleton()->getId("_ES_CHAR_WITH_QUOTES_TOKEN")),
  _ES_SEQUENCE_TOKEN(Data::IdGenerator::getSingleton()->getId("_ES_SEQUENCE_TOKEN")),
  CUSTOM_LITERAL_TOKEN(Data::IdGenerator::getSingleton()->getId("CUSTOM_LITERAL_TOKEN")),

  SPACES_TOKEN(Data::IdGenerator::getSingleton()->getId("SPACES_TOKEN") | IGNORED_TOKEN_BASE),
  LINE_COMMENT_TOKEN(Data::IdGenerator::getSingleton()->getId("LINE_COMMENT_TOKEN") | IGNORED_TOKEN_BASE),
  BLOCK_COMMENT_TOKEN(Data::IdGenerator::getSingleton()->getId("BLOCK_COMMENT_TOKEN") | IGNORED_TOKEN_BASE)
{
  this->parsingHandler = std::make_shared<Standard::GenericParsingHandler>();
  this->createGrammar();
}


//==============================================================================
// Member Functions

/**
 * Create a CharGroupDefinitionList and add all the required definitions for
 * the Core's grammar.
 */
void TestGrammarPlant::createCharGroupDefinitions()
{
  using namespace Core::Lexer;

  this->charGroupDefinitions = SharedPtr<CharGroupDefinitionList>(new CharGroupDefinitionList());

  // bin_digit = "01";
  this->charGroupDefinitions->add(BIN_DIGIT_CHAR_GROUP,
                                  new SequenceCharGroupUnit('0', '1'));

  // oct_digit = "01234567";
  this->charGroupDefinitions->add(OCT_DIGIT_CHAR_GROUP,
                                  new SequenceCharGroupUnit('0', '7'));

  // dec_digit = "0123456789";
  this->charGroupDefinitions->add(DEC_DIGIT_CHAR_GROUP,
                                  new SequenceCharGroupUnit('0', '9'));

  // hex_digit = "0123456789abcdefABCDEF";
  this->charGroupDefinitions->add(HEX_DIGIT_CHAR_GROUP,
                                  new UnionCharGroupUnit(3,
                                                         new SequenceCharGroupUnit('0', '9'),
                                                         new SequenceCharGroupUnit('a', 'f'),
                                                         new SequenceCharGroupUnit('A', 'F')));

  // letter = 'a'-'z', 'A'-'Z', '_';
  this->charGroupDefinitions->add(LETTER_CHAR_GROUP,
                                  new UnionCharGroupUnit(3,
                                                         new SequenceCharGroupUnit('a', 'z'),
                                                         new SequenceCharGroupUnit('A', 'Z'),
                                                         new SequenceCharGroupUnit('_', '_')));

  // any_char_no_es = ^('\\');
  this->charGroupDefinitions->add(ANY_CHAR_NO_ES_CHAR_GROUP,
                                  new InvertCharGroupUnit(
                                    new SequenceCharGroupUnit('\\', '\\')));

  // any_char_no_es_or_single_quote = ^("\\'");
  this->charGroupDefinitions->add(ANY_CHAR_NO_ES_OR_SINGLE_QUOTE_CHAR_GROUP,
                                  new InvertCharGroupUnit(
                                    new RandomCharGroupUnit("\\'")));

  // any_char_no_es_or_double_quote = ^('\\');
  this->charGroupDefinitions->add(ANY_CHAR_NO_ES_OR_DOUBLE_QUOTE_CHAR_GROUP,
                                  new InvertCharGroupUnit(
                                    new RandomCharGroupUnit("\\\"")));

  // spacing = ' ', '\n', '\r', '\t';
  this->charGroupDefinitions->add(SPACING_CHAR_GROUP,
                                  new RandomCharGroupUnit(" \r\n\t"));
}


/**
 * Create a TokenDefinitionList and add all the required definitions for
 * the Core's grammar.
 */
void TestGrammarPlant::createTokenDefinitions()
{
  using namespace Core::Lexer;

  this->tokenDefinitions = SharedPtr<TokenDefinitionList>(new TokenDefinitionList());

  // identifier = letter + (letter || dec_digit)*v;
  this->tokenDefinitions->add(IDENTIFIER_TOKEN,
                              new ConcatTerm(2,
                                             new CharGroupTerm(LETTER_CHAR_GROUP),
                                             new DuplicateTerm(
                                               new AlternativeTerm(2,
                                                                   new CharGroupTerm(LETTER_CHAR_GROUP),
                                                                   new CharGroupTerm(DEC_DIGIT_CHAR_GROUP)))));

  // int_literal =
  //    (dec_int_literal || bin_int_literal || oct_int_literal || hex_int_literal) +
  //    ("u" || "U")*o + (("i" || "I") + dec_int_literal)*o;
  this->tokenDefinitions->add(INT_LITERAL_TOKEN,
                              new ConcatTerm(3,
                                             new AlternativeTerm(4,
                                                                 new ReferenceTerm(_DEC_INT_LITERAL_TOKEN),
                                                                 new ReferenceTerm(_BIN_INT_LITERAL_TOKEN),
                                                                 new ReferenceTerm(_OCT_INT_LITERAL_TOKEN),
                                                                 new ReferenceTerm(_HEX_INT_LITERAL_TOKEN)),
                                             new OptionalTerm(
                                               new AlternativeTerm(2,
                                                                   new ConstTerm("u"),
                                                                   new ConstTerm("U"))),
                                             new OptionalTerm(
                                               new ConcatTerm(2,
                                                              new AlternativeTerm(2,
                                                                                  new ConstTerm("i"),
                                                                                  new ConstTerm("I")),
                                                              new ReferenceTerm(_DEC_INT_LITERAL_TOKEN)))));

  // @inner dec_int_literal = dec_digit*v1;
  this->tokenDefinitions->add(_DEC_INT_LITERAL_TOKEN,
                              new DuplicateTerm(
                                new CharGroupTerm(DEC_DIGIT_CHAR_GROUP), 1),
                              true);

  // @inner bin_int_literal = ("0b" || "0B") + bin_digit*v1;
  this->tokenDefinitions->add(_BIN_INT_LITERAL_TOKEN,
                              new ConcatTerm(2,
                                             new AlternativeTerm(2,
                                                                 new ConstTerm("0b"),
                                                                 new ConstTerm("0B")),
                                             new DuplicateTerm(
                                               new CharGroupTerm(BIN_DIGIT_CHAR_GROUP), 1)),
                              true);

  // @inner oct_int_literal = ("0o" || "0O") + oct_digit*v1;
  this->tokenDefinitions->add(_OCT_INT_LITERAL_TOKEN,
                              new ConcatTerm(2,
                                             new AlternativeTerm(2,
                                                                 new ConstTerm("0o"),
                                                                 new ConstTerm("0O")),
                                             new DuplicateTerm(
                                               new CharGroupTerm(OCT_DIGIT_CHAR_GROUP), 1)),
                              true);

  // @inner hex_int_literal = ("0h" || "0H") + hex_digit*v1;
  this->tokenDefinitions->add(_HEX_INT_LITERAL_TOKEN,
                              new ConcatTerm(2,
                                             new AlternativeTerm(2,
                                                                 new ConstTerm("0h"),
                                                                 new ConstTerm("0H")),
                                             new DuplicateTerm(
                                               new CharGroupTerm(HEX_DIGIT_CHAR_GROUP), 1)),
                              true);

  // float_literal =
  //    dec_digit*v1 + float_postfix ||
  //    dec_digit*v1 + float_exponent + float_postfix*o ||
  //    dec_digit*v + "." + dec_digit*v1 + float_exponent*o +
  //        float_postfix*o;
  this->tokenDefinitions->add(FLOAT_LITERAL_TOKEN,
                              new AlternativeTerm(3,
                                                  new ConcatTerm(2,
                                                                 new DuplicateTerm(new CharGroupTerm(DEC_DIGIT_CHAR_GROUP), 1),
                                                                 new ReferenceTerm(_FLOAT_POSTFIX_TOKEN)),
                                                  new ConcatTerm(3,
                                                                 new DuplicateTerm(new CharGroupTerm(DEC_DIGIT_CHAR_GROUP), 1),
                                                                 new ReferenceTerm(_FLOAT_EXPONENT_TOKEN),
                                                                 new OptionalTerm(new ReferenceTerm(_FLOAT_POSTFIX_TOKEN))),
                                                  new ConcatTerm(5,
                                                                 new DuplicateTerm(
                                                                   new CharGroupTerm(DEC_DIGIT_CHAR_GROUP)),
                                                                 new ConstTerm("."),
                                                                 new DuplicateTerm(
                                                                   new CharGroupTerm(DEC_DIGIT_CHAR_GROUP), 1),
                                                                 new OptionalTerm(
                                                                   new ReferenceTerm(_FLOAT_EXPONENT_TOKEN)),
                                                                 new OptionalTerm(
                                                                   new ReferenceTerm(_FLOAT_POSTFIX_TOKEN)))));

  // @inner float_exponent = ("e" || "E") + ("+" || "-")*o + dec_digit*v1;
  this->tokenDefinitions->add(_FLOAT_EXPONENT_TOKEN,
                              new ConcatTerm(3,
                                             new AlternativeTerm(2,
                                                                 new ConstTerm("e"),
                                                                 new ConstTerm("E")),
                                             new OptionalTerm(
                                               new AlternativeTerm(2,
                                                                   new ConstTerm("+"),
                                                                   new ConstTerm("-"))),
                                             new DuplicateTerm(
                                               new CharGroupTerm(DEC_DIGIT_CHAR_GROUP), 1)),
                              true);

  // @inner float_postfix = ("f" || "F") + dec_digit*v;
  this->tokenDefinitions->add(_FLOAT_POSTFIX_TOKEN,
                              new ConcatTerm(2,
                                             new AlternativeTerm(2,
                                                                 new ConstTerm("f"),
                                                                 new ConstTerm("F")),
                                             new DuplicateTerm(new CharGroupTerm(DEC_DIGIT_CHAR_GROUP))),
                              true);

  // char_literal = "'" + es_char_with_double_quote + "'" + char_code_postfix*o;
  // TODO: Add the char_code_postfix part
  this->tokenDefinitions->add(CHAR_LITERAL_TOKEN,
                              new ConcatTerm(3,
                                             new ConstTerm("'"),
                                             new ReferenceTerm(_ES_CHAR_WITH_DOUBLE_QUOTE_TOKEN),
                                             new ConstTerm("'")));

  // string_literal =
  //    string_literal_part + (spacing*v + string_literal_part)*v + char_code_postfix*o;
  // TODO: Add the char_code_postfix part
  this->tokenDefinitions->add(STRING_LITERAL_TOKEN,
                              new ConcatTerm(2,
                                             new ReferenceTerm(_STRING_LITERAL_PART_TOKEN),
                                             new DuplicateTerm(
                                               new ConcatTerm(2,
                                                              new DuplicateTerm(
                                                                new CharGroupTerm(SPACING_CHAR_GROUP)),
                                                              new ReferenceTerm(_STRING_LITERAL_PART_TOKEN)))));

  // @inner string_literal_part = "\"" + es_char_with_single_quote*v + "\"";
  this->tokenDefinitions->add(_STRING_LITERAL_PART_TOKEN,
                              new ConcatTerm(3,
                                             new ConstTerm("\""),
                                             new DuplicateTerm(
                                               new ReferenceTerm(_ES_CHAR_WITH_SINGLE_QUOTE_TOKEN)),
                                             new ConstTerm("\"")),
                              true);

  // @inner es_char_with_single_quote =
  //    any_char_no_es_or_double_quote || char_code_es ||
  //    (heap.escape_sequences.kwd)~alternate[heap.escape_sequences];
  // TODO: Add the heap.escape_sequences part
  this->tokenDefinitions->add(_ES_CHAR_WITH_SINGLE_QUOTE_TOKEN,
                              new AlternativeTerm(2,
                                                  new CharGroupTerm(ANY_CHAR_NO_ES_OR_DOUBLE_QUOTE_CHAR_GROUP),
                                                  new ReferenceTerm(_ES_SEQUENCE_TOKEN)),
                              true);

  // @inner es_char_with_double_quote =
  //    any_char_no_es_or_single_quote || char_code_es ||
  //    (heap.escape_sequences.kwd)~alternate[heap.escape_sequences];
  // TODO: Add the heap.escape_sequences part
  this->tokenDefinitions->add(_ES_CHAR_WITH_DOUBLE_QUOTE_TOKEN,
                              new AlternativeTerm(2,
                                                  new CharGroupTerm(ANY_CHAR_NO_ES_OR_SINGLE_QUOTE_CHAR_GROUP),
                                                  new ReferenceTerm(_ES_SEQUENCE_TOKEN)),
                              true);

  // @inner es_char_with_quotes =
  //    any_char_no_es || char_code_es ||
  //    (heap.escape_sequences.kwd)~alternate[heap.escape_sequences];
  // TODO: Add the heap.escape_sequences part
  this->tokenDefinitions->add(_ES_CHAR_WITH_QUOTES_TOKEN,
                              new AlternativeTerm(2,
                                                  new CharGroupTerm(ANY_CHAR_NO_ES_CHAR_GROUP),
                                                  new ReferenceTerm(_ES_SEQUENCE_TOKEN)),
                              true);

  // @inner char_code_es =
  //    "\\c" + hex_digit*2 ||
  //    "\\u" + hex_digit*4 ||
  //    "\\w" + hex_digit*8;
  this->tokenDefinitions->add(_ES_SEQUENCE_TOKEN,
                              new ConcatTerm(2,
                                             new ConstTerm("\\"),
                                             new AlternativeTerm(6,
                                                                 new ConstTerm("n"),
                                                                 new ConstTerm("t"),
                                                                 new ConstTerm("r"),
                                                                 new ConcatTerm(3,
                                                                                new ConstTerm("c"),
                                                                                new CharGroupTerm(HEX_DIGIT_CHAR_GROUP),
                                                                                new CharGroupTerm(HEX_DIGIT_CHAR_GROUP)),
                                                                 new ConcatTerm(5,
                                                                                new ConstTerm("u"),
                                                                                new CharGroupTerm(HEX_DIGIT_CHAR_GROUP),
                                                                                new CharGroupTerm(HEX_DIGIT_CHAR_GROUP),
                                                                                new CharGroupTerm(HEX_DIGIT_CHAR_GROUP),
                                                                                new CharGroupTerm(HEX_DIGIT_CHAR_GROUP)),
                                                                 new ConcatTerm(9,
                                                                                new ConstTerm("w"),
                                                                                new CharGroupTerm(HEX_DIGIT_CHAR_GROUP),
                                                                                new CharGroupTerm(HEX_DIGIT_CHAR_GROUP),
                                                                                new CharGroupTerm(HEX_DIGIT_CHAR_GROUP),
                                                                                new CharGroupTerm(HEX_DIGIT_CHAR_GROUP),
                                                                                new CharGroupTerm(HEX_DIGIT_CHAR_GROUP),
                                                                                new CharGroupTerm(HEX_DIGIT_CHAR_GROUP),
                                                                                new CharGroupTerm(HEX_DIGIT_CHAR_GROUP),
                                                                                new CharGroupTerm(HEX_DIGIT_CHAR_GROUP))),
                                             true));


  //// IGNORED TOKENS

  // spacing * v1;
  this->tokenDefinitions->add(SPACES_TOKEN,
                              new DuplicateTerm(
                                new CharGroupTerm(SPACING_CHAR_GROUP), 1));
}


/**
 * Create a Production_Definition_List and add all the required definitions for
 * the Core's grammar. The created list will have assigned parsing handlers.
 */
void TestGrammarPlant::createProductionDefinitions()
{
  using namespace Core::Data;
  using namespace Core::Common;
  using namespace Core::Parser;
  using namespace Core::Standard;

  // TODO: Replace the generic parsing handler for the root with the appropriate one.
  // Program = Statement*v1;
  this->store.setSharedValue(STR("Program"), SymbolDefinition::create({
    {SymbolDefElement::TERM, MultiplyTerm::create({
       {TermElement::PRIORITY, std::make_shared<Integer>(1)},
       {TermElement::MIN, std::make_shared<Integer>(1)},
       {TermElement::TERM, ReferenceTerm::create(STR("root.Statement"))}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  // Statement = Command + ";" || Expression + ";";
  this->store.setSharedValue(STR("Statement"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::ESPI, 1},
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          ReferenceTerm::create(STR("root.SubStatement")),
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR(";"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
  }));

  // Statement = Command || Expression;
  this->store.setSharedValue(STR("SubStatement"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, List::create({
          ReferenceTerm::create(STR("root.Command")),
          ReferenceTerm::create(STR("root.Expression"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // Command = "do" + Expression;
  this->store.setSharedValue(STR("Command"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR("do")),
          ReferenceTerm::create(STR("root.Expression"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
  }));

  // Set = "{" + Statement*v + "}";
  this->store.setSharedValue(STR("Set"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR("{")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
            {TermElement::TERM, ReferenceTerm::create(STR("root.Statement"))}
          }),
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR("}"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
  }));

  // Expression = ListExp + ("?" + Expression)*o;
  this->store.setSharedValue(STR("Expression"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, List::create({
           ReferenceTerm::create(STR("root.ListExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::MAX, std::make_shared<Integer>(1)},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
                {TermElement::TERM, List::create({
                   TokenTerm::create(ParsingFlags::OMISSIBLE, STR("?")),
                   ReferenceTerm::create(STR("root.Expression"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
   }));

  // ListExp = ","*v + ColonPairExp + ("," + ColonPairExp*o)*v;
  this->store.setSharedValue(STR("ListExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, List::create({
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::TERM, TokenTerm::create(ParsingFlags::OMISSIBLE, STR(","))}
          }),
          ReferenceTerm::create(STR("root.ColonPairExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
               {TermElement::TERM, List::create({
                  TokenTerm::create(ParsingFlags::OMISSIBLE, STR(",")),
                  MultiplyTerm::create({
                    {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                    {TermElement::FLAGS, ParsingFlags::PASS_UP},
                    {TermElement::MAX, std::make_shared<Integer>(1)},
                    {TermElement::TERM, ReferenceTerm::create(STR("root.ColonPairExp"))}
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
  this->store.setSharedValue(STR("ColonPairExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, List::create({
           ReferenceTerm::create(STR("root.AssignmentExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::MAX, std::make_shared<Integer>(1)},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
                {TermElement::TERM, List::create({
                   TokenTerm::create(ParsingFlags::OMISSIBLE, STR(":")),
                   ReferenceTerm::create(STR("root.AssignmentExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
     {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
   }));

  // AssignmentExp = LogExp + (AssignmentOp + AssignmentExp)*o;
  this->store.setSharedValue(STR("AssignmentExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          ReferenceTerm::create(STR("root.LogExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::MAX, std::make_shared<Integer>(1)},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::TERM, List::create({
                  ReferenceTerm::create(STR("root.AssignmentOp")),
                  ReferenceTerm::create(STR("root.AssignmentExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // LogExp = ComparisonExp + (LogOp + ComparisonExp)*v;
  this->store.setSharedValue(STR("LogExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, List::create({
           ReferenceTerm::create(STR("root.ComparisonExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_UP},
                {TermElement::TERM, List::create({
                   ReferenceTerm::create(STR("root.LogOp")),
                   ReferenceTerm::create(STR("root.ComparisonExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
     {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
   }));

  // ComparisonExp = AddExp + (ComparisonOp + AddExp)*v;
  this->store.setSharedValue(STR("ComparisonExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          ReferenceTerm::create(STR("root.AddExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::TERM, List::create({
                  ReferenceTerm::create(STR("root.ComparisonOp")),
                  ReferenceTerm::create(STR("root.AddExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // AddExp = MulExp + (AddOp + MulExp)*v;
  this->store.setSharedValue(STR("AddExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, List::create({
           ReferenceTerm::create(STR("root.MulExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_UP},
                {TermElement::TERM, List::create({
                   ReferenceTerm::create(STR("root.AddOp")),
                   ReferenceTerm::create(STR("root.MulExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
     {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
   }));

  // MulExp = BitwiseExp + (MulOp + BitwiseExp)*v;
  this->store.setSharedValue(STR("MulExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, List::create({
           ReferenceTerm::create(STR("root.BitwiseExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_UP},
                {TermElement::TERM, List::create({
                   ReferenceTerm::create(STR("root.MulOp")),
                   ReferenceTerm::create(STR("root.BitwiseExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
     {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
   }));

  // BitwiseExp = UnaryExp + (BitwiseOp + UnaryExp)*v;
  this->store.setSharedValue(STR("BitwiseExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, List::create({
           ReferenceTerm::create(STR("root.UnaryExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_UP},
                {TermElement::TERM, List::create({
                   ReferenceTerm::create(STR("root.BitwiseOp")),
                   ReferenceTerm::create(STR("root.UnaryExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
     {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
   }));

  // UnaryExp = PrefixOp*o + FunctionalExp + PostfixOp*o;
  this->store.setSharedValue(STR("UnaryExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, List::create({
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::MAX, std::make_shared<Integer>(1)},
             {TermElement::TERM, ReferenceTerm::create(STR("root.PrefixOp"))}
           }),
           ReferenceTerm::create(STR("root.FunctionalExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, ParsingFlags::PASS_UP},
             {TermElement::MAX, std::make_shared<Integer>(1)},
             {TermElement::TERM, ReferenceTerm::create(STR("root.PostfixOp"))}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
     {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
   }));

  // FunctionalExp = Operand + (LinkExp || ParamPassExp)*v;
  this->store.setSharedValue(STR("FunctionalExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          ReferenceTerm::create(STR("root.Operand")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::TERM, AlternateTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::TERM, List::create({
                  ReferenceTerm::create(STR("root.LinkExp")),
                  ReferenceTerm::create(STR("root.ParamPassExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // LinkExp = LinkOp + Operand;
  this->store.setSharedValue(STR("LinkExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, List::create({
          ReferenceTerm::create(STR("root.LinkOp")),
          ReferenceTerm::create(STR("root.Operand"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
  }));

  // ParamPassExp = "(" + Expression*o + ")" || "[" + Expression*o + "]";
  this->store.setSharedValue(STR("ParamPassExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::TERM, List::create({
           ConcatTerm::create({
             {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
             {TermElement::TERM, List::create({
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR("(")),
                MultiplyTerm::create({
                  {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                  {TermElement::FLAGS, ParsingFlags::PASS_UP},
                  {TermElement::MAX, std::make_shared<Integer>(1)},
                  {TermElement::TERM, ReferenceTerm::create(STR("root.Expression"))}
                }),
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR(")"))
              })}
           }),
           ConcatTerm::create({
             {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
             {TermElement::TERM, List::create({
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR("[")),
                MultiplyTerm::create({
                  {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                  {TermElement::FLAGS, ParsingFlags::PASS_UP},
                  {TermElement::MAX, std::make_shared<Integer>(1)},
                  {TermElement::TERM, ReferenceTerm::create(STR("root.Expression"))}
                }),
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR("]"))
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler},
   }));

  // Operand = Parameter || Command || Set || "(" + Expression*o + ")" || "[" + Expression*o + "]";
  this->store.setSharedValue(STR("Operand"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::TERM, List::create({
          ReferenceTerm::create(STR("root.Parameter")),
          ReferenceTerm::create(STR("root.Set")),
          ConcatTerm::create({
            {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
            {TermElement::TERM, List::create({
               TokenTerm::create(ParsingFlags::OMISSIBLE, STR("(")),
               MultiplyTerm::create({
                 {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                 {TermElement::FLAGS, ParsingFlags::PASS_UP},
                 {TermElement::MAX, std::make_shared<Integer>(1)},
                 {TermElement::TERM, ReferenceTerm::create(STR("root.SubStatement"))}
               }),
               TokenTerm::create(ParsingFlags::OMISSIBLE, STR(")"))
             })}
          }),
          ConcatTerm::create({
            {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
            {TermElement::TERM, List::create({
               TokenTerm::create(ParsingFlags::OMISSIBLE, STR("[")),
               MultiplyTerm::create({
                 {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                 {TermElement::FLAGS, ParsingFlags::PASS_UP},
                 {TermElement::MAX, std::make_shared<Integer>(1)},
                 {TermElement::TERM, ReferenceTerm::create(STR("root.SubStatement"))}
               }),
               TokenTerm::create(ParsingFlags::OMISSIBLE, STR("]"))
             })}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
  }));

  // Parameter = identifier || Literal;
  this->store.setSharedValue(STR("Parameter"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, List::create({
          TokenTerm::create(0, std::make_shared<Integer>(IDENTIFIER_TOKEN)),
          ReferenceTerm::create(STR("root.Literal"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // Literal = int_literal || float_literal || char_literal || string_literal;
  this->store.setSharedValue(STR("Literal"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, List::create({
          TokenTerm::create(0, std::make_shared<Integer>(INT_LITERAL_TOKEN)),
          TokenTerm::create(0, std::make_shared<Integer>(FLOAT_LITERAL_TOKEN)),
          TokenTerm::create(0, std::make_shared<Integer>(CHAR_LITERAL_TOKEN)),
          TokenTerm::create(0, std::make_shared<Integer>(STRING_LITERAL_TOKEN))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // AssignmentOp = ":=" || "+=" || "-=" || "*=" || "/=" || "%=" || "&=" || "|=" || "$=" || "^:=" || "<<=" || ">>=";
  this->store.setSharedValue(STR("AssignmentOp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::FLAGS, ParsingFlags::PASS_UP},
        {TermElement::TERM, List::create({
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
  this->store.setSharedValue(STR("ComparisonOp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::FLAGS, ParsingFlags::PASS_UP},
        {TermElement::TERM, List::create({
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
  this->store.setSharedValue(STR("AddOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, List::create({
          TokenTerm::create(0, STR("+")),
          TokenTerm::create(0, STR("-"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  // MulOp = "*" || "/" || "%";
  this->store.setSharedValue(STR("MulOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, List::create({
          TokenTerm::create(0, STR("*")),
          TokenTerm::create(0, STR("/")),
          TokenTerm::create(0, STR("%"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  // BitwiseOp = "|" || "^|" || "$" || "^$" || "&" || "^&" || "<<" || ">>";
  this->store.setSharedValue(STR("BitwiseOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, List::create({
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
  this->store.setSharedValue(STR("LogOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, List::create({
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
  this->store.setSharedValue(STR("PrefixOp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::FLAGS, ParsingFlags::PASS_UP},
        {TermElement::TERM, List::create({
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
  this->store.setSharedValue(STR("PostfixOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, List::create({
          TokenTerm::create(0, STR("++")),
          TokenTerm::create(0, STR("--"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  // LinkOp = "." || "->" || "=>";
  this->store.setSharedValue(STR("LinkOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP},
       {TermElement::TERM, List::create({
          TokenTerm::create(0, STR(".")),
          TokenTerm::create(0, STR("->")),
          TokenTerm::create(0, STR("=>"))
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  GrammarModule *root = this->store.getRootModule().s_cast_get<GrammarModule>();
  root->setStartRef(ReferenceParser::parseQualifier(STR("module.Program")));
}

} } // namespace
