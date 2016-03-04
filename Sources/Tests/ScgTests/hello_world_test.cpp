/**
 * @file Tests/ScgTests/hello_world_test.cpp
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
#include <simple_test.h>

using namespace Scg;

namespace Tests { namespace ScgTests
{

bool RunHelloWorldTest()
{
  std::cout << RunSimpleTest({
    std::make_shared<CallFunction>("printf", List::create({std::make_shared<StringConst>("Hello World!\n")})),
    std::make_shared<Return>(std::make_shared<IntegerConst>(0))
  }, {
    DeclareExtFunction::create("printf", CreateTypeSpecByName("int"),
      {CreateTypeSpecByName("string")}, true)
  });

  return true;
}

} } // namespace
