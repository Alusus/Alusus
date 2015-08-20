/**
 * @file Tests/ScgTests/simple_test.h
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

namespace Tests { namespace ScgTests
{
// Macros for constants
#undef STR
#define STR(x) new StringConst((x))
#define INT(x) new IntegerConst((x))
#define FLOAT(x) new FloatConst((x))
#define DOUBLE(x) new DoubleConst((x))

#define DEFINE_STRUCT(name) new DefineStruct(name, {
#define STRUCT_FIELD(type, name) VariableDefinition(TYPE(type), name)
#define FUNC_ARG(type, name) VariableDefinition(TYPE(type), name)
#define END_DEFINE_STRUCT() })

#define TYPE(name) CreateTypeSpecByName(name)
#define DEF_VAR(type, name) new DefineVariable(TYPE(type), name)
#define DEF_ARRAY(type, name, size) new DefineVariable(CreateArrayOfPrimitiveTypeSpec(type, size), name)

// Referencing variables.
#define CONTENT(expr) new Content(expr)
#define VAR_PTR(name) new IdentifierReference(name)
#define VAR(name) CONTENT(VAR_PTR(name))
#define FIELD_PTR(var, field) new MemberFieldReference(var, field)
#define FIELD(var, field) CONTENT(FIELD_PTR(var, field))
#define ELEM_PTR(var, index) new ArrayElementReference(var, new IntegerConst(index))
#define ELEM(var, index) CONTENT(ELEM_PTR(var, new IntegerConst(index)))
#define FIELD_OF_ELEM(name, index, field) CONTENT(FIELD_PTR(ELEM_PTR(VAR_PTR(name), index), field))

// Constants
#define INT_CONST(value) new IntegerConst(value)
#define STR_CONST(value) new StringConst(value)
#define ASSIGN(lhs, rhs) new AssignmentOperator(lhs, rhs)

// Different statements
#define CALL(name, ...) new CallFunction((name), new List({__VA_ARGS__}))
#define RET(expr) new Return((expr))
#define RET_ZERO() new Return(new IntegerConst(0))
#define ASSIGN_VARS(lhs, rhs) ASSIGN(CONTENT(VAR_PTR((lhs))), CONTENT(VAR_PTR((rhs))))

Scg::ValueTypeSpec *CreateTypeSpecByName(const std::string &name);
Scg::ValueTypeSpec *CreatePointerToPrimitiveTypeSpec(const std::string &name);
Scg::ValueTypeSpec *CreateArrayOfPrimitiveTypeSpec(const std::string &name, int size);
std::string BuildSimpleTest(const Scg::ExpressionArray &mainBody,
    const std::vector<Scg::Expression*> &linkStatements = {});
std::string RunSimpleTest(const Scg::ExpressionArray &mainBody,
    const std::vector<Scg::Expression*> &linkStatements = {});

} } // namespace
