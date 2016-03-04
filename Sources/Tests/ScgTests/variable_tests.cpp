/**
 * @file Tests/ScgTests/variable_tests.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

// Alusus header files
#include <core.h>
#include <scg.h>
#include <simple_test.h>

using namespace Scg;

namespace Tests
{
namespace ScgTests
{

bool TestDefinePrimitiveTypes()
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = Block::create({
    std::make_shared<DefineVariable>(CreateTypeSpecByName("int"), "testInt"),
    std::make_shared<DefineVariable>(CreateTypeSpecByName("float"), "testFloat"),
    std::make_shared<DefineVariable>(CreateTypeSpecByName("double"), "testDouble"),
    std::make_shared<DefineVariable>(CreateTypeSpecByName("string"), "testString"),
    std::make_shared<Return>(std::make_shared<IntegerConst>(0))
  });
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
                                               VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefinePrimitiveTypes");
  module->appendNode(main);
  auto program = new CodeGenUnit();
  program->addModule(module);
  std::cout << program->compile() << std::endl;
  std::cout << "TestDefinePrimitiveTypes succeeded." << std::endl;
  delete program;


  LlvmContainer::finalize();

  return true;
}

bool TestDefinePointersOfPrimitiveTypes()
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = Block::create({
    std::make_shared<DefineVariable>(CreatePointerToPrimitiveTypeSpec("int"), "testInt"),
    std::make_shared<DefineVariable>(CreatePointerToPrimitiveTypeSpec("float"), "testFloat"),
    std::make_shared<DefineVariable>(CreatePointerToPrimitiveTypeSpec("double"), "testDouble"),
    std::make_shared<DefineVariable>(CreatePointerToPrimitiveTypeSpec("string"), "testString"),
    std::make_shared<Return>(std::make_shared<IntegerConst>(0))
  });
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
                                               VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefinePointerOfPrimitiveTypes");
  module->appendNode(main);
  auto program = new CodeGenUnit();
  program->addModule(module);
  std::cout << program->compile() << std::endl;
  std::cout << "TestDefinePointerOfPrimitiveTypes succeeded." << std::endl;
  delete program;

  LlvmContainer::finalize();

  return true;
}

bool TestDefineArraysOfPrimitiveTypes()
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = Block::create({
    std::make_shared<DefineVariable>(CreateArrayOfPrimitiveTypeSpec("int", 10), "testInt"),
    std::make_shared<DefineVariable>(CreateArrayOfPrimitiveTypeSpec("float", 10), "testFloat"),
    std::make_shared<DefineVariable>(CreateArrayOfPrimitiveTypeSpec("double", 10), "testDouble"),
    std::make_shared<DefineVariable>(CreateArrayOfPrimitiveTypeSpec("string", 10), "testString"),
    std::make_shared<Return>(std::make_shared<IntegerConst>(0))
  });
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
                                               VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefineArraysOfPrimitiveTypes");
  module->appendNode(main);
  auto program = new CodeGenUnit();
  program->addModule(module);
  std::cout << program->compile() << std::endl;
  std::cout << "TestDefineArraysOfPrimitiveTypes succeeded." << std::endl;
  delete program;

  LlvmContainer::finalize();

  return true;
}

bool TestDefineStructure()
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = Block::create({
    DefineStruct::create("Point", {
      VariableDefinition(CreateTypeSpecByName("int"), "x"),
      VariableDefinition(CreateTypeSpecByName("int"), "y")
    }),
    std::make_shared<DefineVariable>(CreateTypeSpecByName("Point"), "point"),
    std::make_shared<Return>(std::make_shared<IntegerConst>(0))
  });
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
                                               VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefineStructure");
  module->appendNode(main);
  auto program = new CodeGenUnit();
  program->addModule(module);
  std::cout << program->compile() << std::endl;
  std::cout << "TestDefineStructure succeeded." << std::endl;
  delete program;

  LlvmContainer::finalize();

  return true;
}

bool TestDefineArrayOfStructure()
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = Block::create({
    DefineStruct::create("Point", {
      VariableDefinition(CreateTypeSpecByName("int"), "x"),
      VariableDefinition(CreateTypeSpecByName("int"), "y")
    }),
    std::make_shared<DefineVariable>(CreateArrayOfPrimitiveTypeSpec("Point", 10), "points"),
    std::make_shared<Return>(std::make_shared<IntegerConst>(0))
  });
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
                                               VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefineArrayOfStructure");
  module->appendNode(main);
  auto program = new CodeGenUnit();
  program->addModule(module);
  std::cout << program->compile() << std::endl;
  std::cout << "TestDefineArrayOfStructure succeeded." << std::endl;
  delete program;

  LlvmContainer::finalize();

  return true;
}

bool TestDefineLinkListNodeStructure()
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = Block::create({
    DefineStruct::create("LinkListNode", {
      VariableDefinition(CreateTypeSpecByName("int"), "data"),
      VariableDefinition(CreatePointerToPrimitiveTypeSpec("LinkListNode"), "prev"),
      VariableDefinition(CreatePointerToPrimitiveTypeSpec("LinkListNode"), "next"),
    }),
    std::make_shared<DefineVariable>(CreateTypeSpecByName("LinkListNode"), "node"),
    std::make_shared<Return>(std::make_shared<IntegerConst>(0))
  });
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
                                               VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefineLinkListNodeStructure");
  module->appendNode(main);
  auto program = new CodeGenUnit();
  program->addModule(module);
  std::cout << program->compile() << std::endl;
  std::cout << "TestDefineLinkListNodeStructure succeeded." << std::endl;
  delete program;

  LlvmContainer::finalize();

  return true;
}

bool TestDefinePointerToArray()
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = Block::create({
    std::make_shared<DefineVariable>(std::make_shared<PointerValueTypeSpec>(CreateArrayOfPrimitiveTypeSpec("int", 10)), "ints"),
    std::make_shared<DefineVariable>(std::make_shared<PointerValueTypeSpec>(CreateArrayOfPrimitiveTypeSpec("float", 10)), "floats"),
    std::make_shared<DefineVariable>(std::make_shared<PointerValueTypeSpec>(CreateArrayOfPrimitiveTypeSpec("double", 10)), "doubles"),
    std::make_shared<DefineVariable>(std::make_shared<PointerValueTypeSpec>(CreateArrayOfPrimitiveTypeSpec("string", 10)), "strings"),
    std::make_shared<Return>(std::make_shared<IntegerConst>(0))
  });
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
                                               VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefinePointerToArray");
  module->appendNode(main);
  auto program = new CodeGenUnit();
  program->addModule(module);
  std::cout << program->compile() << std::endl;
  std::cout << "TestDefinePointerToArray succeeded." << std::endl;
  delete program;

  LlvmContainer::finalize();

  return true;
}

bool TestDefineArrayOfPointers()
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = Block::create({
    std::make_shared<DefineVariable>(std::make_shared<ArrayValueTypeSpec>(CreatePointerToPrimitiveTypeSpec("int"), 10), "ints"),
    std::make_shared<DefineVariable>(std::make_shared<ArrayValueTypeSpec>(CreatePointerToPrimitiveTypeSpec("float"), 10), "floats"),
    std::make_shared<DefineVariable>(std::make_shared<ArrayValueTypeSpec>(CreatePointerToPrimitiveTypeSpec("double"), 10), "doubles"),
    std::make_shared<DefineVariable>(std::make_shared<ArrayValueTypeSpec>(CreatePointerToPrimitiveTypeSpec("string"), 10), "strings"),
    std::make_shared<Return>(std::make_shared<IntegerConst>(0))
  });
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
                                               VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefineArrayOfPointers");
  module->appendNode(main);
  auto program = new CodeGenUnit();
  program->addModule(module);
  std::cout << program->compile() << std::endl;
  std::cout << "TestDefineArrayOfPointers succeeded." << std::endl;
  delete program;

  LlvmContainer::finalize();

  return true;
}

bool RunAllVariableTests()
{
  auto ret = true;

  if (!TestDefinePrimitiveTypes()) ret = false;

  if (!TestDefinePointersOfPrimitiveTypes()) ret = false;

  if (!TestDefineArraysOfPrimitiveTypes()) ret = false;

  if (!TestDefineStructure()) ret = false;

  if (!TestDefineArrayOfStructure()) ret = false;

  if (!TestDefineLinkListNodeStructure()) ret = false;

  if (!TestDefinePointerToArray()) ret = false;

  if (!TestDefineArrayOfPointers()) ret = false;

  return ret;
}

}
} // namespace
