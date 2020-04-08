/**
 * @file Core/Data/Ast/ast.h
 * Contains the definitions and include statements of all types in the Data::Ast
 * namespace.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_AST_H
#define CORE_DATA_AST_AST_H

namespace Core::Data::Ast
{

/**
 * @defgroup core_data_ast AST Classes
 * @ingroup core_data
 * @brief Abstract Syntax Tree classes.
 */


//==============================================================================
// Macros

#define _PRINT_AST_TYPE_NAME1(type) stream << S(#type)
#define _PRINT_AST_TYPE_NAME2(type, extra) stream << S(#type " ") extra
#define _PRINT_AST_TYPE_NAME(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, _, _, _, _PRINT_AST_TYPE_NAME2, _PRINT_AST_TYPE_NAME1)(__VA_ARGS__)

#define IMPLEMENT_AST_MAP_PRINTABLE(...) \
  public: virtual void print(OutStream &stream, Int indents=0) const \
  { \
    _PRINT_AST_TYPE_NAME(__VA_ARGS__); \
    Word id = this->getProdId(); \
    if (id != UNKNOWN_ID) { \
      stream << S(" [") << ID_GENERATOR->getDesc(id) << S("]"); \
    } \
    for (Word i = 0; i < this->getElementCount(); ++i) { \
      stream << S("\n"); \
      printIndents(stream, indents+1); \
      stream << this->getElementKey(i).c_str() << S(": "); \
      Core::Data::dumpData(stream, this->getElement(i), indents+1); \
    } \
  }

#define IMPLEMENT_AST_LIST_PRINTABLE(...) \
  public: virtual void print(OutStream &stream, Int indents=0) const \
  { \
    _PRINT_AST_TYPE_NAME(__VA_ARGS__); \
    Word id = this->getProdId(); \
    if (id != UNKNOWN_ID) { \
      stream << S(" [") << ID_GENERATOR->getDesc(id) << S("]"); \
    } \
    for (Word i = 0; i < this->getElementCount(); ++i) { \
      stream << S("\n"); \
      printIndents(stream, indents+1); \
      Core::Data::dumpData(stream, this->getElement(i), indents+1); \
    } \
  }


//==============================================================================
// Data Types

ti_s_enum(BracketType, TiInt, "Core.Data.Ast", "Core", "alusus.org", ROUND, SQUARE);


//==============================================================================
// Forward Defnitions

class Definition;


//==============================================================================
// Global Functions

SharedPtr<SourceLocation> const& findSourceLocation(TiObject const *obj);

void addSourceLocation(TiObject *obj, SourceLocation *sl);

Bool mergeDefinition(Definition *def, DynamicContaining<TiObject> *target, Notices::Store *noticeStore);
Bool addPossiblyMergeableElement(TiObject *src, DynamicContaining<TiObject> *target, Notices::Store *noticeStore);
Bool addPossiblyMergeableElements(
  Containing<TiObject> *src, DynamicContaining<TiObject> *target, Notices::Store *noticeStore
);
void translateModifier(Data::Grammar::SymbolDefinition *symbolDef, TiObject *modifier);

TioSharedPtr _clone(TioSharedPtr const &obj, SourceLocation *sl);
template <class T> SharedPtr<T> clone(SharedPtr<T> const &obj, SourceLocation *sl = 0)
{
  return _clone(obj, sl).template s_cast<T>();
}

} // namespace

#include "MetaHaving.h"
#include "Mergeable.h"

#include "List.h"
#include "MergeList.h"
#include "Map.h"

#include "Token.h"
#include "Route.h"

#include "Definition.h"
#include "Bridge.h"
#include "Scope.h"
#include "ParamPass.h"
#include "InfixOperator.h"
#include "OutfixOperator.h"
#include "Text.h"
#include "Bracket.h"
#include "GenericCommand.h"
#include "Alias.h"

namespace Core::Data::Ast
{

DEFINE_AST_OUTFIX_OPERATOR(PrefixOperator);
DEFINE_AST_OUTFIX_OPERATOR(PostfixOperator);

DEFINE_AST_INFIX_OPERATOR(AssignmentOperator);
DEFINE_AST_INFIX_OPERATOR(ComparisonOperator);
DEFINE_AST_INFIX_OPERATOR(AdditionOperator);
DEFINE_AST_INFIX_OPERATOR(MultiplicationOperator);
DEFINE_AST_INFIX_OPERATOR(BitwiseOperator);
DEFINE_AST_INFIX_OPERATOR(LogOperator);
DEFINE_AST_INFIX_OPERATOR(LinkOperator);
DEFINE_AST_INFIX_OPERATOR(ConditionalOperator);

DEFINE_AST_TEXT_ELEMENT(Identifier);
DEFINE_AST_TEXT_ELEMENT(IntegerLiteral);
DEFINE_AST_TEXT_ELEMENT(FloatLiteral);
DEFINE_AST_TEXT_ELEMENT(CharLiteral);
DEFINE_AST_TEXT_ELEMENT(StringLiteral);

} // namespace

#endif
