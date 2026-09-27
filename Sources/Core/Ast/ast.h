/**
 * @file Core/Ast/ast.h
 * Contains the definitions and include statements of all types in the Core::Ast
 * namespace.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_AST_H
#define CORE_AST_AST_H

namespace Core::Ast
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
      stream << this->getElementKey(i) << S(": "); \
      Core::Ast::dumpAst(stream, this->getElement(i), indents+1); \
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
      Core::Ast::dumpAst(stream, this->getElement(i), indents+1); \
    } \
  }


//==============================================================================
// Data Types

ti_s_enum(BracketType, TiInt, "Core.Ast", "Core", "alusus.org", ROUND, SQUARE);

}


//==============================================================================
// Forward Defnitions

namespace Core::Ast
{
  class Node;
  class Definition;
  class Seeker;
  class SourceLocation;
  class SourceLocationStack;
}

namespace Core::Notices
{
  class Notice;
  class Store;
}


//==============================================================================
// Global Functions

namespace Core::Ast
{

/**
 * @brief Find an object in the chain of owners with the given type.
 * @ingroup core_data_ast
 * If the given object is of the given type, it will be returned.
 */
Node* findOwner(Node *obj, TypeInfo const *typeInfo);

template <class T> T* findOwner(Node *obj)
{
  return static_cast<T*>(findOwner(obj, T::getTypeInfo()));
}

SharedPtr<SourceLocation> const& findSourceLocation(Node const *node);

void addSourceLocation(Node *node, SourceLocation *sl);

Bool mergeDefinition(
  Definition *def, DynamicContaining<Node> *target, Int &index, Seeker *seeker, Notices::Store *noticeStore
);
Bool addPossiblyMergeableElement(
  Node *src, DynamicContaining<Node> *target, Int &index, Seeker *seeker, Notices::Store *noticeStore
);
Bool addPossiblyMergeableElement(
  Node *src, DynamicContaining<Node> *target, Seeker *seeker, Notices::Store *noticeStore
);
Bool addPossiblyMergeableElements(
  Containing<Node> *src, DynamicContaining<Node> *target, Int &index,
  Seeker *seeker, Notices::Store *noticeStore
);
Bool addPossiblyMergeableElements(
  Containing<Node> *src, DynamicContaining<Node> *target, Seeker *seeker, Notices::Store *noticeStore
);
void translateModifier(Grammar::SymbolDefinition *symbolDef, Node *modifier);

SharedPtr<Node> _clone(Node *obj, SourceLocation *sl);
template <class T> SharedPtr<T> clone(T *obj, SourceLocation *sl = 0)
{
  return _clone(obj, sl).template s_cast<T>();
}

SharedPtr<SourceLocation> cloneSourceLocation(SourceLocation const *sl);

Bool isEqual(Node *obj1, Node *obj2);
Bool _isEqual(TiObject *obj1, TiObject *obj2);

/**
 * @brief Print the given object to the given stream.
 * @ingroup core_data_ast
 *
 * If the object is an AST node, it will pass the call to Node::print, otherwise
 * it will print the object type and its value if it's a basic type.
 */
void dumpAst(OutStream &stream, TiObject *ptr, int indents);

SharedPtr<SourceLocation> concatSourceLocation(SourceLocation *sl1, SourceLocation *sl2);

SharedPtr<SourceLocation> concatFlattenedSourceLocation(SourceLocation *sl, SourceLocationStack const &stack);

Bool isEqual(SourceLocation const *sl1, SourceLocation const *sl2);

} // namespace

#include "source_location.h"

#include "Node.h"
#include "NodeStack.h"
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
#include "Passage.h"

namespace Core::Ast
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

// Operations
#include "Seeker.h"

#endif
