/**
 * @file Tests/CoreTests/Data_tests.cpp
 * Contain's the Core::Data namespace's unit tests.
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

// Test for a successful parsing of a simple assignment expression.
TEST_CASE("Core::Data/search", "Successfully search an element with its containing module.")
{
  GrammarRepository repository;

  try {
    repository.setSharedValue(STR("mod1"),
                     Module::create({
                                      {STR("var1"), 0},
                                      {STR("var2"), 0},
                                      {STR("mod2"), Module::create({
                                         {STR("var3"), 0},
                                         {STR("var4"), 0},
                                         {STR("map1"), SharedMap::create(true, {
                                            {STR("var5"), 0},
                                            {STR("var6"), std::make_shared<String>(STR("hello"))}
                                          })}
                                       })}
                                    }));

    SharedModulePairedPtr retVal;

    IdentifiableObject *plainStr;

    SECTION("s1", "Data retrieved using qualifier.")
    {
      repository.getSharedValue(STR("mod1.mod2.map1.var6"), retVal);
      REQUIRE(retVal.object.get() != 0);
      REQUIRE(retVal.object->isA<String>());
      REQUIRE(retVal.object.s_cast<String>()->getStr() == STR("hello"));
      REQUIRE(retVal.module.get() != 0);
      REQUIRE(retVal.module->getId() == ID_GENERATOR->getId(STR("mod1.mod2")));
    }
    SECTION("s2", "Data retrieved using Reference.")
    {
      repository.getSharedValue(ReferenceParser::parseQualifier(STR("mod1.mod2.map1.var6")).get(), retVal);
      REQUIRE(retVal.object.get() != 0);
      REQUIRE(retVal.object->isA<String>());
      REQUIRE(retVal.object.s_cast<String>()->getStr() == STR("hello"));
      REQUIRE(retVal.module.get() != 0);
      REQUIRE(retVal.module->getId() == ID_GENERATOR->getId(STR("mod1.mod2")));
    }
    SECTION("s3", "Plain data retrieved using qualifier.")
    {
      plainStr = repository.getPlainValue(STR("mod1.mod2.map1.var6"));
      REQUIRE(plainStr != 0);
      REQUIRE(plainStr->isA<String>());
      REQUIRE(static_cast<String*>(plainStr)->getStr() == STR("hello"));
    }
    SECTION("s4", "Data retrieved using Reference.")
    {
      plainStr = repository.getPlainValue(ReferenceParser::parseQualifier(STR("mod1.mod2.map1.var6")).get());
      REQUIRE(plainStr != 0);
      REQUIRE(plainStr->isA<String>());
      REQUIRE(static_cast<String*>(plainStr)->getStr() == STR("hello"));
    }
  } catch (Exception &e) {
    FAIL(e.getErrorMessage());
  }
}

} } // namespace
