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
TEST_CASE("Core::Data/simple_seek", "Successfully seek an element with its containing module.")
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


TEST_CASE("Core::Data/advanced_qualifier_seek", "Seek elements with advanced qualifiers.")
{
  // Prepare a data tree.
  SharedPtr<IdentifiableObject> data =
    ParsedList::create(ID_GENERATOR->getId(STR("root")), {
                       ParsedRoute::create(ID_GENERATOR->getId(STR("parent.1")), 1,
                                           ParsedToken::create(UNKNOWN_ID, ID_GENERATOR->getId(STR("token1")),
                                                               STR("text"))),
                       ParsedList::create(ID_GENERATOR->getId(STR("parent.2")), {
                                          ParsedRoute::create(ID_GENERATOR->getId(STR("child.1")), 0),
                                          ParsedToken::create(ID_GENERATOR->getId(STR("child.2")),
                                                              ID_GENERATOR->getId(STR("token2")))}),
                       ParsedToken::create(ID_GENERATOR->getId(STR("parent.3")), ID_GENERATOR->getId(STR("token3")))});

  SharedPtr<IdentifiableObject> result;
  IdentifiableObject *plainResult = 0;
  IdentifiableObject *null = 0;
  QualifierSeeker seeker;

  try {
    SECTION("s1", "Successful one level search.")
    {
      plainResult = seeker.tryGetPlain(STR("self~where(prodId=root)"), data.get());
      REQUIRE(plainResult != null);
    }
    SECTION("s2", "Unseccessful one level search.")
    {
      plainResult = seeker.tryGetPlain(STR("self~where(prodId=noroot)"), data.get());
      REQUIRE(plainResult == null);
    }
    SECTION("s3", "Successful deep search by id only.")
    {
      result = seeker.tryGetShared(STR("self~where(prodId=root).{find prodId=parent.2, 0}.{find prodId=child.2, 0}"), data.get());
      REQUIRE(result != null);
    }
    SECTION("s4", "Unsuccessful deep search by id only.")
    {
      result = seeker.tryGetShared(STR("self~where(prodId=root).{find prodId=parent.2, 0}.{find prodId=child.3, 0}"), data.get());
      REQUIRE(result == null);
    }
    SECTION("s5", "Successful deep search by id and index.")
    {
      result = seeker.tryGetShared(STR("self~where(prodId=root).(1).(0)"), data.get());
      REQUIRE(result != null);
    }
    SECTION("s6", "Unsuccessful deep search by id and index.")
    {
      result = seeker.tryGetShared(STR("self~where(prodId=root).(1).(3)"), data.get());
      REQUIRE(result == null);
    }
    SECTION("s7", "Successful deep search by id and index:id.")
    {
      result = seeker.tryGetShared(STR("self~where(prodId=root).(1).0~where(prodId=child.1)"), data.get());
      REQUIRE(result != null);
    }
    SECTION("s8", "Unsuccessful deep search by id and index:id.")
    {
      result = seeker.tryGetShared(STR("self~where(prodId=root).(1).0~where(prodId=child.2)"), data.get());
      REQUIRE(result == null);
    }
    SECTION("s9", "Unsuccessful extra deep search.")
    {
      result = seeker.tryGetShared(STR("self~where(prodId=root).{find prodId=parent.3, 0}.(0)"), data.get());
      REQUIRE(result == null);
    }
    SECTION("s10", "Successful negative index.")
    {
      result = seeker.tryGetShared(STR("(-1)~where(prodId=parent.3)"), data.get());
      REQUIRE(result != null);
    }
  } catch (Exception &e) {
    FAIL(e.getErrorMessage());
  }
}


TEST_CASE("Core::Data/advanced_reference_seek", "Seek elements with advanced references.")
{
  // Prepare a data tree.
  SharedPtr<IdentifiableObject> data =
    ParsedList::create(ID_GENERATOR->getId(STR("root")), {
                       ParsedRoute::create(ID_GENERATOR->getId(STR("parent.1")), 1,
                                           ParsedToken::create(UNKNOWN_ID, ID_GENERATOR->getId(STR("token1")), STR("text"))),
                       ParsedList::create(ID_GENERATOR->getId(STR("parent.2")), {
                                          ParsedRoute::create(ID_GENERATOR->getId(STR("child.1")), 0),
                                          ParsedToken::create(ID_GENERATOR->getId(STR("child.2")),
                                                              ID_GENERATOR->getId(STR("token2")))}),
                       ParsedToken::create(ID_GENERATOR->getId(STR("parent.3")), ID_GENERATOR->getId(STR("token3")))});

  ReferenceSeeker seeker;
  SharedPtr<Reference> reference;
  SharedPtr<IdentifiableObject> result;
  IdentifiableObject *plainResult = 0;
  IdentifiableObject *null = 0;

  try {
    SECTION("s1", "Successful one level search.")
    {
      reference = ReferenceParser::parseQualifier(STR("self~where(prodId=root)"));
      plainResult = seeker.tryGetPlain(reference.get(), data.get());
      REQUIRE(plainResult != null);
    }
    SECTION("s2", "Unseccessful one level search.")
    {
      reference = ReferenceParser::parseQualifier(STR("self~where(prodId=noroot)"));
      plainResult = seeker.tryGetPlain(reference.get(), data.get());
      REQUIRE(plainResult == null);
    }
    SECTION("s3", "Successful deep search by id only.")
    {
      reference = ReferenceParser::parseQualifier(
        STR("self~where(prodId=root).{find prodId=parent.2, 0}.{find prodId=child.2, 0}"));
      result = seeker.tryGetShared(reference.get(), data.get());
      REQUIRE(result != null);
    }
    SECTION("s4", "Unsuccessful deep search by id only.")
    {
      reference = ReferenceParser::parseQualifier(
        STR("self~where(prodId=root).{find prodId=parent.2, 0}.{find prodId=child.3, 0}"));
      result = seeker.tryGetShared(reference.get(), data.get());
      REQUIRE(result == null);
    }
    SECTION("s5", "Successful deep search by id and index.")
    {
      reference = ReferenceParser::parseQualifier(
        STR("self~where(prodId=root).(1).(0)"));
      result = seeker.tryGetShared(reference.get(), data.get());
      REQUIRE(result != null);
    }
    SECTION("s6", "Unsuccessful deep search by id and index.")
    {
      reference = ReferenceParser::parseQualifier(
        STR("self~where(prodId=root).(1).(3)"));
      result = seeker.tryGetShared(reference.get(), data.get());
      REQUIRE(result == null);
    }
    SECTION("s7", "Successful deep search by id and index:id.")
    {
      reference = ReferenceParser::parseQualifier(
        STR("self~where(prodId=root).(1).(0)~where(prodId=child.1)"));
      result = seeker.tryGetShared(reference.get(), data.get());
      REQUIRE(result != null);
    }
    SECTION("s8", "Unsuccessful deep search by id and index:id.")
    {
      reference = ReferenceParser::parseQualifier(
        STR("self~where(prodId=root).(1).(0)~where(prodId=child.2)"));
      result = seeker.tryGetShared(reference.get(), data.get());
      REQUIRE(result == null);
    }
    SECTION("s9", "Unsuccessful extra deep search.")
    {
      reference = ReferenceParser::parseQualifier(
        STR("self~where(prodId=root).{find prodId=parent.3, 0}.(0)"));
      result = seeker.tryGetShared(reference.get(), data.get());
      REQUIRE(result == null);
    }
    SECTION("s10", "Successful negative index.")
    {
      reference = ReferenceParser::parseQualifier(
        STR("(-1)~where(prodId=parent.3)"));
      result = seeker.tryGetShared(reference.get(), data.get());
      REQUIRE(result != null);
    }
  } catch (Exception &e) {
    FAIL(e.getErrorMessage());
  }
}

} } // namespace
