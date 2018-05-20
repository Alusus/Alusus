/**
 * @file Tests/CoreTests/Main_tests.h
 * Contain's the Core::Main namespace's unit tests.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
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
  TestEngine engine;
  vector<SharedPtr<TiObject> > results;
  vector<SharedPtr<TiObject> > results2;

  try {
    SharedPtr<TiObject> ptr = engine.processString(STR("strVar := \"Hello World\";"), STR("testcode"));
    SECTION("s1", "Data generated.")
    {
      CHECK(ptr.get() != 0);
    }
    results.clear();
    findProdData(ID_GENERATOR->getId(STR("AssignmentExp")), ptr, results);
    SECTION("s2", "One assignment expression in generated data.")
    {
      CHECK(results.size() == 1);
    }
    SECTION("s3", "One identifier token (strVar) in generated expression")
    {
      results2.clear();
      if (results.size() > 0) {
        findToken(ID_GENERATOR->getId(STR("LexerDefs.Identifier")), STR("strVar"), results[0], results2);
      }
      CHECK(results2.size() == 1);
    }
    SECTION("s4", "One const token (:=) in generated expression")
    {
      results2.clear();
      if (results.size() > 0) {
        findToken(UNKNOWN_ID, STR(":="), results[0], results2);
      }
      CHECK(results2.size() == 1);
    }
    SECTION("s5", "One string literal expression (Hello World) in generated expression")
    {
      results2.clear();
      if (results.size() > 0) {
        findToken(ID_GENERATOR->getId(STR("LexerDefs.StringLiteral")),
                  STR("Hello World"), results[0], results2);
      }
      CHECK(results2.size() == 1);
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}

// Test for a successful parsing of a simple prefix expression.
TEST_CASE("Core::Main/prefix_op", "Prefix Operator Successful Parsing Test")
{
  TestEngine engine;
  vector<SharedPtr<TiObject> > results;
  vector<SharedPtr<TiObject> > results2;

  try {
    SharedPtr<TiObject> ptr = engine.processString(STR("--strVar;"), STR("testcode"));
    SECTION("s1", "Data generated.")
    {
      CHECK(ptr.get() != 0);
    }
    results.clear();
    findProdData(ID_GENERATOR->getId(STR("UnaryExp")), ptr, results);
    SECTION("s2", "One unary expression in generated data.")
    {
      CHECK(results.size() == 1);
    }
    SECTION("s3", "One identifier token (strVar) in generated expression")
    {
      results2.clear();
      if (results.size() > 0) {
        findToken(ID_GENERATOR->getId(STR("LexerDefs.Identifier")), STR("strVar"), results[0], results2);
      }
      CHECK(results2.size() == 1);
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}

TEST_CASE("Core::Main/do_cmd", "'do' Command Successful Parsing Test")
{
  TestEngine engine;
  vector<SharedPtr<TiObject> > results;
  vector<SharedPtr<TiObject> > results2;

  try {
    SharedPtr<TiObject> ptr = engine.processString(STR("do { a = 5; };"), STR("testcode"));
    SECTION("s1", "Data generated.")
    {
      CHECK(ptr.get() != 0);
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}

// Test for a successful parsing of multiple statements.
TEST_CASE("Core::Main/successful", "Multiple Statements Successful Parsing Test")
{
  TestEngine engine;
  vector<SharedPtr<TiObject> > results;
  vector<SharedPtr<TiObject> > results1, results2, results3, results4;
  NoticeReceiver notices(&engine);

  try {
    notices.clear();
    SharedPtr<TiObject> ptr = engine.processString(
      STR("strVar := \"Hello World\";"
          "intVar := 5;"
          "intVar++;"
          "--intVar;"
          "hashVar := (a:1, b:2, c:3);"
          "do s:10;"
          "do { a = 5; };"),
      STR("testcode"));
    SECTION("s1", "No error msgs received.")
    {
      CHECK(notices.getNoticeCount() == 0);
    }
    SECTION("s2", "Data generated.")
    {
      CHECK(ptr.get() != 0);
    }
    results.clear();
    findProdData(ID_GENERATOR->getId(STR("AssignmentExp")), ptr, results);
    SECTION("s3", "Three assignment expression statements in generated data.")
    {
      CHECK(results.size() == 3);
    }
    SECTION("s4", "First assignment statement correct.")
    {
      results1.clear();
      results2.clear();
      results3.clear();
      if (results.size() > 0) {
        findToken(ID_GENERATOR->getId(STR("LexerDefs.Identifier")), STR("strVar"),
                  results[0], results1, 0);
        findToken(UNKNOWN_ID, STR(":="), results[0], results2, 1);
        findToken(ID_GENERATOR->getId(STR("LexerDefs.StringLiteral")), STR("Hello World"),
                  results[0], results3, 2);
      }
      CHECK((results1.size() == 1 && results2.size() == 1 && results3.size() == 1));
    }
    SECTION("s5", "Second assignment statement correct.")
    {
      results1.clear();
      results2.clear();
      results3.clear();
      if (results.size() > 1) {
        findToken(ID_GENERATOR->getId(STR("LexerDefs.Identifier")), STR("intVar"),
                  results[1], results1, 0);
        findToken(UNKNOWN_ID, STR(":="), results[1], results2, 1);
        findToken(ID_GENERATOR->getId(STR("LexerDefs.IntLiteral")), STR("5"), results[1], results3, 2);
      }
      CHECK((results1.size() == 1 && results2.size() == 1 && results3.size() == 1));
    }
    SECTION("s6", "Hash assignment statement correct.")
    {
      results1.clear();
      if (results.size() > 2) {
        findProdData(ID_GENERATOR->getId(STR("ColonPairExp")), results[2], results1);
      }
      CHECK(results1.size() == 3);
    }
    results.clear();
    findProdData(ID_GENERATOR->getId(STR("UnaryExp")), ptr, results);
    SECTION("s7", "Unary expression statements in generated data.")
    {
      CHECK(results.size() == 2);
    }
    SECTION("s8", "Postfix op expression correct.")
    {
      results1.clear();
      results2.clear();
      if (results.size() > 0) {
        findToken(ID_GENERATOR->getId(STR("LexerDefs.Identifier")), STR("intVar"),
                  results[0], results1, 0);
        findToken(UNKNOWN_ID, STR("++"), results[0], results2, 1);
      }
      CHECK((results1.size() == 1 && results2.size() == 1));
    }
    SECTION("s9", "Prefix op expression correct.")
    {
      results1.clear();
      results2.clear();
      if (results.size() > 1) {
        findToken(UNKNOWN_ID, STR("--"), results[1], results2, 0);
        findToken(ID_GENERATOR->getId(STR("LexerDefs.Identifier")), STR("intVar"),
                  results[1], results1, 1);
      }
      CHECK((results1.size() == 1 && results2.size() == 1));
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
          findToken(ID_GENERATOR->getId(STR("LexerDefs.Identifier")), STR("s"),
                    results1[0], results2, 0);
          findToken(ID_GENERATOR->getId(STR("LexerDefs.IntLiteral")), STR("10"),
                    results1[0], results3, 1);
        }
      }
      CHECK((results2.size() == 1 && results3.size() == 1));
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
            findToken(ID_GENERATOR->getId(STR("LexerDefs.Identifier")), STR("a"),
                      results2[0], results3);
            findToken(ID_GENERATOR->getId(STR("LexerDefs.IntLiteral")), STR("5"),
                      results2[0], results4);
          }
        }
      }
      CHECK((results3.size() == 1 && results4.size() == 1));
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}

// Test for an error during parsing of multiple statements.
TEST_CASE("Core::Main/error", "Multiple Statements With Syntax Error Test")
{
  TestEngine engine;
  vector<SharedPtr<TiObject> > results;
  vector<SharedPtr<TiObject> > results1, results2, results3;
  NoticeReceiver notices(&engine);

  try {
    notices.clear();
    SharedPtr<TiObject> ptr = engine.processString(
      STR("strVar := \"Hello World\";\n"
          "intVar := 5;\n"
          "intVar+++;\n"
          "--intVar;\n"
          "opVar := i +;\n"
          "opVar := i + j;\n"
          "opVar := "),
      STR("testcode"));
    SECTION("s1", "Error msgs received.")
    {
      CHECK(notices.getNoticeCount() == 3);
    }
    SECTION("s2", "Error msgs correct.")
    {
      SharedPtr<Notices::Notice> msg;
      if (notices.getNoticeCount() > 0) msg = notices.getMsg(0);
      CHECK((msg != 0));
      CHECK(msg->getCode().compare("P1001") == 0);
      auto sl = msg->getSourceLocation().ti_cast_get<Core::Data::SourceLocationRecord>();
      CHECK(sl != 0);
      CHECK(sl->line == 3);
      CHECK(sl->column == 10);

      msg = SharedPtr<Notices::Notice>(0);
      if (notices.getNoticeCount() > 1) msg = notices.getMsg(1);
      CHECK((msg != 0));
      CHECK(msg->getCode().compare("P1001") == 0);
      sl = msg->getSourceLocation().ti_cast_get<Core::Data::SourceLocationRecord>();
      CHECK(sl != 0);
      CHECK(sl->line == 5);
      CHECK(sl->column == 13);

      msg = SharedPtr<Notices::Notice>(0);
      if (notices.getNoticeCount() > 2) msg = notices.getMsg(2);
      CHECK((msg != 0));
      CHECK(msg->getCode().compare("P1002") == 0);
    }
    SECTION("s3", "Data generated.")
    {
      CHECK(ptr.get() != 0);
    }
    results.clear();
    findProdData(ID_GENERATOR->getId(STR("Statement")), ptr, results);
    SECTION("s4", "Seven statements in generated data.")
    {
      CHECK(results.size() == 7);
    }
    SECTION("s5", "First two statements not empty.")
    {
      Bool empty = false;
      if (results.size() < 2) empty = true;
      else for (Int i = 0; i < 2; i++) {
        if (isEmpty(results[i])) empty = true;
      }
      CHECK(empty == false);
    }
    SECTION("s6", "Third statement empty.")
    {
      Bool empty = false;
      if (results.size() > 2 && isEmpty(results[2])) empty = true;
      CHECK(empty == true);
    }
    SECTION("s7", "Fourth statement not empty.")
    {
      Bool empty = true;
      if (results.size() > 3 && !isEmpty(results[3])) empty = false;
      CHECK(empty == false);
    }
    SECTION("s8", "Fifth statement empty.")
    {
      Bool empty = false;
      if (results.size() > 4 && isEmpty(results[4])) empty = true;
      CHECK(empty == true);
    }
    SECTION("s9", "Sixth statement not empty.")
    {
      Bool empty = true;
      if (results.size() > 5 && !isEmpty(results[5])) empty = false;
      CHECK(empty == false);
    }
    SECTION("s10", "Seven statement empty.")
    {
      Bool empty = false;
      if (results.size() > 6 && isEmpty(results[6])) empty = true;
      CHECK(empty == true);
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}

} } // namespace
