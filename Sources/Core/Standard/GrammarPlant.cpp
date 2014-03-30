/**
 * @file Core/Standard/GrammarPlant.cpp
 * Contains the implementation of class Standard::GrammarPlant.
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

//==============================================================================
// Constructor

GrammarPlant::GrammarPlant(RootManager *root) :
  Data::GrammarPlant(false),
  BIN_DIGIT_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("BIN_DIGIT_CHAR_GROUP")),
  OCT_DIGIT_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("OCT_DIGIT_CHAR_GROUP")),
  DEC_DIGIT_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("DEC_DIGIT_CHAR_GROUP")),
  HEX_DIGIT_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("HEX_DIGIT_CHAR_GROUP")),
  LETTER_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("LETTER_CHAR_GROUP")),
  ANY_CHAR_NO_ES_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("ANY_CHAR_NO_ES_CHAR_GROUP")),
  ANY_CHAR_NO_ES_OR_SINGLE_QUOTE_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("ANY_CHAR_NO_ES_OR_SINGLE_QUOTE_CHAR_GROUP")),
  ANY_CHAR_NO_ES_OR_DOUBLE_QUOTE_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("ANY_CHAR_NO_ES_OR_DOUBLE_QUOTE_CHAR_GROUP")),
  ANY_CHAR_NO_RETURN_CHAR_GROUP(Data::IdGenerator::getSingleton()->getId("ANY_CHAR_NO_RETURN_CHAR_GROUP")),
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
  this->parsingHandler = std::make_shared<GenericParsingHandler>();
  this->importHandler = std::make_shared<ImportParsingHandler>(root);
  this->createGrammar();
}


////////////////////////////////////////////////////////////////////////////////
// Member Functions

/**
 * Create a CharGroupDefinitionList and add all the required definitions for
 * the Core's grammar.
 */
void GrammarPlant::createCharGroupDefinitions()
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

  // any_char_no_return = ^('\\');
  this->charGroupDefinitions->add(ANY_CHAR_NO_RETURN_CHAR_GROUP,
                                  new InvertCharGroupUnit(
                                      new SequenceCharGroupUnit('\n', '\n')));
  // spacing = ' ', '\n', '\r', '\t';
  this->charGroupDefinitions->add(SPACING_CHAR_GROUP,
                                  new RandomCharGroupUnit(" \r\n\t"));
}


/**
 * Create a TokenDefinitionList and add all the required definitions for
 * the Core's grammar.
 */
void GrammarPlant::createTokenDefinitions()
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

  // "//" + any_char_no_return * v1 + "\n";
  this->tokenDefinitions->add(LINE_COMMENT_TOKEN,
                              new ConcatTerm(3,
                                 new ConstTerm("//"),
                                 new DuplicateTerm(
                                     new CharGroupTerm(ANY_CHAR_NO_RETURN_CHAR_GROUP)),
                                 new ConstTerm("\n")));
}


/**
 * Create a Production_Definition_List and add all the required definitions for
 * the Core's grammar. The created list will have assigned parsing handlers.
 */
void GrammarPlant::createProductionDefinitions()
{
  using namespace Core::Data;
  using namespace Core::Common;

  // TODO: Replace the generic parsing handler for the root with the appropriate one.

  //// TokenData module.

  this->manager.setValue(STR("TokenData"), GrammarModule::create({}));
  // assignmentOpList : keywords := (":=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "$=", "^:=", "<<=", ">>=");
  this->manager.setValue(STR("TokenData.assignmentOpList"), Map::create(true, {
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
  this->manager.setValue(STR("TokenData.comparisonOpList"), Map::create(true, {
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
  this->manager.setValue(STR("TokenData.addOpList"), Map::create(true, {
    {STR("+"), 0},
    {STR("-"), 0}
  }));
  // mulOpList : keywords := ("*", "/", "%");
  this->manager.setValue(STR("TokenData.mulOpList"), Map::create(true, {
    {STR("*"), 0},
    {STR("/"), 0},
    {STR("%"), 0}
  }));
  // bitwiseOpList : keywords := ("|", "^|", "$", "^$", "&", "^&", "<<", ">>");
  this->manager.setValue(STR("TokenData.bitwiseOpList"), Map::create(true, {
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
  this->manager.setValue(STR("TokenData.logOpList"), Map::create(true, {
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
  this->manager.setValue(STR("TokenData.prefixOpList"), Map::create(true, {
   {STR("++"), 0},
   {STR("--"), 0},
   {STR("+"), 0},
   {STR("-"), 0},
   {STR("^"), 0},
   {STR("^^"), 0},
   {STR("not"), 0}
  }));
  // postfixOpList : keywords := ("++", "--");
  this->manager.setValue(STR("TokenData.postfixOpList"), Map::create(true, {
    {STR("++"), 0},
    {STR("--"), 0}
  }));
  // linkOpList : keywords := ("->", ".", ".>", "<.");
  this->manager.setValue(STR("TokenData.linkOpList"), Map::create(true, {
   {STR("->"), 0},
   {STR("."), 0},
   {STR(".>"), 0},
   {STR("<."), 0}
  }));
  // lowLinkOpList : keywords := ("=>", "..", "..>", "<..");
  this->manager.setValue(STR("TokenData.lowLinkOpList"), Map::create(true, {
   {STR("=>"), 0},
   {STR(".."), 0},
   {STR("..>"), 0},
   {STR("<.>"), 0}
  }));
  // lowerLinkOpList : keywords := (":", ":>", "<:");
  this->manager.setValue(STR("TokenData.lowerLinkOpList"), Map::create(true, {
   {STR(":"), 0},
   {STR(":>"), 0},
   {STR("<:"), 0}
  }));
  // lowestLinkOpList : keywords := ("::", ::>", "<::", "in");
  this->manager.setValue(STR("TokenData.lowestLinkOpList"), Map::create(true, {
    {STR("::"), 0},
    {STR("::>"), 0},
    {STR("<::"), 0},
    {STR("in"), 0}
  }));

  this->generateConstTokensForStrings(this->manager.getPlainValue(STR("root.TokenData")));


  //// Main

  //// Program : StatementList.
  // Program : prod as StatementList(DefaultMain.DefaultStatement);
  this->manager.setValue(STR("Program"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ReferenceTerm::create(STR("root.Main.StatementList"))},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  this->manager.setValue(STR("Main"), GrammarModule::create({}));

  //// StatementList : Statement { ";" Statement }.
  // StatementList : prod (stmt:production[Statement]) as
  //     stmt*(0, 1) + (lexer.Constant(";") + stmt*(0, 1))*(0, endless);
  this->manager.setValue(STR("Main.StatementList"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::ESPI, 1},
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::MAX, std::make_shared<Integer>(1)},
            {TermElement::TERM, ReferenceTerm::create(STR("args.stmt"))}}),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ERROR_SYNC_TERM},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
               {TermElement::TERM, List::create({
                  TokenTerm::create(ParsingFlags::OMISSIBLE, STR(";")),
                  MultiplyTerm::create({
                    {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                    {TermElement::FLAGS, ParsingFlags::PASS_UP},
                    {TermElement::MAX, std::make_shared<Integer>(1)},
                    {TermElement::TERM, ReferenceTerm::create(STR("args.stmt"))}
                  })
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, Map::create(false, {
       {STR("stmt"), ReferenceParser::parseQualifier(STR("module.Statement"))}})},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  //// Statement : { Phrase }.
  // Statement : @prefix(heap.Modifiers.(DefaultModifierCmd|StmtModifierCmd))
  //             @limit[user.parent=self,child.terms=self]
  //     prod (phrases:list[hash[prd:prod[Phrase], min:integer, max:integer, pty:integer]]) as
  //         concat(phrases:phrase)->( @priority(phrase.pty,0) phrase.prd*(phrase.min, phrase.max) );
  this->manager.setValue(STR("Main.Statement"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::ESPI, 1000},
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::REF, ReferenceParser::parseQualifier(STR("stack.phrase"))},
      {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.phrases"))},
      {TermElement::TERM, MultiplyTerm::create({
         {TermElement::PRIORITY, ReferenceParser::parseQualifier(STR("stack.phrase.pty"))},
         {TermElement::FLAGS, ParsingFlags::PASS_UP},
         {TermElement::MIN, ReferenceParser::parseQualifier(STR("stack.phrase.min"))},
         {TermElement::MAX, ReferenceParser::parseQualifier(STR("stack.phrase.max"))},
         {TermElement::TERM, ReferenceTerm::create(STR("stack.phrase.prd"))}
       })}
    })},
   {SymbolDefElement::VARS, Map::create(false, {
      {STR("phrases"), List::create({
         Map::create(false, {
           {STR("prd"), ReferenceParser::parseQualifier(STR("module.Phrase"))},
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
  this->manager.setValue(STR("Main.Phrase"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
       {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.pty"))},
       {TermElement::TERM, List::create({
          ReferenceTerm::create(STR("args.cmd")),
          ReferenceTerm::create(STR("args.exp"))
        })}
     })},
    {SymbolDefElement::VARS, Map::create(false, {
       {STR("cmd"), ReferenceParser::parseQualifier("module.LeadingCmdGrp")},
       {STR("exp"), ReferenceParser::parseQualifier("root.Expression")},
       {STR("pty"), 0}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // LeadingCommandGroup
  this->manager.setValue(STR("Main.LeadingCmdGrp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
      {TermElement::REF, ReferenceParser::parseQualifier(STR("stack.cmd"))},
      {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.cmds"))},
      {TermElement::TERM, ReferenceTerm::create(STR("stack.cmd"))}
    })},
   {SymbolDefElement::VARS, Map::create(false, {
      {STR("cmds"), List::create({
         ReferenceParser::parseQualifier(STR("module.Do")),
         ReferenceParser::parseQualifier(STR("module.Import"))
       })}
    })},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  //// Do = "do" + Subject
  this->manager.setValue(STR("Main.Do"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root.Cmd"))},
    {SymbolDefElement::VARS, Map::create(false, {
       {STR("kwd"), std::make_shared<String>(STR("do"))},
       {STR("prms"), List::create({
          Map::create(false, {
            {STR("prd"), ReferenceParser::parseQualifier(STR("root.Subject"))},
            {STR("min"), std::make_shared<Integer>(1)},
            {STR("max"), std::make_shared<Integer>(1)},
            {STR("pty"), std::make_shared<Integer>(1)}
          })
        })}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}
  }));

  //// Import = "import" + Subject
  this->manager.setValue(STR("Main.Import"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root.Cmd"))},
    {SymbolDefElement::VARS, Map::create(false, {
       {STR("kwd"), std::make_shared<String>(STR("import"))},
       {STR("prms"), List::create({
          Map::create(false, {
            {STR("prd"), ReferenceParser::parseQualifier(STR("root.Subject"))},
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
  this->manager.setValue(STR("Cmd"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          TokenTerm::create(ParsingFlags::OMISSIBLE, std::make_shared<Integer>(0), ReferenceParser::parseQualifier(STR("args.kwd"))),
          ConcatTerm::create({
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::REF, ReferenceParser::parseQualifier(STR("stack.p"))},
            {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.prms"))},
            {TermElement::TERM, MultiplyTerm::create({
               {TermElement::PRIORITY, ReferenceParser::parseQualifier(STR("stack.p.pty"))},
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::MIN, ReferenceParser::parseQualifier(STR("stack.p.min"))},
               {TermElement::MAX, ReferenceParser::parseQualifier(STR("stack.p.max"))},
               {TermElement::TERM, ReferenceTerm::create(STR("stack.p.prd"))}
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
  this->manager.setValue(STR("Expression"), GrammarModule::create({{STR("@start"), ReferenceParser::parseQualifier(STR("module.Exp"))}}));

  // Exp : @single prod as LowestLinkExp + (@priority(in,0) lexer.Constant("\\")*(0,1));
  this->manager.setValue(STR("Expression.Exp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, List::create({
         ReferenceTerm::create(STR("module.LowestLinkExp")),
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
  this->manager.setValue(STR("Expression.LowestLinkExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, List::create({
         ReferenceTerm::create(STR("module.ConditionalExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, ParsingFlags::PASS_UP},
           {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
              {TermElement::TERM, List::create({
                 TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("lowestLinkOp"))),
                                   ReferenceParser::parseQualifier(STR("root.TokenData.lowestLinkOpList"))),
                 ReferenceTerm::create(STR("module.ConditionalExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, Map::create(false, {{STR("enable"), 0}})},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // ConditionalExp : @single @prefix(heap.Modifiers.ConditionalModifierCmd)
  //     prod (enable:integer[0<=n<=1]:=1) as
  //     ListExp + (@priority(in,0) (lexer.Constant("?") + ListExp)*(0,enable));
  this->manager.setValue(STR("Expression.ConditionalExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, List::create({
         ReferenceTerm::create(STR("module.ListExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, ParsingFlags::PASS_UP},
           {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
              {TermElement::TERM, List::create({
                 TokenTerm::create(ParsingFlags::OMISSIBLE, STR("?")),
                 ReferenceTerm::create(STR("module.ListExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, Map::create(false, {{STR("enable"), std::make_shared<Integer>(1)}})},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // ListExp : @single @prefix(heap.Modifiers.ListModifierCmd)
  //     prod (enable:integer:=endless) as
  //     (@priority(in,0) lexer.Constant(",")*(0,enable)) + LowerLinkExp +
  //     (@priority(in,0) (lexer.Constant(",") + LowerLinkExp*(0,1))*(0,enable));
  this->manager.setValue(STR("Expression.ListExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, List::create({
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(0)},
            {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
            {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.enable"))},
            {TermElement::TERM, TokenTerm::create(ParsingFlags::OMISSIBLE, STR(","))}
          }),
          ReferenceTerm::create(STR("module.LowerLinkExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
            {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.enable"))},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
               {TermElement::TERM, List::create({
                  TokenTerm::create(ParsingFlags::OMISSIBLE, STR(",")),
                  MultiplyTerm::create({
                    {TermElement::PRIORITY, std::make_shared<Integer>(1)},
                    {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
                    {TermElement::MAX, std::make_shared<Integer>(1)},
                    {TermElement::TERM, ReferenceTerm::create(STR("module.LowerLinkExp"))}
                  })
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, Map::create(false, {{STR("enable"), 0}})},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // LowerLinkExp : @single @prefix(heap.Modifiers.LowerLinkModifierCmd)
  //     prod (enable:integer:=endless) as
  //     AssignmentExp + (@priority(in,0) (LowerLinkOp + AssignmentExp)*(0,enable));
  this->manager.setValue(STR("Expression.LowerLinkExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          ReferenceTerm::create(STR("module.AssignmentExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
            {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.enable"))},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
               {TermElement::TERM, List::create({
                  TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("lowerLinkOp"))),
                  ReferenceParser::parseQualifier(STR("root.TokenData.lowerLinkOpList"))),
                  ReferenceTerm::create(STR("module.AssignmentExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, Map::create(false, {{STR("enable"), 0}})},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // AssignmentExp : @single @prefix(heap.Modifiers.AssignmentModifierCmd)
  //     prod (enable:integer:=endless) as
  //     LogExp + (@priority(in,0) (AssignmentOp + LogExp)*(0,enable));
  this->manager.setValue(STR("Expression.AssignmentExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          ReferenceTerm::create(STR("module.LogExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
            {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.enable"))},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
               {TermElement::TERM, List::create({
                  TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("assignmentOp"))),
                  ReferenceParser::parseQualifier(STR("root.TokenData.assignmentOpList"))),
                  ReferenceTerm::create(STR("module.LogExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, Map::create(false, {{STR("enable"), 0}})},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // LogExp : @single @prefix(heap.Modifiers.LogModifierCmd)
  //     prod (enable:integer:=endless) as
  //     ComparisonExp + (@priority(in,0) (LogOp + ComparisonExp)*(0,enable));
  this->manager.setValue(STR("Expression.LogExp"), SymbolDefinition::create({
     {SymbolDefElement::TERM, ConcatTerm::create({
        {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
        {TermElement::TERM, List::create({
           ReferenceTerm::create(STR("module.ComparisonExp")),
           MultiplyTerm::create({
             {TermElement::PRIORITY, std::make_shared<Integer>(1)},
             {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
             {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.enable"))},
             {TermElement::TERM, ConcatTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
                {TermElement::TERM, List::create({
                   TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("logOp"))),
                   ReferenceParser::parseQualifier(STR("root.TokenData.logOpList"))),
                   ReferenceTerm::create(STR("module.ComparisonExp"))
                 })}
              })}
           })
         })}
      })},
     {SymbolDefElement::VARS, Map::create(false, {{STR("enable"), 0}})},
     {SymbolDefElement::HANDLER, this->parsingHandler},
     {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
   }));

  // ComparisonExp : @single @prefix(heap.Modifiers.ComparisonModifierCmd)
  //     prod (enable:integer:=endless) as
  //     LowLinkExp + (@priority(in,0) (ComparisonOp + LowLinkExp)*(0,enable));
  this->manager.setValue(STR("Expression.ComparisonExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          ReferenceTerm::create(STR("module.LowLinkExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
            {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.enable"))},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
               {TermElement::TERM, List::create({
                  TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("comparisonOp"))),
                  ReferenceParser::parseQualifier(STR("root.TokenData.comparisonOpList"))),
                  ReferenceTerm::create(STR("module.LowLinkExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, Map::create(false, {{STR("enable"), 0}})},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // LowLinkExp : @single @prefix(heap.Modifiers.LowLinkModifierCmd)
  //     prod (enable:integer:=endless) as
  //     AddExp + (@priority(in,0) (LowLinkOp + AddExp)*(0,enable));
  this->manager.setValue(STR("Expression.LowLinkExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          ReferenceTerm::create(STR("module.AddExp")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, std::make_shared<Integer>(1)},
            {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
            {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.enable"))},
            {TermElement::TERM, ConcatTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
               {TermElement::TERM, List::create({
                  TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("lowLinkOp"))),
                  ReferenceParser::parseQualifier(STR("root.TokenData.lowLinkOpList"))),
                  ReferenceTerm::create(STR("module.AddExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, Map::create(false, {{STR("enable"), 0}})},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // AddExp : @single @prefix(heap.Modifiers.AddModifierCmd)
  //     prod (enable:integer:=endless) as
  //     MulExp + (@priority(in,0) (AddOp + MulExp)*(0,enable));
  this->manager.setValue(STR("Expression.AddExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, List::create({
         ReferenceTerm::create(STR("module.MulExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
           {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
              {TermElement::TERM, List::create({
                 TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("addOp"))),
                 ReferenceParser::parseQualifier(STR("root.TokenData.addOpList"))),
                 ReferenceTerm::create(STR("module.MulExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, Map::create(false, {{STR("enable"), 0}})},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // MulExp : @single @prefix(heap.Modifiers.MulModifierCmd)
  //     prod (enable:integer:=endless) as
  //     BitwiseExp + (@priority(in,0) (MulOp + BitwiseExp)*(0,enable));
  this->manager.setValue(STR("Expression.MulExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, List::create({
         ReferenceTerm::create(STR("module.BitwiseExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
           {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
              {TermElement::TERM, List::create({
                 TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("mulOp"))),
                 ReferenceParser::parseQualifier(STR("root.TokenData.mulOpList"))),
                 ReferenceTerm::create(STR("module.BitwiseExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, Map::create(false, {{STR("enable"), 0}})},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // BitwiseExp : @single @prefix(heap.Modifiers.BitwiseModifierCmd)
  //     prod (enable:integer:=endless) as
  //     UnaryExp + (@priority(in,0) (BitwiseOp + UnaryExp)*(0,enable));
  this->manager.setValue(STR("Expression.BitwiseExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, List::create({
         ReferenceTerm::create(STR("module.UnaryExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM|ParsingFlags::PASS_UP},
           {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.enable"))},
           {TermElement::TERM, ConcatTerm::create({
              {TermElement::FLAGS, ParsingFlags::PASS_UP|ParsingFlags::OMISSIBLE},
              {TermElement::TERM, List::create({
                 TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("bitwiseOp"))),
                 ReferenceParser::parseQualifier(STR("root.TokenData.bitwiseOpList"))),
                 ReferenceTerm::create(STR("module.UnaryExp"))
               })}
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, Map::create(false, {{STR("enable"), 0}})},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // UnaryExp : @single @prefix(heap.Modifiers.UnaryModifierCmd)
  //     prod (enable1:integer[0<=n<=1]:=1, enable2:integer[0<=n<=1]:=1) as
  //     (@priority(in,0) PrefixOp*(0,enable1)) + FunctionalExp + (@priority(in,0) PostfixOp*(0,enable2));
  this->manager.setValue(STR("Expression.UnaryExp"), SymbolDefinition::create({
   {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, List::create({
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, ParsingFlags::PASS_UP},
           {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.enable1"))},
           {TermElement::TERM, ReferenceTerm::create(STR("module.PrefixOp"))}
         }),
         ReferenceTerm::create(STR("module.FunctionalExp")),
         MultiplyTerm::create({
           {TermElement::PRIORITY, std::make_shared<Integer>(1)},
           {TermElement::FLAGS, ParsingFlags::PASS_UP},
           {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.enable2"))},
           {TermElement::TERM, ReferenceTerm::create(STR("module.PostfixOp"))}
         })
       })}
    })},
   {SymbolDefElement::VARS, Map::create(false, {
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
  this->manager.setValue(STR("Expression.FunctionalExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          ReferenceTerm::create(STR("args.operand")),
          MultiplyTerm::create({
            {TermElement::PRIORITY, ReferenceParser::parseQualifier(STR("args.pty2"))},
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::MAX, ReferenceParser::parseQualifier(STR("args.dup"))},
            {TermElement::TERM, AlternateTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.fltr2"))},
               {TermElement::TERM, List::create({
                  ReferenceTerm::create(STR("module.LinkExp")),
                  ReferenceTerm::create(STR("module.PostfixTildeExp")),
                  ReferenceTerm::create(STR("module.ParamPassExp"))
                })}
             })}
          })
        })}
     })},
    {SymbolDefElement::VARS, Map::create(false, {
       {STR("operand"), ReferenceParser::parseQualifier(STR("root.Subject"))},
       {STR("pty2"), std::make_shared<Integer>(1)},
       {STR("dup"), 0},
       {STR("fltr2"), 0}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // LinkExp : @single prod (operand:production[heap.Subject]:=heap.Subject) as LinkOp + operand;
  this->manager.setValue(STR("Expression.LinkExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::TERM, List::create({
          TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("linkOp"))),
          ReferenceParser::parseQualifier(STR("root.TokenData.linkOpList"))),
          ReferenceTerm::create(STR("args.operand"))
        })}
     })},
    {SymbolDefElement::VARS, Map::create(false, {{STR("operand"), ReferenceParser::parseQualifier(STR("root.Subject"))}})},
    {SymbolDefElement::HANDLER, this->parsingHandler}}));

  //// ParamPassExp : "(" [Expression] ")" | "[" [Expression] "]".
  // ParamPassExp : @single prod (expr:production[Expression||Statement]:=heap.Expression, fltr:filter:=null) as
  //     @filter(fltr) lexer.Constant("(") + expr*(0,1) + lexer.Constant(")") ||
  //                   lexer.Constant("[") + expr*(0,1) + lexer.Constant("]");
  this->manager.setValue(STR("Expression.ParamPassExp"), SymbolDefinition::create({
  {SymbolDefElement::TERM, AlternateTerm::create({
     {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM},
     {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.fltr"))},
     {TermElement::TERM, List::create({
        ConcatTerm::create({
          {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
          {TermElement::TERM, List::create({
             TokenTerm::create(ParsingFlags::OMISSIBLE, STR("(")),
             MultiplyTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::MAX, std::make_shared<Integer>(1)},
               {TermElement::TERM, ReferenceTerm::create(STR("args.expr"))}
             }),
             TokenTerm::create(ParsingFlags::OMISSIBLE, STR(")"))
           })}
        }),
        ConcatTerm::create({
          {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
          {TermElement::TERM, List::create({
             TokenTerm::create(ParsingFlags::OMISSIBLE, STR("[")),
             MultiplyTerm::create({
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::MAX, std::make_shared<Integer>(1)},
               {TermElement::TERM, ReferenceTerm::create(STR("args.expr"))}
             }),
             TokenTerm::create(ParsingFlags::OMISSIBLE, STR("]"))
           })}
        })
      })}
   })},
  {SymbolDefElement::VARS, Map::create(false, {
     {STR("expr"), ReferenceParser::parseQualifier(STR("root.Expression"))},
     {STR("fltr"), 0}})},
  {SymbolDefElement::HANDLER, this->parsingHandler}}));

  //// PostfixTildeExp :
  //// "~" keyword {Subject}.
  //// "~(" Expression {Subject} ")".
  //PostfixTildeExp : @single prod (cmd:production[PostfixTildeCmd]:=DefaultPostfixTildeCmd) as
  //    lexer.Constant("~") + cmd;
  this->manager.setValue(STR("Expression.PostfixTildeExp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
      {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
      {TermElement::TERM, List::create({
         TokenTerm::create(ParsingFlags::OMISSIBLE, STR("~")),
         ReferenceTerm::create(STR("args.cmd"))
      })}
    })},
    {SymbolDefElement::VARS, Map::create(false, {
      {STR("cmd"), ReferenceParser::parseQualifier(STR("module.DefaultPostfixTildeCmd"))}
    })},
    {SymbolDefElement::HANDLER, this->parsingHandler}}));
  //OpenPostfixTildeCmd : @limit[user.parent=PostfixTildeCmd]
  //    prod (expr:production[Expression], args:list[hash[sbj:valid_subject, min:integer,
  //                                                      max:integer, pty:integer]]) as
  //    lexer.Constant("(") + expr + concat (args:a)->( @priority(pty,0) a.sbj*(a.min,a.max) ) +
  //    lexer.Constant(")");
  this->manager.setValue(STR("Expression.OpenPostfixTildeCmd"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR("(")),
          ReferenceTerm::create(STR("args.expr")),
          ConcatTerm::create({
            {TermElement::FLAGS, ParsingFlags::PASS_UP},
            {TermElement::REF, ReferenceParser::parseQualifier(STR("stack.p"))},
            {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.prms"))},
            {TermElement::TERM, MultiplyTerm::create({
               {TermElement::PRIORITY, ReferenceParser::parseQualifier(STR("stack.p.pty"))},
               {TermElement::FLAGS, ParsingFlags::PASS_UP},
               {TermElement::MIN, ReferenceParser::parseQualifier(STR("stack.p.min"))},
               {TermElement::MAX, ReferenceParser::parseQualifier(STR("stack.p.max"))},
               {TermElement::TERM, ReferenceTerm::create(STR("stack.p.prd"))}
             })}
          }),
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR(")"))
        })}
     })},
    {SymbolDefElement::VARS, Map::create(false, {
       {STR("expr"), ReferenceParser::parseQualifier(STR("root.Expression"))},
       {STR("prms"), List::create({Map::create(false, {
                                     {STR("pty"), std::make_shared<Integer>(1)},
                                     {STR("min"), 0},
                                     {STR("max"), 0},
                                     {STR("prd"), ReferenceParser::parseQualifier(STR("root.Expression"))}
                                   })})}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler}}));
  //DefaultPostfixTildeCmd=>PostfixTildeCmd : prod_group;
  this->manager.setValue(STR("Expression.DefaultPostfixTildeCmd"), SymbolDefinition::create({
   {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
      {TermElement::REF, ReferenceParser::parseQualifier(STR("stack.cmd"))},
      {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.cmds"))},
      {TermElement::TERM, ReferenceTerm::create(STR("stack.cmd"))}
    })},
   {SymbolDefElement::VARS, Map::create(false, {
      {STR("cmds"), List::create({ReferenceParser::parseQualifier(STR("module.OpenPostfixTildeCmd"))})}
    })},
   {SymbolDefElement::HANDLER, this->parsingHandler},
   {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  //// Operators :
  // PrefixOp : prod ref heap.ConstantKeywordGroup(heap.TokenData.prefixOpList);
  this->manager.setValue(STR("Expression.PrefixOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("prefixOp"))),
      ReferenceParser::parseQualifier(STR("root.TokenData.prefixOpList")))},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));
  // PostfixOp : prod ref heap.ConstantKeywordGroup(heap.TokenData.postfixOpList);
  this->manager.setValue(STR("Expression.PostfixOp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, TokenTerm::create(0, std::make_shared<Integer>(ID_GENERATOR->getId(STR("postfixOp"))),
      ReferenceParser::parseQualifier(STR("root.TokenData.postfixOpList")))},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));


  // Subject : Parameter | Command | Expression | Statement | Set.
  this->manager.setValue(STR("Subject"), GrammarModule::create({{STR("@start"), ReferenceParser::parseQualifier(STR("module.Subject1"))}}));

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
  this->manager.setValue(STR("Subject.Subject1"), SymbolDefinition::create({
   {SymbolDefElement::TERM, AlternateTerm::create({
      {TermElement::FLAGS, TermFlags::ONE_ROUTE_TERM},
      {TermElement::TERM, List::create({
         AlternateTerm::create({
           {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
           {TermElement::REF, ReferenceParser::parseQualifier(STR("stack.s"))},
           {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.sbj1"))},
           {TermElement::TERM, ReferenceTerm::create(STR("stack.s"))}
         }),
         ConcatTerm::create({
           {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
           {TermElement::TERM, List::create({
              TokenTerm::create(ParsingFlags::OMISSIBLE, STR("(")),
              MultiplyTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_UP},
                {TermElement::MIN, ReferenceParser::parseQualifier(STR("args.frc2"))},
                {TermElement::MAX, std::make_shared<Integer>(1)},
                {TermElement::TERM, AlternateTerm::create({
                   {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
                   {TermElement::REF, ReferenceParser::parseQualifier(STR("stack.s2"))},
                   {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.sbj2"))},
                   {TermElement::TERM, ReferenceTerm::create(STR("stack.s2"))}
                 })}
              }),
              TokenTerm::create(ParsingFlags::OMISSIBLE, STR(")"))
            })}
         }),
         ConcatTerm::create({
           {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
           {TermElement::TERM, List::create({
              TokenTerm::create(ParsingFlags::OMISSIBLE, STR("[")),
              MultiplyTerm::create({
                {TermElement::FLAGS, ParsingFlags::PASS_UP},
                {TermElement::MIN, ReferenceParser::parseQualifier(STR("args.frc3"))},
                {TermElement::MAX, std::make_shared<Integer>(1)},
                {TermElement::TERM, AlternateTerm::create({
                   {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
                   {TermElement::REF, ReferenceParser::parseQualifier(STR("stack.s3"))},
                   {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.sbj3"))},
                   {TermElement::TERM, ReferenceTerm::create(STR("stack.s3"))}
                 })}
              }),
              TokenTerm::create(ParsingFlags::OMISSIBLE, STR("]"))
            })}
         })
       })}
    })},
   {SymbolDefElement::VARS, Map::create(false, {
      {STR("sbj1"), List::create({
         ReferenceParser::parseQualifier(STR("module.SubjectCmdGrp")),
         ReferenceParser::parseQualifier(STR("module.Parameter")),
         ReferenceParser::parseQualifier(STR("root.Set"))
       })},
      {STR("sbj2"), List::create({ReferenceParser::parseQualifier(STR("root.Expression"))})},
      {STR("sbj3"), List::create({ReferenceParser::parseQualifier(STR("root.Expression"))})},
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
  this->manager.setValue(STR("Subject.Subject2"), SymbolDefinition::create({
     {SymbolDefElement::TERM, AlternateTerm::create({
        {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
        {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.fltr"))},
        {TermElement::TERM, List::create({
           ReferenceTerm::create(STR("args.sbj")),
           ConcatTerm::create({
             {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
             {TermElement::TERM, List::create({
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR("(")),
                MultiplyTerm::create({
                  {TermElement::FLAGS, ParsingFlags::PASS_UP},
                  {TermElement::MIN, ReferenceParser::parseQualifier(STR("args.frc"))},
                  {TermElement::MAX, std::make_shared<Integer>(1)},
                  {TermElement::TERM, ReferenceTerm::create(STR("args.sbj"))}
                }),
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR(")"))
              })}
           }),
           ConcatTerm::create({
             {TermElement::FLAGS, ParsingFlags::OMISSIBLE|ParsingFlags::PASS_UP},
             {TermElement::TERM, List::create({
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR("[")),
                MultiplyTerm::create({
                  {TermElement::FLAGS, ParsingFlags::PASS_UP},
                  {TermElement::MIN, ReferenceParser::parseQualifier(STR("args.frc"))},
                  {TermElement::MAX, std::make_shared<Integer>(1)},
                  {TermElement::TERM, ReferenceTerm::create(STR("args.sbj"))}
                }),
                TokenTerm::create(ParsingFlags::OMISSIBLE, STR("]"))
              })}
           })
         })}
      })},
     {SymbolDefElement::HANDLER, this->parsingHandler}
   }));

  // SubjectCommandGroup
  this->manager.setValue(STR("Subject.SubjectCmdGrp"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
       {TermElement::REF, ReferenceParser::parseQualifier(STR("stack.cmd"))},
       {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.cmds"))},
       {TermElement::TERM, ReferenceTerm::create(STR("stack.cmd"))}
     })},
    {SymbolDefElement::VARS, Map::create(false, {{ STR("cmds"), List::create({}) }} )},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  //// Parameter: Identifier | Literal.
  // Parameter : @single @prefix(heap.Modifiers.ParameterModifierCmd)
  //     prod (fltr:filter:=null, cnsts:keywords:=null) as
  //     @filter(fltr) lexer.Identifier || Literal || heap.ConstantKeywordGroup(cnsts);
  this->manager.setValue(STR("Subject.Parameter"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
       {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.fltr"))},
       {TermElement::TERM, List::create({
          TokenTerm::create(0, std::make_shared<Integer>(IDENTIFIER_TOKEN)),
          ReferenceTerm::create(STR("module.Literal"))
        })}
     })},
    {SymbolDefElement::VARS, Map::create(false, {{ STR("fltr"), 0 }} )},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));

  // Literal:
  // Literal : @single prod (fltr:filter:=null) as
  //     @filter(fltr) lexer.IntLiteral || lexer.FloatLiteral || lexer.CharLiteral || lexer.StringLiteral ||
  //                   lexer.CustomLiteral;
  this->manager.setValue(STR("Subject.Literal"), SymbolDefinition::create({
    {SymbolDefElement::TERM, AlternateTerm::create({
       {TermElement::FLAGS, ParsingFlags::PASS_UP|TermFlags::ONE_ROUTE_TERM},
       {TermElement::DATA, ReferenceParser::parseQualifier(STR("args.fltr"))},
       {TermElement::TERM, List::create({
          TokenTerm::create(0, std::make_shared<Integer>(INT_LITERAL_TOKEN)),
          TokenTerm::create(0, std::make_shared<Integer>(FLOAT_LITERAL_TOKEN)),
          TokenTerm::create(0, std::make_shared<Integer>(CHAR_LITERAL_TOKEN)),
          TokenTerm::create(0, std::make_shared<Integer>(STRING_LITERAL_TOKEN))
        })}
     })},
    {SymbolDefElement::VARS, Map::create(false, {{ STR("fltr"), 0 }} )},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));


  // Set : "{" StatementList "}".
  // Set : @limit[child.terms=self,user.parent=self] @prefix(heap.Modifiers.DefaultModifierCmd)
  //     prod (stmt:production[Statement], ovrd:grammar_override) as
  //     lexer.Constant("{") + @override(ovrd) StatementList(stmt) + lexer.Constant("}");
  this->manager.setValue(STR("Set"), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConcatTerm::create({
       {TermElement::FLAGS, ParsingFlags::OMISSIBLE},
       {TermElement::TERM, List::create({
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR("{")),
          ReferenceTerm::create(STR("args.stmt")),
          TokenTerm::create(ParsingFlags::OMISSIBLE, STR("}"))
        })}
     })},
    {SymbolDefElement::VARS, Map::create(false, {
       {STR("stmt"), ReferenceParser::parseQualifier(STR("root.Main.StatementList"))}
     })},
    {SymbolDefElement::HANDLER, this->parsingHandler},
    {SymbolDefElement::FLAGS, ParsingFlags::OMISSIBLE}
  }));


  GrammarModule *root = this->manager.getRootModule().s_cast_get<GrammarModule>();
  root->setStartRef(ReferenceParser::parseQualifier(STR("module.Program")));
}

} } // namespace
