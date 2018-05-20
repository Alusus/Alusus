/**
 * @file Spp/Ast/ast.h
 * Contains the definitions and include statements of all types in the Ast
 * namespace.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_AST_H
#define SPP_AST_AST_H

namespace Spp { namespace Ast
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

/**
 * @ingroup spp_ast
 */
ti_s_enum(CallMatchStatus, TiInt, "Spp.Ast", "Spp", "alusus.net", NONE = 0, CASTED = 1, DEREF = 2, EXACT = 3);

} } // namespace


//==============================================================================
// Classes

//// AST Classes
// Containers
#include "Block.h"
#include "Module.h"
#include "Template.h"
#include "Function.h"
// Types
#include "Type.h"
#include "VoidType.h"
#include "IntegerType.h"
#include "FloatType.h"
#include "PointerType.h"
#include "ReferenceType.h"
#include "ArrayType.h"
#include "UserType.h"
// Control Statements
#include "ForStatement.h"
#include "IfStatement.h"
#include "WhileStatement.h"
#include "ContinueStatement.h"
#include "BreakStatement.h"
#include "ReturnStatement.h"
// Operators
#include "PointerOp.h"
#include "ContentOp.h"
#include "CastOp.h"
#include "SizeOp.h"
// Misc
#include "ArgPack.h"

// Helpers
#include "Helper.h"
#include "NodePathResolver.h"
#include "metadata_helpers.h"

#endif
