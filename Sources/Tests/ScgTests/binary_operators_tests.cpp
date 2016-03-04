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
  auto mainBody = Block::create({
    std::make_shared<DefineVariable>(CreateTypeSpecByName("int"), "a"),
    std::make_shared<DefineVariable>(CreateTypeSpecByName("int"), "b"),
    std::make_shared<AssignmentOperator>(
      std::make_shared<Content>(std::make_shared<IdentifierReference>("a")),
      std::make_shared<Content>(std::make_shared<IdentifierReference>("b"))),
    std::make_shared<Return>(std::make_shared<IntegerConst>(0))
  });
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
                                               VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestAssignmentOperator");
  module->appendNode(main);
  auto program = new CodeGenUnit();
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
  auto mainBody = Block::create({
    std::make_shared<DefineVariable>(CreateTypeSpecByName("int"), "a"),
    std::make_shared<DefineVariable>(CreateTypeSpecByName("int"), "b"),
    std::make_shared<CallFunction>("__add", List::create({
      std::make_shared<Content>(std::make_shared<IdentifierReference>("a")),
      std::make_shared<Content>(std::make_shared<IdentifierReference>("b"))
    })),
    std::make_shared<CallFunction>("__sub", List::create({
      std::make_shared<Content>(std::make_shared<IdentifierReference>("a")),
      std::make_shared<Content>(std::make_shared<IdentifierReference>("b"))
    })),
    std::make_shared<CallFunction>("__mul", List::create({
      std::make_shared<Content>(std::make_shared<IdentifierReference>("a")),
      std::make_shared<Content>(std::make_shared<IdentifierReference>("b"))
    })),
    std::make_shared<CallFunction>("__div", List::create({
      std::make_shared<Content>(std::make_shared<IdentifierReference>("a")),
      std::make_shared<Content>(std::make_shared<IdentifierReference>("b"))
    })),
    std::make_shared<BinaryOperator>(BinaryOperator::GREATERTHAN,
    std::make_shared<Content>(std::make_shared<IdentifierReference>("a")),
    std::make_shared<Content>(std::make_shared<IdentifierReference>("b"))),
    std::make_shared<BinaryOperator>(BinaryOperator::GREATERTHAN_EQUAL,
    std::make_shared<Content>(std::make_shared<IdentifierReference>("a")),
    std::make_shared<Content>(std::make_shared<IdentifierReference>("b"))),
    std::make_shared<BinaryOperator>(BinaryOperator::LESSTHAN,
    std::make_shared<Content>(std::make_shared<IdentifierReference>("a")),
    std::make_shared<Content>(std::make_shared<IdentifierReference>("b"))),
    std::make_shared<BinaryOperator>(BinaryOperator::LESSTHAN_EQUAL,
    std::make_shared<Content>(std::make_shared<IdentifierReference>("a")),
    std::make_shared<Content>(std::make_shared<IdentifierReference>("b"))),
    std::make_shared<BinaryOperator>(BinaryOperator::EQUAL,
    std::make_shared<Content>(std::make_shared<IdentifierReference>("a")),
    std::make_shared<Content>(std::make_shared<IdentifierReference>("b"))),
    std::make_shared<BinaryOperator>(BinaryOperator::NOTEQUAL,
    std::make_shared<Content>(std::make_shared<IdentifierReference>("a")),
    std::make_shared<Content>(std::make_shared<IdentifierReference>("b"))),
    std::make_shared<Return>(std::make_shared<IntegerConst>(0))
  });
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
                                               VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestBinaryOperators");
  module->appendNode(main);
  auto program = new CodeGenUnit();
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
