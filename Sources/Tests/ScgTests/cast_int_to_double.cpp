/**
 * @file Tests/ScgTests/cast_int_to_float.cpp
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


namespace Tests { namespace ScgTests
{

bool RunCastIntToFloatTest()
{
  std::cout << RunSimpleTest({
    DEF_VAR("int", "intValue"),
    ASSIGN(VAR("intValue"), INT(5)),
    DEF_VAR("double", "doubleValue"),
    ASSIGN(VAR("doubleValue"), new CastIntToDouble(VAR("intValue"))),
    CALL("printf", STR("%f!\n"), VAR("doubleValue")),
    RET(INT(0))
  }, {
      new DeclareExtFunction("printf", CreateTypeSpecByName("int"),
          {CreateTypeSpecByName("string")}, true)
  });

  return true;
}

} } // namespace
