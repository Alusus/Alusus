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
#include <scg.h>
#include <simple_test.h>

using namespace Scg;

// TODO: This was copied from StructureTests.cpp. It should be defined in
// a separate header file instead to avoid duplication.
#define CONTENT(expr) new Content(expr)
#define VAR_PTR(name) new PointerToVariable(name)
#define VAR(name) CONTENT(VAR_PTR(name))

namespace Tests { namespace ScgTests
{
bool TestGlobalvariable()
{
  LlvmContainer::Initialize();

  // Defines the global variable.
  auto defGlobInt = new DefineVariable(CreateTypeSpecByName("int"), "globVarInt");
  auto defGlobDouble = new DefineVariable(CreateTypeSpecByName("double"), "globVarDouble");

  // Creates a link to printf.
  auto printfLink = new DeclareExtFunction("printf", CreateTypeSpecByName("int"),
      {CreateTypeSpecByName("string")}, true);

  // Creates the main function.
  auto main = new DefineFunction("main", new ValueTypeSpecByName("int"),
      VariableDefinitionArray(), new Block({
      new AssignmentOperator(VAR("globVarInt"), new IntegerConst(10)),
      new AssignmentOperator(VAR("globVarDouble"), new DoubleConst(20.0)),
      new CallFunction("printf", new List({new StringConst("The value of the integer global variable is %d\n"), VAR("globVarInt") })),
      new CallFunction("printf", new List({new StringConst("The value of the double global variable is %f\n"), VAR("globVarDouble") })),
      new Return(new IntegerConst(0))
  }));

  // Creates the module.
  Module module("MainModule");
  module.AppendExpression(defGlobInt);
  module.AppendExpression(defGlobDouble);
  module.AppendExpression(printfLink);
  module.AppendExpression(main);
  module.Execute("main");
  //module.Compile();

  LlvmContainer::Finalize();

  return true;
}
} } // namespace
