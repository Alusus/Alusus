/**
 * @file Tests/ScgTests/structure_tests.cpp
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


bool TestDefineAndUseStructure()
{

  std::cout << RunSimpleTest({
    DEFINE_STRUCT("Point")
        STRUCT_FIELD("int", "x"),
        STRUCT_FIELD("int", "y")
    END_DEFINE_STRUCT(),
    DEF_VAR("Point", "point"),
    ASSIGN(FIELD(VAR_PTR("point"), "x"), INT_CONST(10)),
    ASSIGN(FIELD(VAR_PTR("point"), "y"), INT_CONST(5)),
    CALL("printf", STR_CONST("point.x = %d\n"), FIELD(VAR_PTR("point"), "x")),
    CALL("printf", STR_CONST("point.y = %d\n"), FIELD(VAR_PTR("point"), "y")),
    RET_ZERO()
  }, {
      new DeclareExtFunction("printf", TYPE("int"), {TYPE("string")}, true)
  });

  return true;
}

bool TestDefineAndUseArrayOfStructures()
{
  std::cout << RunSimpleTest({
    DEFINE_STRUCT("Point")
        STRUCT_FIELD("int", "x"),
        STRUCT_FIELD("int", "y")
    END_DEFINE_STRUCT(),
    DEF_ARRAY("Point", "points", 10),
    ASSIGN(FIELD_OF_ELEM("points", 5, "x"), INT_CONST(10)),
    ASSIGN(FIELD_OF_ELEM("points", 4, "y"), INT_CONST(5)),
    CALL("printf", STR_CONST("points[5].x = %d\n"), FIELD_OF_ELEM("points", 5, "x")),
    CALL("printf", STR_CONST("points[4].y = %d\n"), FIELD_OF_ELEM("points", 4, "y")),
    RET_ZERO()
  }, {
      new DeclareExtFunction("printf", TYPE("int"), {TYPE("string")}, true)
  });

  return true;
}

bool TestCallFunctionReturningStructure()
{
  std::cout << RunSimpleTest({
    DEF_VAR("Point", "point"),
    ASSIGN(VAR("point"), CALL("GetPoint", INT_CONST(10), INT_CONST(5))),
    CALL("printf", STR_CONST("point.x = %d\n"), FIELD(VAR_PTR("point"), "x")),
    CALL("printf", STR_CONST("point.y = %d\n"), FIELD(VAR_PTR("point"), "y")),
    RET_ZERO()
  }, {
      new DeclareExtFunction("printf", TYPE("int"), {TYPE("string")}, true),
      DEFINE_STRUCT("Point")
          STRUCT_FIELD("int", "x"),
          STRUCT_FIELD("int", "y")
      END_DEFINE_STRUCT(),
      new DefineFunction("GetPoint", TYPE("Point"),
          {FUNC_ARG("int", "x"), FUNC_ARG("int", "y")}, new Block({
              DEF_VAR("Point", "point"),
              ASSIGN(FIELD(VAR_PTR("point"), "x"), VAR("x")),
              ASSIGN(FIELD(VAR_PTR("point"), "y"), VAR("y")),
              RET(VAR("point"))
          }))
      });

  return true;
}

bool RunAllStructureTests()
{
  auto ret = true;
  if (!TestDefineAndUseStructure()) ret = false;
  if (!TestDefineAndUseArrayOfStructures()) ret = false;
  if (!TestCallFunctionReturningStructure()) ret = false;

  return ret;
}

} } // namespace
