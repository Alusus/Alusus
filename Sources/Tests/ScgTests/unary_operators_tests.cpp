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
  LlvmContainer::Initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto mainBody = new Block({
      new DefineVariable(CreateTypeSpecByName("int"), "intCounter"),
      new DefineVariable(CreateTypeSpecByName("float"), "floatCounter"),
      new DefineVariable(CreateTypeSpecByName("double"), "doubleCounter"),
      new UnaryOperator(UnaryOperator::INCREMENT,
          new Content(new IdentifierReference("intCounter"))),
      new UnaryOperator(UnaryOperator::INCREMENT,
          new Content(new IdentifierReference("floatCounter"))),
      new UnaryOperator(UnaryOperator::INCREMENT,
          new Content(new IdentifierReference("doubleCounter"))),
      new Return(new IntegerConst(0))
  });
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
      VariableDefinitionArray(), mainBody);

  // Create the module.
  auto module = new Module("TestIncrementOperator");
  module->AppendExpression(main);
  auto program = new Program();
  program->AddModule(module);
  std::cout << program->Compile() << std::endl;
  std::cout << "TestIncrementOperator succeeded." << std::endl;
  delete program;

  LlvmContainer::Finalize();

  return true;
}

bool RunAllUnaryOperatorsTests()
{
  auto ret = true;
  if (!TestIncrementOperator()) ret = false;

  return ret;
}

} } // namespace
