/**
 * @file Tests/CoreTests/Standard_tests.h
 * Contain's the Core::Standard namespace's unit tests.
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
using namespace Core::Parser;
using namespace Core::Standard;

TEST_CASE("Core::Standard::ParsedDataBrowser/static", "Successful Search Using Static Functions")
{
  // Prepare a data tree.
  SharedPtr<ParsedItem> data(
    new ParsedList(ID_GENERATOR->getId(STR("root")), 3,
                   new ParsedRoute(ID_GENERATOR->getId(STR("parent.1")), 1,
                                   new ParsedToken(UNKNOWN_ID, ID_GENERATOR->getId(STR("token1")), STR("text"))),
                   new ParsedList(ID_GENERATOR->getId(STR("parent.2")), 2,
                                  new ParsedRoute(ID_GENERATOR->getId(STR("child.1")), 0),
                                  new ParsedToken(ID_GENERATOR->getId(STR("child.2")), ID_GENERATOR->getId(STR("token2")))),
                   new ParsedToken(ID_GENERATOR->getId(STR("parent.3")), ID_GENERATOR->getId(STR("token3")))));

  SharedPtr<IdentifiableObject> result;
  IdentifiableObject *null = 0;

  try {
    SECTION("s1", "Successful one level search.")
    {
      result = ParsedDataBrowser::getValue<IdentifiableObject>(STR("root"), data);
      REQUIRE(result != null);
    }
    SECTION("s2", "Unseccessful one level search.")
    {
      result = ParsedDataBrowser::getValue<IdentifiableObject>(STR("noroot"), data);
      REQUIRE(result == null);
    }
    SECTION("s3", "Successful deep search by id only.")
    {
      result = ParsedDataBrowser::getValue<IdentifiableObject>(STR("root>parent.2>child.2"), data);
      REQUIRE(result != null);
    }
    SECTION("s4", "Unsuccessful deep search by id only.")
    {
      result = ParsedDataBrowser::getValue<IdentifiableObject>(STR("root>parent.2>child.3"), data);
      REQUIRE(result == null);
    }
    SECTION("s5", "Successful deep search by id and index.")
    {
      result = ParsedDataBrowser::getValue<IdentifiableObject>(STR("root>1>0"), data);
      REQUIRE(result != null);
    }
    SECTION("s6", "Unsuccessful deep search by id and index.")
    {
      result = ParsedDataBrowser::getValue<IdentifiableObject>(STR("root>1>3"), data);
      REQUIRE(result == null);
    }
    SECTION("s7", "Successful deep search by id and index:id.")
    {
      result = ParsedDataBrowser::getValue<IdentifiableObject>(STR("root>1>0:child.1"), data);
      REQUIRE(result != null);
    }
    SECTION("s8", "Unsuccessful deep search by id and index:id.")
    {
      result = ParsedDataBrowser::getValue<IdentifiableObject>(STR("root>1>0:child.2"), data);
      REQUIRE(result == null);
    }
    SECTION("s9", "Unsuccessful extra deep search.")
    {
      result = ParsedDataBrowser::getValue<IdentifiableObject>(STR("root>parent.3>0"), data);
      REQUIRE(result == null);
    }
  } catch (Exception &e) {
    FAIL(e.getErrorMessage());
  }
}


TEST_CASE("Core::Standard::ParsedDataBrowser/local", "Successful Search Using Local Functions")
{
  // Prepare a data tree.
  SharedPtr<ParsedItem> data(
    new ParsedList(ID_GENERATOR->getId(STR("root")), 3,
                   new ParsedRoute(ID_GENERATOR->getId(STR("parent.1")), 1,
                                   new ParsedToken(UNKNOWN_ID, ID_GENERATOR->getId(STR("token1")), STR("text"))),
                   new ParsedList(ID_GENERATOR->getId(STR("parent.2")), 2,
                                  new ParsedRoute(ID_GENERATOR->getId(STR("child.1")), 0),
                                  new ParsedToken(ID_GENERATOR->getId(STR("child.2")), ID_GENERATOR->getId(STR("token2")))),
                   new ParsedToken(ID_GENERATOR->getId(STR("parent.3")), ID_GENERATOR->getId(STR("token3")))));

  ParsedDataBrowser browser;
  SharedPtr<IdentifiableObject> result;
  IdentifiableObject *null = 0;

  try {
    SECTION("s1", "Successful one level search.")
    {
      browser.initialize(STR("root"));
      result = browser.getValue<IdentifiableObject>(data);
      REQUIRE(result != null);
    }
    SECTION("s2", "Unseccessful one level search.")
    {
      browser.initialize(STR("noroot"));
      result = browser.getValue<IdentifiableObject>(data);
      REQUIRE(result == null);
    }
    SECTION("s3", "Successful deep search by id only.")
    {
      browser.initialize(STR("root>parent.2>child.2"));
      result = browser.getValue<IdentifiableObject>(data);
      REQUIRE(result != null);
    }
    SECTION("s4", "Unsuccessful deep search by id only.")
    {
      browser.initialize(STR("root>parent.2>child.3"));
      result = browser.getValue<IdentifiableObject>(data);
      REQUIRE(result == null);
    }
    SECTION("s5", "Successful deep search by id and index.")
    {
      browser.initialize(STR("root>1>0"));
      result = browser.getValue<IdentifiableObject>(data);
      REQUIRE(result != null);
    }
    SECTION("s6", "Unsuccessful deep search by id and index.")
    {
      browser.initialize(STR("root>1>3"));
      result = browser.getValue<IdentifiableObject>(data);
      REQUIRE(result == null);
    }
    SECTION("s7", "Successful deep search by id and index:id.")
    {
      browser.initialize(STR("root>1>0:child.1"));
      result = browser.getValue<IdentifiableObject>(data);
      REQUIRE(result != null);
    }
    SECTION("s8", "Unsuccessful deep search by id and index:id.")
    {
      browser.initialize(STR("root>1>0:child.2"));
      result = browser.getValue<IdentifiableObject>(data);
      REQUIRE(result == null);
    }
    SECTION("s9", "Unsuccessful extra deep search.")
    {
      browser.initialize(STR("root>parent.3>0"));
      result = browser.getValue<IdentifiableObject>(data);
      REQUIRE(result == null);
    }
  } catch (Exception &e) {
    FAIL(e.getErrorMessage());
  }
}

} } // namespace
