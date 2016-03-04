/**
 * @file Tests/ScgTests/global_variable.cpp
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

// TODO: This was copied from StructureTests.cpp. It should be defined in
// a separate header file instead to avoid duplication.

namespace Tests { namespace ScgTests
{

bool TestGlobalVariable()
{
  LlvmContainer::initialize();

  // Defines the global variable.
  auto defGlobInt = std::make_shared<DefineVariable>(CreateTypeSpecByName("int"), "globVarInt");
  auto defGlobDouble = std::make_shared<DefineVariable>(CreateTypeSpecByName("double"), "globVarDouble");

  // Creates a link to printf.
  auto printfLink = DeclareExtFunction::create("printf", CreateTypeSpecByName("int"),
                                               {CreateTypeSpecByName("string")}, true);

  // Creates the main function.
  auto main = std::make_shared<DefineFunction>("main", std::make_shared<ValueTypeSpecByName>("int"),
    VariableDefinitionArray(), Block::create({
      std::make_shared<AssignmentOperator>(VAR("globVarInt"), std::make_shared<IntegerConst>(10)),
      std::make_shared<AssignmentOperator>(VAR("globVarDouble"), std::make_shared<DoubleConst>(20.0)),
      std::make_shared<CallFunction>("printf", List::create({std::make_shared<StringConst>("The value of the integer global variable is %d\n"), VAR("globVarInt") })),
      std::make_shared<CallFunction>("printf", List::create({std::make_shared<StringConst>("The value of the double global variable is %f\n"), VAR("globVarDouble") })),
      std::make_shared<Return>(std::make_shared<IntegerConst>(0))
    }));

  // Creates the module.
  auto *module = new Module("MainModule");
  module->appendNode(defGlobInt);
  module->appendNode(defGlobDouble);
  module->appendNode(printfLink);
  module->appendNode(main);
  auto program = new CodeGenUnit();
  program->addModule(module);
  std::cout << program->compile() << std::endl;
  delete program;

  LlvmContainer::finalize();

  return true;
}

} } // namespace
