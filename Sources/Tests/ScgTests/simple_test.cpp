/**
 * @file Tests/ScgTests/simple_test.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
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

SharedPtr<ValueTypeSpec> CreateTypeSpecByName(const std::string &name)
{
  return std::make_shared<ValueTypeSpecByName>(name.c_str());
}

SharedPtr<ValueTypeSpec> CreatePointerToPrimitiveTypeSpec(const std::string &name)
{
  return std::make_shared<PointerValueTypeSpec>(std::make_shared<ValueTypeSpecByName>(name.c_str()));
}

SharedPtr<ValueTypeSpec> CreateArrayOfPrimitiveTypeSpec(const std::string &name, int size)
{
  return std::make_shared<ArrayValueTypeSpec>(std::make_shared<ValueTypeSpecByName>(name.c_str()), size);
}

std::string BuildSimpleTest(const AstNodeSharedArray &mainBody,
                            const std::vector<SharedPtr<AstNode>> &headerStatements)
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
                                               VariableDefinitionArray(), std::make_shared<Block>(mainBody));

  // Create the module.
  auto *module = new Module("MainModule");

  for (auto link : headerStatements)
    module->appendNode(link);

  module->appendNode(main);

  auto *program = new CodeGenUnit();
  program->addModule(module);

  // Generate the IR Code and assert that it matches what we expect.
  std::string irCode;
  irCode = program->compile();

  delete program;

  LlvmContainer::finalize();

  return irCode;
}

std::string RunSimpleTest(const AstNodeSharedArray &mainBody,
                          const std::vector<SharedPtr<AstNode>> &headerStatements)
{
  LlvmContainer::initialize();

  // Create a type specification for an integer.
  // Create the main function.
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
                                               VariableDefinitionArray(), std::make_shared<Block>(mainBody));

  // Create the module.
  auto module = new Module("MainModule");

  for (auto link : headerStatements)
    module->appendNode(link);

  module->appendNode(main);

  auto *program = new CodeGenUnit();
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
