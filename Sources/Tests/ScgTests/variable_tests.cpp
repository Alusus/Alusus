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
#include <scg.h>
#include <simple_test.h>

using namespace Scg;

namespace Tests { namespace ScgTests
{

bool TestDefinePrimitiveTypes()
{
  LlvmContainer::Initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = new Block({
    new DefineVariable(CreateTypeSpecByName("int"), "testInt"),
    new DefineVariable(CreateTypeSpecByName("float"), "testFloat"),
    new DefineVariable(CreateTypeSpecByName("double"), "testDouble"),
    new DefineVariable(CreateTypeSpecByName("string"), "testString"),
    new Return(new IntegerConst(0))
  });
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
      VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefinePrimitiveTypes");
  module->AppendExpression(main);
  auto program = new Program();
  program->AddModule(module);
  std::cout << program->Compile() << std::endl;
  std::cout << "TestDefinePrimitiveTypes succeeded." << std::endl;
  delete program;


  LlvmContainer::Finalize();

  return true;
}

bool TestDefinePointersOfPrimitiveTypes()
{
  LlvmContainer::Initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = new Block({
    new DefineVariable(CreatePointerToPrimitiveTypeSpec("int"), "testInt"),
    new DefineVariable(CreatePointerToPrimitiveTypeSpec("float"), "testFloat"),
    new DefineVariable(CreatePointerToPrimitiveTypeSpec("double"), "testDouble"),
    new DefineVariable(CreatePointerToPrimitiveTypeSpec("string"), "testString"),
    new Return(new IntegerConst(0))
  });
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
      VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefinePointerOfPrimitiveTypes");
  module->AppendExpression(main);
  auto program = new Program();
  program->AddModule(module);
  std::cout << program->Compile() << std::endl;
  std::cout << "TestDefinePointerOfPrimitiveTypes succeeded." << std::endl;
  delete program;

  LlvmContainer::Finalize();

  return true;
}

bool TestDefineArraysOfPrimitiveTypes()
{
  LlvmContainer::Initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = new Block({
    new DefineVariable(CreateArrayOfPrimitiveTypeSpec("int", 10), "testInt"),
    new DefineVariable(CreateArrayOfPrimitiveTypeSpec("float", 10), "testFloat"),
    new DefineVariable(CreateArrayOfPrimitiveTypeSpec("double", 10), "testDouble"),
    new DefineVariable(CreateArrayOfPrimitiveTypeSpec("string", 10), "testString"),
    new Return(new IntegerConst(0))
  });
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
      VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefineArraysOfPrimitiveTypes");
  module->AppendExpression(main);
  auto program = new Program();
  program->AddModule(module);
  std::cout << program->Compile() << std::endl;
  std::cout << "TestDefineArraysOfPrimitiveTypes succeeded." << std::endl;
  delete program;

  LlvmContainer::Finalize();

  return true;
}

bool TestDefineStructure()
{
  LlvmContainer::Initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = new Block({
      new DefineStruct("Point", {
          VariableDefinition(CreateTypeSpecByName("int"), "x"),
          VariableDefinition(CreateTypeSpecByName("int"), "y")
      }),
      new DefineVariable(CreateTypeSpecByName("Point"), "point"),
      new Return(new IntegerConst(0))
  });
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
      VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefineStructure");
  module->AppendExpression(main);
  auto program = new Program();
  program->AddModule(module);
  std::cout << program->Compile() << std::endl;
  std::cout << "TestDefineStructure succeeded." << std::endl;
  delete program;

  LlvmContainer::Finalize();

  return true;
}

bool TestDefineArrayOfStructure()
{
  LlvmContainer::Initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = new Block({
      new DefineStruct("Point", {
          VariableDefinition(CreateTypeSpecByName("int"), "x"),
          VariableDefinition(CreateTypeSpecByName("int"), "y")
      }),
      new DefineVariable(CreateArrayOfPrimitiveTypeSpec("Point", 10), "points"),
      new Return(new IntegerConst(0))
  });
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
      VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefineArrayOfStructure");
  module->AppendExpression(main);
  auto program = new Program();
  program->AddModule(module);
  std::cout << program->Compile() << std::endl;
  std::cout << "TestDefineArrayOfStructure succeeded." << std::endl;
  delete program;

  LlvmContainer::Finalize();

  return true;
}

bool TestDefineLinkListNodeStructure()
{
  LlvmContainer::Initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = new Block({
      new DefineStruct("LinkListNode", {
          VariableDefinition(CreateTypeSpecByName("int"), "data"),
          VariableDefinition(CreatePointerToPrimitiveTypeSpec("LinkListNode"), "prev"),
          VariableDefinition(CreatePointerToPrimitiveTypeSpec("LinkListNode"), "next"),
      }),
      new DefineVariable(CreateTypeSpecByName("LinkListNode"), "node"),
      new Return(new IntegerConst(0))
  });
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
      VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefineLinkListNodeStructure");
  module->AppendExpression(main);
  auto program = new Program();
  program->AddModule(module);
  std::cout << program->Compile() << std::endl;
  std::cout << "TestDefineLinkListNodeStructure succeeded." << std::endl;
  delete program;

  LlvmContainer::Finalize();

  return true;
}

bool TestDefinePointerToArray()
{
  LlvmContainer::Initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = new Block({
      new DefineVariable(new PointerValueTypeSpec(CreateArrayOfPrimitiveTypeSpec("int", 10)), "ints"),
      new DefineVariable(new PointerValueTypeSpec(CreateArrayOfPrimitiveTypeSpec("float", 10)), "floats"),
      new DefineVariable(new PointerValueTypeSpec(CreateArrayOfPrimitiveTypeSpec("double", 10)), "doubles"),
      new DefineVariable(new PointerValueTypeSpec(CreateArrayOfPrimitiveTypeSpec("string", 10)), "strings"),
      new Return(new IntegerConst(0))
  });
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
      VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefinePointerToArray");
  module->AppendExpression(main);
  auto program = new Program();
  program->AddModule(module);
  std::cout << program->Compile() << std::endl;
  std::cout << "TestDefinePointerToArray succeeded." << std::endl;
  delete program;

  LlvmContainer::Finalize();

  return true;
}

bool TestDefineArrayOfPointers()
{
  LlvmContainer::Initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = new Block({
      new DefineVariable(new ArrayValueTypeSpec(CreatePointerToPrimitiveTypeSpec("int"), 10), "ints"),
      new DefineVariable(new ArrayValueTypeSpec(CreatePointerToPrimitiveTypeSpec("float"), 10), "floats"),
      new DefineVariable(new ArrayValueTypeSpec(CreatePointerToPrimitiveTypeSpec("double"), 10), "doubles"),
      new DefineVariable(new ArrayValueTypeSpec(CreatePointerToPrimitiveTypeSpec("string"), 10), "strings"),
      new Return(new IntegerConst(0))
  });
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
      VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestDefineArrayOfPointers");
  module->AppendExpression(main);
  auto program = new Program();
  program->AddModule(module);
  std::cout << program->Compile() << std::endl;
  std::cout << "TestDefineArrayOfPointers succeeded." << std::endl;
  delete program;

  LlvmContainer::Finalize();

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

} } // namespace
