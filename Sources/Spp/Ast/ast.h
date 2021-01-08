/**
 * @file Spp/Ast/ast.h
 * Contains the definitions and include statements of all types in the Ast
 * namespace.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_AST_H
#define SPP_AST_AST_H

namespace Spp::Ast
{

/**
 * @defgroup spp_ast Ast
 * @ingroup spp
 * @brief Classes related to the SPP's AST.
 */

//==============================================================================
// Forward Class Declarations

class NodePathResolver;
class Helper;
class Type;


//==============================================================================
// Types

/// @ingroup spp_ast
s_enum(TypeMatchOptions,
  NONE = 0,
  SKIP_DEREF = 1
);

/// @ingroup spp_ast
class TypeMatchStatus : public TiObject
{
  TYPE_INFO(TypeMatchStatus, TiObject, "Spp.Ast", "Spp", "alusus.org");
  public:
  enum Status {
    NONE = 0,
    AGGREGATION = 1,
    EXPLICIT_CAST = 2,
    CUSTOM_CASTER = 3,
    IMPLICIT_CAST = 4,
    PROMOTION = 5,
    REF_AGGREGATION = 6,
    EXACT = 7
  };
  Int value;
  Int derefs;
  TypeMatchStatus() : value(0), derefs(0) {}
  TypeMatchStatus(Status v, Int d = 0) : value(v), derefs(d) {}
  TypeMatchStatus const& operator=(Status v) { this->value = v; return *this; }
  TypeMatchStatus const& operator=(TypeMatchStatus const &v) {
    this->value = v.value;
    this->derefs = v.derefs;
    return *this;
  }
  bool operator ==(TypeMatchStatus const &v) const { return this->value == v.value && this->derefs == v.derefs; }
  bool operator !=(TypeMatchStatus const &v) const { return this->value != v.value || this->derefs != v.derefs; }
  bool operator ==(Status v) const { return this->value == v; }
  bool operator !=(Status v) const { return this->value != v; }
  bool operator >(TypeMatchStatus const &v) const { return this->value > v.value; }
  bool operator >=(TypeMatchStatus const &v) const { return this->value >= v.value; }
  bool operator >(Status v) const { return this->value > v; }
  bool operator >=(Status v) const { return this->value >= v; }
  bool operator <(TypeMatchStatus const &v) const { return this->value < v.value; }
  bool operator <=(TypeMatchStatus const &v) const { return this->value <= v.value; }
  bool operator <(Status v) const { return this->value < v; }
  bool operator <=(Status v) const { return this->value <= v; }
};

/// @ingroup spp_ast
ti_s_enum(DefinitionDomain,
  TiInt, "Spp.Ast", "Spp", "alusus.org",
  FUNCTION = 0, OBJECT = 1, GLOBAL = 2
);

/// @ingroup spp_ast
struct CalleeLookupResult
{
  TypeMatchStatus matchStatus = TypeMatchStatus::NONE;
  PlainList<TiObject> stack;
  Type *type = 0;
  SharedPtr<Core::Notices::Notice> notice;
  Int thisIndex = -2;
};

/// @ingroup spp_ast
s_enum(TypeInitMethod,
  NONE = 0,
  AUTO = 1,
  USER = 2,
  BOTH = 3
);

} // namespace


//==============================================================================
// Type Names

DEFINE_TYPE_NAME(Spp::Ast::CalleeLookupResult, "alusus.org/Spp/Spp.Ast.CalleeLookupResult");


//==============================================================================
// Classes

//// AST Classes
#include "Block.h"
// Types
#include "Type.h"
#include "DataType.h"
#include "VoidType.h"
#include "IntegerType.h"
#include "FloatType.h"
#include "PointerType.h"
#include "ReferenceType.h"
#include "ArrayType.h"
#include "UserType.h"
#include "FunctionType.h"
#include "Macro.h"
// Containers
#include "Module.h"
#include "TemplateVarDef.h"
#include "Template.h"
#include "Function.h"
// Control Statements
#include "ForStatement.h"
#include "IfStatement.h"
#include "WhileStatement.h"
#include "ContinueStatement.h"
#include "BreakStatement.h"
#include "ReturnStatement.h"
#include "PreprocessStatement.h"
// Operators
#include "PointerOp.h"
#include "ContentOp.h"
#include "DerefOp.h"
#include "NoDerefOp.h"
#include "CastOp.h"
#include "SizeOp.h"
#include "TypeOp.h"
#include "AstRefOp.h"
#include "InitOp.h"
#include "TerminateOp.h"
#include "NextArgOp.h"
// Misc
#include "ArgPack.h"
#include "ThisTypeRef.h"
#include "AstLiteralCommand.h"

// Helpers
#include "Helper.h"
#include "NodePathResolver.h"
#include "metadata_helpers.h"

#endif
