/**
 * @file Tests/ScgTests/unary_operators_tests.cpp
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

namespace Tests { namespace ScgTests
{

bool TestIncrementOperator()
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = Block::create({
    std::make_shared<DefineVariable>(CreateTypeSpecByName("int"), "intCounter"),
    std::make_shared<DefineVariable>(CreateTypeSpecByName("float"), "floatCounter"),
    std::make_shared<DefineVariable>(CreateTypeSpecByName("double"), "doubleCounter"),
    std::make_shared<UnaryOperator>(UnaryOperator::INCREMENT,
    std::make_shared<Content>(std::make_shared<IdentifierReference>("intCounter"))),
    std::make_shared<UnaryOperator>(UnaryOperator::INCREMENT,
    std::make_shared<Content>(std::make_shared<IdentifierReference>("floatCounter"))),
    std::make_shared<UnaryOperator>(UnaryOperator::INCREMENT,
    std::make_shared<Content>(std::make_shared<IdentifierReference>("doubleCounter"))),
    std::make_shared<Return>(std::make_shared<IntegerConst>(0))
  });
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
                                               VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestIncrementOperator");
  module->appendNode(main);
  auto program = new CodeGenUnit();
  program->addModule(module);
  std::cout << program->compile() << std::endl;
  std::cout << "TestIncrementOperator succeeded." << std::endl;
  delete program;

  LlvmContainer::finalize();

  return true;
}

bool RunAllUnaryOperatorsTests()
{
  auto ret = true;

  if (!TestIncrementOperator()) ret = false;

  return ret;
}

} } // namespace
