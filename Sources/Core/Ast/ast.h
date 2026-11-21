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

#define IMPLEMENT_AST_PRINTABLE(type, extra) \
  public: virtual void print(OutStream &stream, Int indents=0) const \
  { \
    stream << S(#type " ") << extra; \
    Word id = this->getProdId(); \
    if (id != UNKNOWN_ID) { \
      stream << S(" [") << ID_GENERATOR->getDesc(id) << S("]"); \
    } \
    Node::printModifiers(stream, indents); \
    Node::printMetadata(stream, indents); \
    Node::printContaining(stream, indents); \
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
  class Identifier;
  class List;
  class Within;
  class Map;
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

/**
 * @brief Apply a Within object to a scope.
 * If the Within object has no qualifier then its statements are added to the target scope. Otherwise they are added to
 * the scope that the qualifier points to, which is looked up once for all statements.
 *
 * If the qualifier's target can't be found, what happens depends on the Within object's merge policy, unless
 * lastAttempt is true. With the IMMEDIATE policy, or if lastAttempt is true, an InvalidWithinTargetNotice is raised and
 * false is returned. With the DELAYED policy the target is not even looked up, and with the AUTO policy the lookup
 * happens but failing it is not an error. In both cases the Within object itself is added to the target scope at
 * index (or at the end if index is -1) so that it can be applied in a later phase, and true is returned.
 *
 * The caller is responsible for making sure the Within object isn't still part of the target when passing lastAttempt.
 * The qualifier is resolved starting from lookupContext, or from the target scope if lookupContext is null.
 */
Bool addWithinElements(
  Within *within, DynamicContaining<Node> *target, Int &index, Seeker *seeker, Notices::Store *noticeStore,
  Node *lookupContext = 0, Bool lastAttempt = false
);

/**
 * @brief Add an element to a scope, merging or expanding it when needed.
 * The element is merged into the existing definition if it's a definition to merge, and it's expanded if it's a
 * Within object. The qualifier of a Within object is resolved starting from lookupContext, or from the target scope if
 * lookupContext is null. Parsing handlers pass the parser's data stack as the lookup context since the scope being
 * built is not yet attached to the scopes that enclose it.
 */
Bool addPossiblyMergeableElement(
  Node *src, DynamicContaining<Node> *target, Int &index, Seeker *seeker, Notices::Store *noticeStore,
  Node *lookupContext = 0
);
Bool addPossiblyMergeableElement(
  Node *src, DynamicContaining<Node> *target, Seeker *seeker, Notices::Store *noticeStore,
  Node *lookupContext = 0
);
Bool addPossiblyMergeableElements(
  Containing<Node> *src, DynamicContaining<Node> *target, Int &index,
  Seeker *seeker, Notices::Store *noticeStore
);
Bool addPossiblyMergeableElements(
  Containing<Node> *src, DynamicContaining<Node> *target, Seeker *seeker, Notices::Store *noticeStore
);

/**
 * @brief Get the identifier that carries a modifier's keyword.
 * @ingroup core_data_ast
 *
 * A modifier can be a plain identifier (`shared`), a qualified identifier (`ns.shared`), or a param-passed
 * identifier (`expname[...]`). This function drills into a LinkOperator or ParamPass wrapping to find the
 * identifier that actually carries the modifier's keyword. Returns 0 if none was found.
 */
Identifier* getModifierKeywordIdentifier(Node *modifier);

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
#include "Within.h"
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
