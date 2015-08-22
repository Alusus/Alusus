/**
 * @file Tests/ScgTests/simple_test.cpp
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

using namespace Scg;

namespace Tests
{
namespace ScgTests
{

ValueTypeSpec *CreateTypeSpecByName(const std::string &name)
{
  return new ValueTypeSpecByName(name.c_str());
}

ValueTypeSpec *CreatePointerToPrimitiveTypeSpec(const std::string &name)
{
  return new PointerValueTypeSpec(new ValueTypeSpecByName(name.c_str()));
}

ValueTypeSpec *CreateArrayOfPrimitiveTypeSpec(const std::string &name, int size)
{
  return new ArrayValueTypeSpec(new ValueTypeSpecByName(name.c_str()), size);
}

std::string BuildSimpleTest(const ExpressionArray &mainBody,
                            const std::vector<Expression*> &headerStatements)
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
                                 VariableDefinitionArray(), new Block(mainBody));

  // Create the module.
  auto *module = new Module("MainModule");

  for (auto link : headerStatements)
    module->appendExpression(link);

  module->appendExpression(main);

  auto *program = new Program();
  program->addModule(module);

  // Generate the IR Code and assert that it matches what we expect.
  std::string irCode;
  irCode = program->compile();

  delete program;

  LlvmContainer::finalize();

  return irCode;
}

std::string RunSimpleTest(const ExpressionArray &mainBody,
                          const std::vector<Expression*> &headerStatements)
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
                                 VariableDefinitionArray(), new Block(mainBody));

  // Create the module.
  auto module = new Module("MainModule");

  for (auto link : headerStatements)
    module->appendExpression(link);

  module->appendExpression(main);

  auto *program = new Program();
  program->addModule(module);

  // Generate the IR Code and assert that it matches what we expect.
  std::string irCode;
  program->execute("MainModule_main");
  delete program;

  LlvmContainer::finalize();

  return irCode;
}

}
} // namespace
