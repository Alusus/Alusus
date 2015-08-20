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
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
      VariableDefinitionArray(), new Block({
      new CallFunction("Print", new List({new StringConst(
          "Hello World, from the other module!\n")})),
      new Return(new IntegerConst(0))
  }));

  // Creates the module.
  auto module = new Module("MainModule");
  //module->AppendExpression(printLink);
  module->AppendExpression(main);

  return module;
}

Module *CreatePrintModule()
{
  // Creates the link to printf function.
  auto printfLink = new DeclareExtFunction("printf", CreateTypeSpecByName("int"),
          {CreateTypeSpecByName("string")}, true);

  // Creates the print function.
  auto print = new DefineFunction("Print", nullptr, {
      VariableDefinition("string", "output")
  }, new Block({
    new CallFunction("printf", new List({new Content(new IdentifierReference("output"))}))
  }));

  // Creates the module.
  auto module = new Module("PrintModule");
  module->AppendExpression(printfLink);
  module->AppendExpression(print);

  return module;
}

bool RunHelloWorldTwoModulesTest()
{
  LlvmContainer::Initialize();

  Program *program = new Program();;
  program->AddModule(CreateMainModule());
  program->AddModule(CreatePrintModule());
  //std::cout << program->Compile();
  program->Execute("MainModule_main");
  delete program;

  LlvmContainer::Finalize();

  return true;
}

} } // namespace
