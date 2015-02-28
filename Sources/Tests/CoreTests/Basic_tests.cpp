/**
 * @file Tests/CoreTests/Data_tests.cpp
 * Contain's the Core::Basic namespace's unit tests.
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

TEST_CASE("Core::Basic/getSharedPtr", "Test of getSharedPtr function.")
{
  try {
    SECTION("s1", "getSharedPtr shares ownership with existing pointers.")
    {
      SharedPtr<Data::SharedList> list(new SharedList());
      SharedPtr<Data::SharedList> pl2 = getSharedPtr(list.get());
      REQUIRE(pl2.get() != 0);
      REQUIRE(pl2.get() == list.get());
      REQUIRE(list.use_count() == 2);
      REQUIRE(pl2.use_count() == 2);
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}


TEST_CASE("Core::Basic/shared_pointer_casting", "Test casting of shared pointers.")
{
  // Prepare a data tree.
  SharedPtr<IdentifiableObject> data =
    ParsedList::create(ID_GENERATOR->getId(STR("root")), {
                       ParsedToken::create(ID_GENERATOR->getId(STR("parent.3")), ID_GENERATOR->getId(STR("token3")))});
  QualifierSeeker seeker;

  try {
    SECTION("s1", "Static Cast")
    {
      SharedPtr<ParsedToken> testToken1 = s_cast<ParsedToken>(getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get())));
      SharedPtr<IdentifiableObject> testTok = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get()));
      SharedPtr<ParsedToken> testToken2 = s_cast<ParsedToken>(testTok);
      REQUIRE(testToken1.get() != 0);
      REQUIRE(testToken2.get() == testToken1.get());
      REQUIRE(testToken1.use_count() > 1);
      REQUIRE(testToken2.use_count() > 1);
    }
    SECTION("s2", "Reinterpret Cast")
    {
      SharedPtr<ParsedToken> testToken1 = r_cast<ParsedToken>(getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get())));
      SharedPtr<IdentifiableObject> testTok = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get()));
      SharedPtr<ParsedToken> testToken2 = r_cast<ParsedToken>(testTok);
      REQUIRE(testToken1.get() != 0);
      REQUIRE(testToken2.get() == testToken1.get());
      REQUIRE(testToken1.use_count() > 1);
      REQUIRE(testToken2.use_count() > 1);
    }
    SECTION("s3", "IdentifiableObject Cast")
    {
      SharedPtr<ParsedToken> testToken1 = io_cast<ParsedToken>(getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get())));
      SharedPtr<IdentifiableObject> testTok = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get()));
      SharedPtr<ParsedToken> testToken2 = io_cast<ParsedToken>(testTok);
      REQUIRE(testToken1.get() != 0);
      REQUIRE(testToken2.get() == testToken1.get());
      REQUIRE(testToken1.use_count() > 1);
      REQUIRE(testToken2.use_count() > 1);
    }
    SECTION("s4", "Member Static Cast")
    {
      SharedPtr<ParsedToken> testToken1 = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get())).s_cast<ParsedToken>();
      SharedPtr<IdentifiableObject> testTok = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get()));
      SharedPtr<ParsedToken> testToken2 = testTok.s_cast<ParsedToken>();
      REQUIRE(testToken1.get() != 0);
      REQUIRE(testToken2.get() == testToken1.get());
      REQUIRE(testToken1.use_count() > 1);
      REQUIRE(testToken2.use_count() > 1);
    }
    SECTION("s5", "Member Reinterpret Cast")
    {
      SharedPtr<ParsedToken> testToken1 = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get())).r_cast<ParsedToken>();
      SharedPtr<IdentifiableObject> testTok = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get()));
      SharedPtr<ParsedToken> testToken2 = testTok.r_cast<ParsedToken>();
      REQUIRE(testToken1.get() != 0);
      REQUIRE(testToken2.get() == testToken1.get());
      REQUIRE(testToken1.use_count() > 1);
      REQUIRE(testToken2.use_count() > 1);
    }
    SECTION("s6", "Member IdentifiableObject Cast")
    {
      SharedPtr<ParsedToken> testToken1 = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get())).io_cast<ParsedToken>();
      SharedPtr<IdentifiableObject> testTok = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get()));
      SharedPtr<ParsedToken> testToken2 = testTok.io_cast<ParsedToken>();
      REQUIRE(testToken1.get() != 0);
      REQUIRE(testToken2.get() == testToken1.get());
      REQUIRE(testToken1.use_count() > 1);
      REQUIRE(testToken2.use_count() > 1);
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}

} } // namespace
