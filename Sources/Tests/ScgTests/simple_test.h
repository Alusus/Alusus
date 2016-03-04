/**
 * @file Tests/ScgTests/simple_test.h
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

namespace Tests { namespace ScgTests
{

// Macros for constants
#undef STR
#define STR(x) std::make_shared<StringConst>((x))
#define INT(x) std::make_shared<IntegerConst>((x))
#define FLOAT(x) std::make_shared<FloatConst>((x))
#define DOUBLE(x) std::make_shared<DoubleConst>((x))

#define DEFINE_STRUCT(name) DefineStruct::create(name, {
#define STRUCT_FIELD(type, name) VariableDefinition(TYPE(type), name)
#define FUNC_ARG(type, name) VariableDefinition(TYPE(type), name)
#define END_DEFINE_STRUCT() })

#define TYPE(name) CreateTypeSpecByName(name)
#define DEF_VAR(type, name) std::make_shared<DefineVariable>(TYPE(type), name)
#define DEF_ARRAY(type, name, size) std::make_shared<DefineVariable>(CreateArrayOfPrimitiveTypeSpec(type, size), name)

// Referencing variables.
#define CONTENT(expr) std::make_shared<Content>(expr)
#define VAR_PTR(name) std::make_shared<IdentifierReference>(name)
#define VAR(name) CONTENT(VAR_PTR(name))
#define FIELD_PTR(var, field) std::make_shared<MemberFieldReference>(var, field)
#define FIELD(var, field) CONTENT(FIELD_PTR(var, field))
#define ELEM_PTR(var, index) std::make_shared<ArrayElementReference>(var, std::make_shared<IntegerConst>(index))
#define ELEM(var, index) CONTENT(ELEM_PTR(var, std::make_shared<IntegerConst>(index)))
#define FIELD_OF_ELEM(name, index, field) CONTENT(FIELD_PTR(ELEM_PTR(VAR_PTR(name), index), field))

// Constants
#define INT_CONST(value) std::make_shared<IntegerConst>(value)
#define STR_CONST(value) std::make_shared<StringConst>(value)
#define ASSIGN(lhs, rhs) std::make_shared<AssignmentOperator>(lhs, rhs)

// Different statements
#define CALL(name, ...) std::make_shared<CallFunction>((name), List::create({__VA_ARGS__}))
#define RET(expr) std::make_shared<Return>((expr))
#define RET_ZERO() std::make_shared<Return>(std::make_shared<IntegerConst>(0))
#define ASSIGN_VARS(lhs, rhs) ASSIGN(CONTENT(VAR_PTR((lhs))), CONTENT(VAR_PTR((rhs))))

SharedPtr<Scg::ValueTypeSpec> CreateTypeSpecByName(const std::string &name);
SharedPtr<Scg::ValueTypeSpec> CreatePointerToPrimitiveTypeSpec(const std::string &name);
SharedPtr<Scg::ValueTypeSpec> CreateArrayOfPrimitiveTypeSpec(const std::string &name, int size);
std::string BuildSimpleTest(const Scg::AstNodeSharedArray &mainBody,
                            const std::vector<SharedPtr<Scg::AstNode>> &linkStatements = {});
std::string RunSimpleTest(const Scg::AstNodeSharedArray &mainBody,
                          const std::vector<SharedPtr<Scg::AstNode>> &linkStatements = {});

} } // namespace
