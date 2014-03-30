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

#define DEFINE_STRUCT(name) new DefineStruct(name, {
#define STRUCT_FIELD(type, name) VariableDefinition(TYPE(type), name)
#define FUNC_ARG(type, name) VariableDefinition(TYPE(type), name)
#define END_DEFINE_STRUCT() })
#define DEF_VAR(type, name) new DefineVariable(TYPE(type), name)
#define DEF_ARRAY(type, name, size) new DefineVariable(CreateArrayOfPrimitiveTypeSpec(type, size), name)

#define TYPE(name) CreateTypeSpecByName(name)

// Referencing variables.
#define CONTENT(expr) new Content(expr)
#define VAR_PTR(name) new PointerToVariable(name)
#define VAR(name) CONTENT(VAR_PTR(name))
#define FIELD_PTR(var, field) new PointerToMemberField(var, field)
#define FIELD(var, field) CONTENT(FIELD_PTR(var, field))
#define ELEM_PTR(var, index) new PointerToArrayElement(var, new IntegerConst(index))
#define ELEM(var, index) CONTENT(ELEM_PTR(var, new IntegerConst(index)))
#define FIELD_OF_ELEM(name, index, field) CONTENT(FIELD_PTR(ELEM_PTR(VAR_PTR(name), index), field))

// Constants
#define INT_CONST(value) new IntegerConst(value)
#define STR_CONST(value) new StringConst(value)
#define ASSIGN(lhs, rhs) new AssignmentOperator(lhs, rhs)

// Different statements
#define CALL(name, ...) new CallFunction(name, new List({__VA_ARGS__}))
#define RET(expr) new Return(expr)
#define RET_ZERO() new Return(new IntegerConst(0))

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
