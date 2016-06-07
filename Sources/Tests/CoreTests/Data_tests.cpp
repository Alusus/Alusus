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

TEST_CASE("Core::Data/qualifier_validation", "Successfully validate reference qualifiers.")
{
  try {
    SECTION("s1", "Valid absolute qualifier.")
    {
      CHECK(REF_PARSER->validateQualifier(STR("aaa.bbb.ccc")) == true);
      CHECK(REF_PARSER->validateQualifier(STR("123.bbb.ccc")) == true);
      CHECK(REF_PARSER->validateQualifier(STR("aaa.123.ccc")) == true);
      CHECK(REF_PARSER->validateQualifier(STR("aaa.bbb.123")) == true);
    }
    SECTION("s2", "Valid indirect qualifier.")
    {
      CHECK(REF_PARSER->validateQualifier(STR("aaa.{eval bbb.ccc}")) == true);
      CHECK(REF_PARSER->validateQualifier(STR("123.{eval bbb}.ccc")) == true);
      CHECK(REF_PARSER->validateQualifier(STR("{eval aaa}.123.ccc")) == true);
    }
    SECTION("s3", "Invalid qualifiers.")
    {
      CHECK(REF_PARSER->validateQualifier(STR("aaa.bbb.ccc}")) == false);
      CHECK(REF_PARSER->validateQualifier(STR("123.{eval bbb.ccc")) == false);
      CHECK(REF_PARSER->validateQualifier(STR("aaa.123.")) == false);
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}


TEST_CASE("Core::Data/qualifier_finding_last_seg", "Successfully find last segment within qualifiers.")
{
  try {
    SECTION("s1", "Find last seg of absolute qualifier.")
    {
      Char const *seg = REF_PARSER->findLastQualifierSegment(STR("aaa.bbb.ccc"));
      CHECK(compareStr(seg, STR("ccc")) == 0);
    }
    SECTION("s2", "Find last seg of indirect qualifier.")
    {
      Char const *seg = REF_PARSER->findLastQualifierSegment(STR("aaa.{eval bbb}.ccc"));
      CHECK(compareStr(seg, STR("ccc")) == 0);
    }
    SECTION("s3", "Find last seg which itself is indirect.")
    {
      Char const *seg = REF_PARSER->findLastQualifierSegment(STR("aaa.bbb.{eval ccc.ddd}"));
      CHECK(compareStr(seg, STR("{eval ccc.ddd}")) == 0);
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}


TEST_CASE("Core::Data/simple_seek", "Successfully seek an element with its containing module.")
{
  GrammarRepository repository;

  try {
    repository.set(STR("mod1"),
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
                                    }).get());

    PlainPairedPtr retVal;

    TiObject *plainStr;

    SECTION("s1", "Data retrieved using qualifier.")
    {
      repository.get(STR("mod1.mod2.map1.var6"), retVal.object, Module::getTypeInfo(), &retVal.parent);
      CHECK(retVal.object != 0);
      CHECK(retVal.object->isA<String>());
      CHECK(static_cast<String*>(retVal.object)->getStr() == STR("hello"));
      CHECK(retVal.parent != 0);
      CHECK(static_cast<Module*>(retVal.parent)->getId() == ID_GENERATOR->getId(STR("mod1.mod2")));
    }
    SECTION("s2", "Data retrieved using Reference.")
    {
      repository.get(REF_PARSER->parseQualifier(STR("mod1.mod2.map1.var6")).get(), retVal.object,
                     Module::getTypeInfo(), &retVal.parent);
      CHECK(retVal.object != 0);
      CHECK(retVal.object->isA<String>());
      CHECK(static_cast<String*>(retVal.object)->getStr() == STR("hello"));
      CHECK(retVal.parent != 0);
      CHECK(static_cast<Module*>(retVal.parent)->getId() == ID_GENERATOR->getId(STR("mod1.mod2")));
    }
    SECTION("s3", "Plain data retrieved using qualifier.")
    {
      plainStr = repository.get(STR("mod1.mod2.map1.var6"));
      CHECK(plainStr != 0);
      CHECK(plainStr->isA<String>());
      CHECK(static_cast<String*>(plainStr)->getStr() == STR("hello"));
    }
    SECTION("s4", "Data retrieved using Reference.")
    {
      plainStr = repository.get(REF_PARSER->parseQualifier(STR("mod1.mod2.map1.var6")).get());
      CHECK(plainStr != 0);
      CHECK(plainStr->isA<String>());
      CHECK(static_cast<String*>(plainStr)->getStr() == STR("hello"));
    }
    SECTION("s5", "Data retrieved using qualifier.")
    {
      QualifierSeeker seeker;
      TiObject *result, *parent;
      seeker.tryGet(STR("mod1.mod2.map1.var6"), repository.getRoot().get(), result, Module::getTypeInfo(), &parent);
      CHECK(result != 0);
      CHECK(parent != 0);
      TiObject *parent2;
      seeker.tryGet(STR("mod1.mod2.map1.var6"), repository.getRoot().get(), result, SharedMap::getTypeInfo(), &parent2);
      CHECK(result != 0);
      CHECK(parent != 0);
      CHECK(parent != parent2);
    }
    SECTION("s6", "Data retrieved using reference.")
    {
      ReferenceSeeker seeker;
      TiObject *result, *parent;
      seeker.tryGet(REF_PARSER->parseQualifier(STR("mod1.mod2.map1.var6")).get(),
                    repository.getRoot().get(), result, Module::getTypeInfo(), &parent);
      CHECK(result != 0);
      CHECK(parent != 0);
      TiObject *parent2;
      seeker.tryGet(REF_PARSER->parseQualifier(STR("mod1.mod2.map1.var6")).get(),
                    repository.getRoot().get(), result, SharedMap::getTypeInfo(), &parent2);
      CHECK(result != 0);
      CHECK(parent != 0);
      CHECK(parent != parent2);
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}


TEST_CASE("Core::Data/advanced_qualifier_seek", "Seek elements with advanced qualifiers.")
{
  // Prepare a data tree.
  SharedPtr<TiObject> data =
    Ast::List::create(ID_GENERATOR->getId(STR("root")), {
                       Ast::Route::create(ID_GENERATOR->getId(STR("parent.1")), 1,
                                           Ast::Token::create(UNKNOWN_ID, ID_GENERATOR->getId(STR("token1")),
                                                               STR("text"))),
                       Ast::List::create(ID_GENERATOR->getId(STR("parent.2")), {
                                          Ast::Route::create(ID_GENERATOR->getId(STR("child.1")), 0),
                                          Ast::Token::create(ID_GENERATOR->getId(STR("child.2")),
                                                              ID_GENERATOR->getId(STR("token2")))}),
                       Ast::Token::create(ID_GENERATOR->getId(STR("parent.3")), ID_GENERATOR->getId(STR("token3")))});

  TiObject *result = 0;
  TiObject *plainResult = 0;
  TiObject *null = 0;
  QualifierSeeker seeker;

  try {
    SECTION("s1", "Successful one level search.")
    {
      plainResult = seeker.tryGet(STR("self~where(prodId=root)"), data.get());
      CHECK(plainResult != null);
    }
    SECTION("s2", "Unseccessful one level search.")
    {
      plainResult = seeker.tryGet(STR("self~where(prodId=noroot)"), data.get());
      CHECK(plainResult == null);
    }
    SECTION("s3", "Successful deep search by id only.")
    {
      result = seeker.tryGet(STR("self~where(prodId=root).{find prodId=parent.2, 0}.{find prodId=child.2, 0}"), data.get());
      CHECK(result != null);
    }
    SECTION("s4", "Unsuccessful deep search by id only.")
    {
      result = seeker.tryGet(STR("self~where(prodId=root).{find prodId=parent.2, 0}.{find prodId=child.3, 0}"), data.get());
      CHECK(result == null);
    }
    SECTION("s5", "Successful deep search by id and index.")
    {
      result = seeker.tryGet(STR("self~where(prodId=root).(1).(0)"), data.get());
      CHECK(result != null);
    }
    SECTION("s6", "Unsuccessful deep search by id and index.")
    {
      result = seeker.tryGet(STR("self~where(prodId=root).(1).(3)"), data.get());
      CHECK(result == null);
    }
    SECTION("s7", "Successful deep search by id and index:id.")
    {
      result = seeker.tryGet(STR("self~where(prodId=root).(1).0~where(prodId=child.1)"), data.get());
      CHECK(result != null);
    }
    SECTION("s8", "Unsuccessful deep search by id and index:id.")
    {
      result = seeker.tryGet(STR("self~where(prodId=root).(1).0~where(prodId=child.2)"), data.get());
      CHECK(result == null);
    }
    SECTION("s9", "Unsuccessful extra deep search.")
    {
      result = seeker.tryGet(STR("self~where(prodId=root).{find prodId=parent.3, 0}.(0)"), data.get());
      CHECK(result == null);
    }
    SECTION("s10", "Successful negative index.")
    {
      result = seeker.tryGet(STR("(-1)~where(prodId=parent.3)"), data.get());
      CHECK(result != null);
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}


TEST_CASE("Core::Data/advanced_reference_seek", "Seek elements with advanced references.")
{
  // Prepare a data tree.
  SharedPtr<TiObject> data =
    Ast::List::create(ID_GENERATOR->getId(STR("root")), {
                       Ast::Route::create(ID_GENERATOR->getId(STR("parent.1")), 1,
                                           Ast::Token::create(UNKNOWN_ID, ID_GENERATOR->getId(STR("token1")), STR("text"))),
                       Ast::List::create(ID_GENERATOR->getId(STR("parent.2")), {
                                          Ast::Route::create(ID_GENERATOR->getId(STR("child.1")), 0),
                                          Ast::Token::create(ID_GENERATOR->getId(STR("child.2")),
                                                              ID_GENERATOR->getId(STR("token2")))}),
                       Ast::Token::create(ID_GENERATOR->getId(STR("parent.3")), ID_GENERATOR->getId(STR("token3")))});

  ReferenceSeeker seeker;
  SharedPtr<Reference> reference;
  TiObject *result = 0;
  TiObject *plainResult = 0;
  TiObject *null = 0;

  try {
    SECTION("s1", "Successful one level search.")
    {
      reference = REF_PARSER->parseQualifier(STR("self~where(prodId=root)"));
      plainResult = seeker.tryGet(reference.get(), data.get());
      CHECK(plainResult != null);
    }
    SECTION("s2", "Unseccessful one level search.")
    {
      reference = REF_PARSER->parseQualifier(STR("self~where(prodId=noroot)"));
      plainResult = seeker.tryGet(reference.get(), data.get());
      CHECK(plainResult == null);
    }
    SECTION("s3", "Successful deep search by id only.")
    {
      reference = REF_PARSER->parseQualifier(
        STR("self~where(prodId=root).{find prodId=parent.2, 0}.{find prodId=child.2, 0}"));
      result = seeker.tryGet(reference.get(), data.get());
      CHECK(result != null);
    }
    SECTION("s4", "Unsuccessful deep search by id only.")
    {
      reference = REF_PARSER->parseQualifier(
        STR("self~where(prodId=root).{find prodId=parent.2, 0}.{find prodId=child.3, 0}"));
      result = seeker.tryGet(reference.get(), data.get());
      CHECK(result == null);
    }
    SECTION("s5", "Successful deep search by id and index.")
    {
      reference = REF_PARSER->parseQualifier(
        STR("self~where(prodId=root).(1).(0)"));
      result = seeker.tryGet(reference.get(), data.get());
      CHECK(result != null);
    }
    SECTION("s6", "Unsuccessful deep search by id and index.")
    {
      reference = REF_PARSER->parseQualifier(
        STR("self~where(prodId=root).(1).(3)"));
      result = seeker.tryGet(reference.get(), data.get());
      CHECK(result == null);
    }
    SECTION("s7", "Successful deep search by id and index:id.")
    {
      reference = REF_PARSER->parseQualifier(
        STR("self~where(prodId=root).(1).(0)~where(prodId=child.1)"));
      result = seeker.tryGet(reference.get(), data.get());
      CHECK(result != null);
    }
    SECTION("s8", "Unsuccessful deep search by id and index:id.")
    {
      reference = REF_PARSER->parseQualifier(
        STR("self~where(prodId=root).(1).(0)~where(prodId=child.2)"));
      result = seeker.tryGet(reference.get(), data.get());
      CHECK(result == null);
    }
    SECTION("s9", "Unsuccessful extra deep search.")
    {
      reference = REF_PARSER->parseQualifier(
        STR("self~where(prodId=root).{find prodId=parent.3, 0}.(0)"));
      result = seeker.tryGet(reference.get(), data.get());
      CHECK(result == null);
    }
    SECTION("s10", "Successful negative index.")
    {
      reference = REF_PARSER->parseQualifier(
        STR("(-1)~where(prodId=parent.3)"));
      result = seeker.tryGet(reference.get(), data.get());
      CHECK(result != null);
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}


TEST_CASE("Core::Data/node_ownership", "node owner is set and reset correctly.")
{
  try {
    SharedPtr<Node> node1 = std::make_shared<Node>();
    SharedPtr<Node> node2 = std::make_shared<Node>();
    SharedPtr<Node> node3 = std::make_shared<Node>();

    SECTION("s1", "New Nodes have null owners.")
    {
      CHECK(node1->getOwner() == 0);
      CHECK(node2->getOwner() == 0);
      CHECK(node3->getOwner() == 0);
    }
    SECTION("s2", "SharedList owns and disowns objects.")
    {
      SharedPtr<SharedList> list = SharedList::create({node1, node2});
      CHECK(node1->getOwner() == list.get());
      CHECK(node2->getOwner() == list.get());
      list->add(node3);
      CHECK(node3->getOwner() == list.get());
      list->remove(1);
      CHECK(node1->getOwner() == list.get());
      CHECK(node2->getOwner() == 0);
      CHECK(node3->getOwner() == list.get());
      list->remove(0);
      CHECK(node1->getOwner() == 0);
      CHECK(node2->getOwner() == 0);
      CHECK(node3->getOwner() == list.get());
      list.reset();
      CHECK(node3->getOwner() == 0);
    }
    SECTION("s3", "SharedMap owns and disowns objects.")
    {
      SharedPtr<SharedMap> map = SharedMap::create(false, {
         {STR("node1"), node1},
         {STR("node2"), node2}
       });
      CHECK(node1->getOwner() == map.get());
      CHECK(node2->getOwner() == map.get());
      map->add(STR("node3"), node3);
      CHECK(node3->getOwner() == map.get());
      map->remove(STR("node2"));
      CHECK(node1->getOwner() == map.get());
      CHECK(node2->getOwner() == 0);
      CHECK(node3->getOwner() == map.get());
      map->remove(0);
      CHECK(node1->getOwner() == 0);
      CHECK(node2->getOwner() == 0);
      CHECK(node3->getOwner() == map.get());
      map.reset();
      CHECK(node3->getOwner() == 0);
    }
    SECTION("s4", "Module owns and disowns objects.")
    {
      SharedPtr<Module> map = Module::create({
         {STR("node1"), node1},
         {STR("node2"), node2}
       });
      CHECK(node1->getOwner() == map.get());
      CHECK(node2->getOwner() == map.get());
      map->add(STR("node3"), node3);
      CHECK(node3->getOwner() == map.get());
      map->remove(STR("node2"));
      CHECK(node1->getOwner() == map.get());
      CHECK(node2->getOwner() == 0);
      CHECK(node3->getOwner() == map.get());
      map->remove(0);
      CHECK(node1->getOwner() == 0);
      CHECK(node2->getOwner() == 0);
      CHECK(node3->getOwner() == map.get());
      map.reset();
      CHECK(node3->getOwner() == 0);
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}


TEST_CASE("Core::Data/id_generation", "ID is generated correctly in data trees.")
{
  try {
    SharedRepository repository(VARIABLE_NAME_MAX_LENGTH, RESERVED_VARIABLE_COUNT);
    repository.setOwningEnabled(true);
    repository.pushLevel(STR("root"), Module::create());
    repository.set(STR("mod1"),
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
                                    }).get());
    SharedPtr<Module> mod = Module::create();
    SharedPtr<Module> childMod = Module::create();
    mod->add(STR("childMod"), childMod);

    SECTION("s1", "setTreeIds on a root generates correct ids.")
    {
      setTreeIds(mod.get());
      CHECK(mod->getId() == ID_GENERATOR->getId(STR("")));
      CHECK(ID_GENERATOR->getDesc(childMod->getId()) == STR("childMod"));
    }
    SECTION("s2", "setTreeIds on a child generates correct ids")
    {
      SharedPtr<Module> mod2 = Module::create();
      SharedPtr<Module> childMod2 = Module::create();
      mod2->add(STR("childMod2"), childMod2);
      childMod->add(STR("myMod"), mod2);
      setTreeIds(mod2.get());
      CHECK(mod2->getId() == ID_GENERATOR->getId(STR("childMod.myMod")));
      CHECK(childMod2->getId() == ID_GENERATOR->getId(STR("childMod.myMod.childMod2")));
    }
    SECTION("s3", "SharedRepository sets the IDs correctly.")
    {
      repository.set(STR("mod1.mod2.map1.mod3"), mod.get());
      CHECK(mod->getId() == ID_GENERATOR->getId(STR("mod1.mod2.map1.mod3")));
      CHECK(childMod->getId() == ID_GENERATOR->getId(STR("mod1.mod2.map1.mod3.childMod")));
    }
    SECTION("s4", "PlainRepository sets the IDs correctly.")
    {
      PlainRepository plainRepo(VARIABLE_NAME_MAX_LENGTH, RESERVED_VARIABLE_COUNT);
      plainRepo.setOwningEnabled(true);
      SharedPtr<Module> plainRoot = Module::create();
      plainRepo.pushLevel(STR("root"), plainRoot.get());
      SharedPtr<Module> plainMod = Module::create();
      plainRepo.set(STR("plainMod"), plainMod.get());

      plainRepo.set(STR("plainMod.mod"), mod.get());
      CHECK(mod->getId() == ID_GENERATOR->getId(STR("plainMod.mod")));
      CHECK(childMod->getId() == ID_GENERATOR->getId(STR("plainMod.mod.childMod")));

      plainRepo.clear();
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}


TEST_CASE("Core::Data/initializables", "Initializable objects are initialized correctly.")
{
  try {
    GrammarRepository repository;
    repository.set(STR("mod1"),
                     Module::create({
                                      {STR("var1"), 0},
                                      {STR("var2"), 0},
                                      {STR("mod2"), GrammarModule::create({
                                         {STR("var3"), 0},
                                         {STR("var4"), 0},
                                         {STR("map1"), SharedMap::create(true, {
                                            {STR("var5"), 0},
                                            {STR("var6"), std::make_shared<String>(STR("hello"))}
                                          })}
                                       })}
                                    }).get());
    SharedPtr<GrammarModule> childMod = GrammarModule::create({
        { STR("@parent"), REF_PARSER->parseQualifier(STR("root:mod1.mod2")) }
      });
    SECTION("s1", "GrammarRepository initializes Initializable objects.")
    {
      CHECK(childMod->getParent() == 0);
      repository.set(STR("mod1.mod3"), childMod.get());
      CHECK(childMod->getParent() != 0);
      TiObject *parent = childMod->getParent();
      IdHolder *idHolder = parent->getInterface<IdHolder>();
      CHECK(idHolder != 0);
      CHECK(idHolder->getId() == ID_GENERATOR->getId(STR("mod1.mod2")));
    }
  } catch (Exception &e) {
    FAIL(e.getVerboseErrorMessage());
  }
}

} } // namespace
