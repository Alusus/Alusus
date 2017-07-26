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
      SharedPtr<Data::SharedList> list(new Data::SharedList());
      SharedPtr<Data::SharedList> pl2 = getSharedPtr(list.get());
      CHECK(pl2.get() != 0);
      CHECK(pl2.get() == list.get());
      CHECK(list.use_count() == 2);
      CHECK(pl2.use_count() == 2);
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}


TEST_CASE("Core::Basic/shared_pointer_casting", "Test casting of shared pointers.")
{
  // Prepare a data tree.
  SharedPtr<TiObject> data =
    Ast::List::create({
      { "prodId", TiWord(ID_GENERATOR->getId(STR("root"))) }
    }, {
      Ast::Token::create({
        { "prodId", TiWord(ID_GENERATOR->getId(STR("parent.3"))) },
        { "id", TiWord(ID_GENERATOR->getId(STR("token3"))) }
      })
    });
  QualifierSeeker seeker;

  try {
    SECTION("s1", "Static Cast")
    {
      SharedPtr<Ast::Token> testToken1 = s_cast<Ast::Token>(getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get())));
      SharedPtr<TiObject> testTok = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get()));
      SharedPtr<Ast::Token> testToken2 = s_cast<Ast::Token>(testTok);
      CHECK(testToken1.get() != 0);
      CHECK(testToken2.get() == testToken1.get());
      CHECK(testToken1.use_count() > 1);
      CHECK(testToken2.use_count() > 1);
    }
    SECTION("s2", "Reinterpret Cast")
    {
      SharedPtr<Ast::Token> testToken1 = r_cast<Ast::Token>(getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get())));
      SharedPtr<TiObject> testTok = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get()));
      SharedPtr<Ast::Token> testToken2 = r_cast<Ast::Token>(testTok);
      CHECK(testToken1.get() != 0);
      CHECK(testToken2.get() == testToken1.get());
      CHECK(testToken1.use_count() > 1);
      CHECK(testToken2.use_count() > 1);
    }
    SECTION("s3", "TiObject Cast")
    {
      SharedPtr<Ast::Token> testToken1 = tio_cast<Ast::Token>(getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get())));
      SharedPtr<TiObject> testTok = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get()));
      SharedPtr<Ast::Token> testToken2 = tio_cast<Ast::Token>(testTok);
      CHECK(testToken1.get() != 0);
      CHECK(testToken2.get() == testToken1.get());
      CHECK(testToken1.use_count() > 1);
      CHECK(testToken2.use_count() > 1);
    }
    SECTION("s4", "Member Static Cast")
    {
      SharedPtr<Ast::Token> testToken1 = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get())).s_cast<Ast::Token>();
      SharedPtr<TiObject> testTok = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get()));
      SharedPtr<Ast::Token> testToken2 = testTok.s_cast<Ast::Token>();
      CHECK(testToken1.get() != 0);
      CHECK(testToken2.get() == testToken1.get());
      CHECK(testToken1.use_count() > 1);
      CHECK(testToken2.use_count() > 1);
    }
    SECTION("s5", "Member Reinterpret Cast")
    {
      SharedPtr<Ast::Token> testToken1 = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get())).r_cast<Ast::Token>();
      SharedPtr<TiObject> testTok = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get()));
      SharedPtr<Ast::Token> testToken2 = testTok.r_cast<Ast::Token>();
      CHECK(testToken1.get() != 0);
      CHECK(testToken2.get() == testToken1.get());
      CHECK(testToken1.use_count() > 1);
      CHECK(testToken2.use_count() > 1);
    }
    SECTION("s6", "Member TiObject Cast")
    {
      SharedPtr<Ast::Token> testToken1 = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get())).tio_cast<Ast::Token>();
      SharedPtr<TiObject> testTok = getSharedPtr(seeker.tryGet(STR("{find prodId=parent.3}"), data.get()));
      SharedPtr<Ast::Token> testToken2 = testTok.tio_cast<Ast::Token>();
      CHECK(testToken1.get() != 0);
      CHECK(testToken2.get() == testToken1.get());
      CHECK(testToken1.use_count() > 1);
      CHECK(testToken2.use_count() > 1);
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}

} } // namespace
