/**
 * @file Tests/ScgTests/hello_world_test.cpp
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

bool RunHelloWorldTest()
{
  std::cout << RunSimpleTest({
    new CallFunction("printf", new List({new StringConst("Hello World!\n")})),
    new Return(new IntegerConst(0))
  }, {
    new DeclareExtFunction("printf", CreateTypeSpecByName("int"),
    {CreateTypeSpecByName("string")}, true)
  });

  return true;
}

}
} // namespace
