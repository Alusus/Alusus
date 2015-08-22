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
#include <core.h>
#include <scg.h>
#include <simple_test.h>

using namespace Scg;

namespace Tests
{
namespace ScgTests
{

bool TestAssignmentOperator()
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = new Block({
    new DefineVariable(CreateTypeSpecByName("int"), "a"),
    new DefineVariable(CreateTypeSpecByName("int"), "b"),
    new AssignmentOperator(
      new Content(new IdentifierReference("a")),
      new Content(new IdentifierReference("b"))),
    new Return(new IntegerConst(0))
  });
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
                                 VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestAssignmentOperator");
  module->appendExpression(main);
  auto program = new Program();
  program->addModule(module);
  std::cout << program->compile() << std::endl;
  std::cout << "TestAssignmentOperator succeeded." << std::endl;
  delete program;

  LlvmContainer::finalize();

  return true;
}

bool TestBinaryOperators()
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = new Block({
    new DefineVariable(CreateTypeSpecByName("int"), "a"),
    new DefineVariable(CreateTypeSpecByName("int"), "b"),
    new CallFunction("__add", new List({
      new Content(new IdentifierReference("a")),
      new Content(new IdentifierReference("b"))
    })),
    new CallFunction("__sub", new List({
      new Content(new IdentifierReference("a")),
      new Content(new IdentifierReference("b"))
    })),
    new CallFunction("__mul", new List({
      new Content(new IdentifierReference("a")),
      new Content(new IdentifierReference("b"))
    })),
    new CallFunction("__div", new List({
      new Content(new IdentifierReference("a")),
      new Content(new IdentifierReference("b"))
    })),
    new BinaryOperator(BinaryOperator::GREATERTHAN,
    new Content(new IdentifierReference("a")),
    new Content(new IdentifierReference("b"))),
    new BinaryOperator(BinaryOperator::GREATERTHAN_EQUAL,
    new Content(new IdentifierReference("a")),
    new Content(new IdentifierReference("b"))),
    new BinaryOperator(BinaryOperator::LESSTHAN,
    new Content(new IdentifierReference("a")),
    new Content(new IdentifierReference("b"))),
    new BinaryOperator(BinaryOperator::LESSTHAN_EQUAL,
    new Content(new IdentifierReference("a")),
    new Content(new IdentifierReference("b"))),
    new BinaryOperator(BinaryOperator::EQUAL,
    new Content(new IdentifierReference("a")),
    new Content(new IdentifierReference("b"))),
    new BinaryOperator(BinaryOperator::NOTEQUAL,
    new Content(new IdentifierReference("a")),
    new Content(new IdentifierReference("b"))),
    new Return(new IntegerConst(0))
  });
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
                                 VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestBinaryOperators");
  module->appendExpression(main);
  auto program = new Program();
  program->addModule(module);
  std::cout << program->compile() << std::endl;
  std::cout << "TestBinaryOperators succeeded." << std::endl;
  delete program;

  LlvmContainer::finalize();

  return true;
}

bool RunAllBinaryOperatorsTests()
{
  auto ret = true;

  if (!TestAssignmentOperator()) ret = false;

  if (!TestBinaryOperators()) ret = false;

  return ret;
}

}
} // namespace
