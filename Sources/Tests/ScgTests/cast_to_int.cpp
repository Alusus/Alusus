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
#include <core.h>
#include <scg.h>
#include <simple_test.h>

using namespace Scg;


namespace Tests { namespace ScgTests
{

bool RunCastToIntTest()
{
  std::cout << RunSimpleTest({
    DEF_VAR("int", "intValue"),
    ASSIGN(VAR("intValue"), INT(5)),
    DEF_VAR("float", "floatValue"),
    ASSIGN(VAR("floatValue"), FLOAT(5.0f)),
    DEF_VAR("double", "doubleValue"),
    ASSIGN(VAR("doubleValue"), DOUBLE(5.0)),
    CALL("printf", STR("%d!\n"), new CastToInt(VAR("intValue"))),
    CALL("printf", STR("%d!\n"), new CastToInt(VAR("floatValue"))),
    CALL("printf", STR("%d!\n"), new CastToInt(VAR("doubleValue"))),
    RET(INT(0))
  }, {
      new DeclareExtFunction("printf", CreateTypeSpecByName("int"),
          {CreateTypeSpecByName("string")}, true)
  });

  return true;
}

} } // namespace
