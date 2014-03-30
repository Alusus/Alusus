/**
 * @file Tests/ScgTests/binary_operators_tests.cpp
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

bool TestAssignmentOperator()
{
  LlvmContainer::Initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = new Block({
      new DefineVariable(CreateTypeSpecByName("int"), "a"),
      new DefineVariable(CreateTypeSpecByName("int"), "b"),
      new AssignmentOperator(
          new Content(new PointerToVariable("a")),
          new Content(new PointerToVariable("b"))),
      new Return(new IntegerConst(0))
  });
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
      VariableDefinitionArray(), mainBody);

  // Create the module.
  Module module("TestAssignmentOperator");
  module.AppendExpression(main);

  // Generate the IR Code and assert that it matches what we expect.
  std::string irCode;
  module.Compile(irCode);

  std::cout << irCode << std::endl;

  std::cout << "TestAssignmentOperator succeeded." << std::endl;

  LlvmContainer::Finalize();

  return true;
}

bool TestBinaryOperators()
{
  LlvmContainer::Initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = new Block({
      new DefineVariable(CreateTypeSpecByName("int"), "a"),
      new DefineVariable(CreateTypeSpecByName("int"), "b"),
      new BinaryOperator(BinaryOperator::ADD,
          new Content(new PointerToVariable("a")),
          new Content(new PointerToVariable("b"))),
      new BinaryOperator(BinaryOperator::SUBTRACT,
          new Content(new PointerToVariable("a")),
          new Content(new PointerToVariable("b"))),
      new BinaryOperator(BinaryOperator::MULTIPLY,
          new Content(new PointerToVariable("a")),
          new Content(new PointerToVariable("b"))),
      // TODO: Add this test when division is implemented.
      //new BinaryOperator(BinaryOperator::DIVISION,
      //    new Content(new PointerToVariable("a")),
      //    new Content(new PointerToVariable("b"))),
      new BinaryOperator(BinaryOperator::GREATERTHAN,
          new Content(new PointerToVariable("a")),
          new Content(new PointerToVariable("b"))),
      new BinaryOperator(BinaryOperator::GREATERTHAN_EQUAL,
          new Content(new PointerToVariable("a")),
          new Content(new PointerToVariable("b"))),
      new BinaryOperator(BinaryOperator::LESSTHAN,
          new Content(new PointerToVariable("a")),
          new Content(new PointerToVariable("b"))),
      new BinaryOperator(BinaryOperator::LESSTHAN_EQUAL,
          new Content(new PointerToVariable("a")),
          new Content(new PointerToVariable("b"))),
      new BinaryOperator(BinaryOperator::EQUAL,
          new Content(new PointerToVariable("a")),
          new Content(new PointerToVariable("b"))),
      new BinaryOperator(BinaryOperator::NOTEQUAL,
          new Content(new PointerToVariable("a")),
          new Content(new PointerToVariable("b"))),
      new Return(new IntegerConst(0))
  });
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
      VariableDefinitionArray(), mainBody);

  // Create the module.
  Module module("TestBinaryOperators");
  module.AppendExpression(main);

  // Generate the IR Code and assert that it matches what we expect.
  std::string irCode;
  module.Compile(irCode);

  std::cout << irCode << std::endl;

  std::cout << "TestBinaryOperators succeeded." << std::endl;

  LlvmContainer::Finalize();

  return true;
}

bool RunAllBinaryOperatorsTests()
{
  auto ret = true;
  if (!TestAssignmentOperator()) ret = false;
  if (!TestBinaryOperators()) ret = false;

  return ret;
}

} } // namespace
