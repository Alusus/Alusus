/**
 * @file Tests/CoreTests/Main_tests.h
 * Contain's the Core::Main namespace's unit tests.
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

// Test for a successful parsing of a simple assignment expression.
TEST_CASE("Core::Main/assignment", "Assignment Expression Successful Parsing Test")
{
  TestProcessor processor;
  vector<SharedPtr<IdentifiableObject> > results;
  vector<SharedPtr<IdentifiableObject> > results2;

  try {
    SharedPtr<IdentifiableObject> ptr = processor.processString(STR("strVar := \"Hello World\";"));
    SECTION("s1", "Data generated.")
    {
      REQUIRE(ptr.get() != 0);
    }
    results.clear();
    findProdData(ID_GENERATOR->getId(STR("AssignmentExp")), ptr, results);
    SECTION("s2", "One assignment expression in generated data.")
    {
      REQUIRE(results.size() == 1);
    }
    SECTION("s3", "One identifier token (strVar) in generated expression")
    {
      results2.clear();
      if (results.size() > 0) {
        findToken(processor.getTestGrammarStore()->IDENTIFIER_TOKEN, STR("strVar"), results[0], results2);
      }
      REQUIRE(results2.size() == 1);
    }
    SECTION("s4", "One const token (:=) in generated expression")
    {
      results2.clear();
      if (results.size() > 0) {
        findToken(UNKNOWN_ID, STR(":="), results[0], results2);
      }
      REQUIRE(results2.size() == 1);
    }
    SECTION("s5", "One string literal expression (Hello World) in generated expression")
    {
      results2.clear();
      if (results.size() > 0) {
        findToken(processor.getTestGrammarStore()->STRING_LITERAL_TOKEN,
                  STR("Hello World"), results[0], results2);
      }
      REQUIRE(results2.size() == 1);
    }
  } catch (Exception &e) {
    FAIL(e.getErrorMessage());
  }
}

// Test for a successful parsing of multiple statements.
TEST_CASE("Core::Main/successful", "Multiple Statements Successful Parsing Test")
{
  TestProcessor processor;
  vector<SharedPtr<IdentifiableObject> > results;
  vector<SharedPtr<IdentifiableObject> > results1, results2, results3, results4;
  BuildMsgReceiver buildMsgs(&processor);

  try {
    buildMsgs.clear();
    SharedPtr<IdentifiableObject> ptr = processor.processString(
      STR("strVar := \"Hello World\";"
          "intVar := 5;"
          "intVar++;"
          "--intVar;"
          "hashVar := (a:1, b:2, c:3);"
          "do s:10;"
          "do { a = 5; };"));
    SECTION("s1", "No error msgs received.")
    {
      REQUIRE(buildMsgs.getMsgCount() == 0);
    }
    SECTION("s2", "Data generated.")
    {
      REQUIRE(ptr.get() != 0);
    }
    results.clear();
    findProdData(ID_GENERATOR->getId(STR("AssignmentExp")), ptr, results);
    SECTION("s3", "Three assignment expression statements in generated data.")
    {
      REQUIRE(results.size() == 3);
    }
    SECTION("s4", "First assignment statement correct.")
    {
      results1.clear();
      results2.clear();
      results3.clear();
      if (results.size() > 0) {
        findToken(processor.getTestGrammarStore()->IDENTIFIER_TOKEN, STR("strVar"),
                  results[0], results1, 0);
        findToken(UNKNOWN_ID, STR(":="), results[0], results2, 1);
        findToken(processor.getTestGrammarStore()->STRING_LITERAL_TOKEN, STR("Hello World"),
                  results[0], results3, 2);
      }
      REQUIRE((results1.size() == 1 && results2.size() == 1 && results3.size() == 1));
    }
    SECTION("s5", "Second assignment statement correct.")
    {
      results1.clear();
      results2.clear();
      results3.clear();
      if (results.size() > 1) {
        findToken(processor.getTestGrammarStore()->IDENTIFIER_TOKEN, STR("intVar"),
                  results[1], results1, 0);
        findToken(UNKNOWN_ID, STR(":="), results[1], results2, 1);
        findToken(processor.getTestGrammarStore()->INT_LITERAL_TOKEN, STR("5"), results[1], results3, 2);
      }
      REQUIRE((results1.size() == 1 && results2.size() == 1 && results3.size() == 1));
    }
    SECTION("s6", "Hash assignment statement correct.")
    {
      results1.clear();
      if (results.size() > 2) {
        findProdData(ID_GENERATOR->getId(STR("ColonPairExp")), results[2], results1);
      }
      REQUIRE(results1.size() == 3);
    }
    results.clear();
    findProdData(ID_GENERATOR->getId(STR("UnaryExp")), ptr, results);
    SECTION("s7", "Unary expression statements in generated data.")
    {
      REQUIRE(results.size() == 2);
    }
    SECTION("s8", "Postfix op expression correct.")
    {
      results1.clear();
      results2.clear();
      if (results.size() > 0) {
        findToken(processor.getTestGrammarStore()->IDENTIFIER_TOKEN, STR("intVar"),
                  results[0], results1, 0);
        findToken(UNKNOWN_ID, STR("++"), results[0], results2, 1);
      }
      REQUIRE((results1.size() == 1 && results2.size() == 1));
    }
    SECTION("s9", "Prefix op expression correct.")
    {
      results1.clear();
      results2.clear();
      if (results.size() > 1) {
        findToken(UNKNOWN_ID, STR("--"), results[1], results2, 0);
        findToken(processor.getTestGrammarStore()->IDENTIFIER_TOKEN, STR("intVar"),
                  results[1], results1, 1);
      }
      REQUIRE((results1.size() == 1 && results2.size() == 1));
    }
    results.clear();
    findProdData(ID_GENERATOR->getId(STR("Command")), ptr, results);
    SECTION("s10", "First command statement correct.")
    {
      results1.clear();
      results2.clear();
      results3.clear();
      if (results.size() > 0) {
        findProdData(ID_GENERATOR->getId(STR("ColonPairExp")), results[0], results1);
        if (results1.size() == 1) {
          findToken(processor.getTestGrammarStore()->IDENTIFIER_TOKEN, STR("s"),
                    results1[0], results2, 0);
          findToken(processor.getTestGrammarStore()->INT_LITERAL_TOKEN, STR("10"),
                    results1[0], results3, 1);
        }
      }
      REQUIRE((results2.size() == 1 && results3.size() == 1));
    }
    SECTION("s11", "Second command statement correct.")
    {
      results1.clear();
      results2.clear();
      results3.clear();
      if (results.size() > 1) {
        findProdData(ID_GENERATOR->getId(STR("Set")), results[1], results1);
        if (results1.size() == 1) {
          findProdData(ID_GENERATOR->getId(STR("ComparisonExp")), results1[0], results2);
          if (results2.size() == 1) {
            findToken(processor.getTestGrammarStore()->IDENTIFIER_TOKEN, STR("a"),
                      results2[0], results3);
            findToken(processor.getTestGrammarStore()->INT_LITERAL_TOKEN, STR("5"),
                      results2[0], results4);
          }
        }
      }
      REQUIRE((results3.size() == 1 && results4.size() == 1));
    }
  } catch (Exception &e) {
    FAIL(e.getErrorMessage());
  }
}

// Test for an error during parsing of multiple statements.
TEST_CASE("Core::Main/error", "Multiple Statements With Syntax Error Test")
{
  TestProcessor processor;
  vector<SharedPtr<IdentifiableObject> > results;
  vector<SharedPtr<IdentifiableObject> > results1, results2, results3;
  BuildMsgReceiver buildMsgs(&processor);

  try {
    buildMsgs.clear();
    SharedPtr<IdentifiableObject> ptr = processor.processString(
      STR("strVar := \"Hello World\";\n"
          "intVar := 5;\n"
          "intVar+++;\n"
          "--intVar;\n"
          "opVar := i +;\n"
          "opVar := i + j;\n"
          "opVar := "));
    SECTION("s1", "Error msgs received.")
    {
      REQUIRE(buildMsgs.getMsgCount() == 3);
    }
    SECTION("s2", "Error msgs correct.")
    {
      SharedPtr<Common::BuildMsg> msg;
      if (buildMsgs.getMsgCount() > 0) msg = buildMsgs.getMsg(0);
      REQUIRE((msg != 0));
      REQUIRE(msg->getCode().compare("P1001") == 0);
      REQUIRE(msg->getLine() == 3);
      REQUIRE(msg->getColumn() == 10);

      msg = SharedPtr<Common::BuildMsg>(0);
      if (buildMsgs.getMsgCount() > 1) msg = buildMsgs.getMsg(1);
      REQUIRE((msg != 0));
      REQUIRE(msg->getCode().compare("P1001") == 0);
      REQUIRE(msg->getLine() == 5);
      REQUIRE(msg->getColumn() == 13);

      msg = SharedPtr<Common::BuildMsg>(0);
      if (buildMsgs.getMsgCount() > 2) msg = buildMsgs.getMsg(2);
      REQUIRE((msg != 0));
      REQUIRE(msg->getCode().compare("P1002") == 0);
    }
    SECTION("s3", "Data generated.")
    {
      REQUIRE(ptr.get() != 0);
    }
    results.clear();
    findProdData(ID_GENERATOR->getId(STR("Statement")), ptr, results);
    SECTION("s4", "Seven statements in generated data.")
    {
      REQUIRE(results.size() == 7);
    }
    SECTION("s5", "First two statements not empty.")
    {
      Bool empty = false;
      if (results.size() < 2) empty = true;
      else for (Int i = 0; i < 2; i++) {
        if (isEmpty(results[i])) empty = true;
      }
      REQUIRE(empty == false);
    }
    SECTION("s6", "Third statement empty.")
    {
      Bool empty = false;
      if (results.size() > 2 && isEmpty(results[2])) empty = true;
      REQUIRE(empty == true);
    }
    SECTION("s7", "Fourth statement not empty.")
    {
      Bool empty = true;
      if (results.size() > 3 && !isEmpty(results[3])) empty = false;
      REQUIRE(empty == false);
    }
    SECTION("s8", "Fifth statement empty.")
    {
      Bool empty = false;
      if (results.size() > 4 && isEmpty(results[4])) empty = true;
      REQUIRE(empty == true);
    }
    SECTION("s9", "Sixth statement not empty.")
    {
      Bool empty = true;
      if (results.size() > 5 && !isEmpty(results[5])) empty = false;
      REQUIRE(empty == false);
    }
    SECTION("s10", "Seven statement empty.")
    {
      Bool empty = false;
      if (results.size() > 6 && isEmpty(results[6])) empty = true;
      REQUIRE(empty == true);
    }
  } catch (Exception &e) {
    FAIL(e.getErrorMessage());
  }
}

} } // namespace
