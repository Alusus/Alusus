/**
 * @file Tests/ScgTests/hello_world_two_modules_test.cpp
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

Module *CreateMainModule()
{
  // Creates a link to the Print function in PrintModule.
  /*auto printLink = new DeclareExtFunction("Print", nullptr,
      {CreateTypeSpecByName("string")}, false);*/

  // Creates the main function.
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
    VariableDefinitionArray(), Block::create({
      std::make_shared<CallFunction>("Print", List::create({
        std::make_shared<StringConst>(
          "Hello World, from the other module!\n")
      })),
      std::make_shared<Return>(std::make_shared<IntegerConst>(0))
    }));

  // Creates the module.
  auto module = new Module("MainModule");
  //module->AppendExpression(printLink);
  module->appendNode(main);

  return module;
}

Module *CreatePrintModule()
{
  // Creates the link to printf function.
  auto printfLink = DeclareExtFunction::create("printf", CreateTypeSpecByName("int"),
                                               {CreateTypeSpecByName("string")}, true);

  // Creates the print function.
  auto print = DefineFunction::create("Print", nullptr, {
    VariableDefinition("string", "output")
  }, Block::create({
    std::make_shared<CallFunction>("printf", List::create({std::make_shared<Content>(std::make_shared<IdentifierReference>("output"))}))
  }));

  // Creates the module.
  auto module = new Module("PrintModule");
  module->appendNode(printfLink);
  module->appendNode(print);

  return module;
}

bool RunHelloWorldTwoModulesTest()
{
  LlvmContainer::initialize();

  CodeGenUnit *program = new CodeGenUnit();
  program->addModule(CreateMainModule());
  program->addModule(CreatePrintModule());
  //std::cout << program->Compile();
  program->execute("MainModule_main");
  delete program;

  LlvmContainer::finalize();

  return true;
}

} } // namespace
