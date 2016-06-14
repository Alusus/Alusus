/**
 * @file Core/Data/data.h
 * Contains the definitions and include statements of all types in the Data
 * namespace.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_DATA_H
#define CORE_DATA_DATA_H

namespace Core { namespace Data
{

/**
 * @defgroup data Data
 * @ingroup core
 * @brief Classes that contains definitions data.
 *
 * This namespace contains definitions for classes and types that are used to
 * hold the data used in definitions.
 */

/**
 * @defgroup data_containers Container Classes
 * @ingroup data
 * @brief Classes for objects that contain other data objects.
 */

/**
 * @defgroup data_terms Term Classes
 * @ingroup data
 * @brief Terms of symbol formulas.
 */

/**
 * @defgroup data_ast AST Classes
 * @ingroup data
 * @brief Abstract Syntax Tree classes.
 */


//==============================================================================
// Data Types

/**
 * @brief Specifies the repository index of each grammar scope.
 * Each of the scopes will have a fixed index within the repository.
 */
s_enum(GrammarScopeIndex, ROOT=0, MODULE=1, PMODULE=2, STACK=3, ARGS=4);

/**
 * @brief An enumeration that specifies the usage criteria of references.
 * The usage criteria is used by references to determine caching criteria.
 * References can cache the indexes of referenced data to avoid repeating
 * the search. By knowing the usage criteria references will be able to
 * determine whether the indexes can be cached.
 *
 * SINGLE_DATA_SINGLE_MATCH: The reference will always be used against the same
 *                           data repository, and will only be used to match a
 *                           single target.
 * SINGLE_DATA_MULTI_MATCH: The reference will always be used against the same
 *                          data repository, but can match multiple targets.
 * MULTI_DATA: The reference can be used against different data repositories.
 */
s_enum(ReferenceUsageCriteria, SINGLE_DATA_SINGLE_MATCH, SINGLE_DATA_MULTI_MATCH, MULTI_DATA);

/**
 * @brief An enumeration for the list of operations for references.
 * References use lambdas to determine the operation to perform.and those
 * lambdas can return one of the values in this enumeration to tell the
 * Reference object what operation to perform.
 *
 * MOVE: Move to the next element matched by this reference.
 * STOP: Stop the matching and ignore any other possible matches.
 * PERFORM_AND_MOVE: Perform the requested operation on this match, then move
 *                   to the next match.
 * PERFORM_AND_STOP: Perform the requested operation on this match, then stop
 *                   and ignore other possible matches.
 */
s_enum(RefOp, MOVE, STOP, PERFORM_AND_MOVE, PERFORM_AND_STOP);

/**
 * @brief An enumeration used to define term flags.
 * @ingroup data
 *
 * ERROR_SYNC_TERM: Specifies that the given term is an error sync term, which
 *                  means that in the case of syntax errors the parser should
 *                  wait at that term for a sync token.
 * ONE_ROUTE_TERM: Specifies that the given multi-route term (alternate or
 *                 multiply terms) only allows one route to be accepted. If a
 *                 route is found the state machine will not try to test the
 *                 lower priority routes.
 */
s_enum(TermFlags, ERROR_SYNC_TERM=(1<<16), ONE_ROUTE_TERM=(1<<17));

/**
 * @brief An enumeration used to define symbol flags.
 * @ingroup data
 *
 * ROOT_TOKEN: Specifies that the symbol is a root token that should be checked
 *             by the lexer when it starts a new token. The lexer loops through
 *             all symbols in the lexer module that has this flag set and skips
 *             other symbol definitions.
 */
s_enum(SymbolFlags, ROOT_TOKEN=(1<<16), IGNORED_TOKEN=(1<<17));

/**
 * @brief An enumeration for change operation of Containers' contents.
 * @ingroup data
 *
 * This enumeration is used to specifiy the type of the operation happneing on
 * Containers. This is used by containers' contentChangeNotifier.
 *
 * ADD: The operation is an addition of a new element.
 * UPDATE: The operation is an update of an existing element.
 * REMOVE: The operation is a removal of an existing element.
 */
s_enum(ContentChangeOp, ADD, UPDATE, REMOVE);

/**
 * @brief An enumeration for metadata elements of GrammarModule.
 * @ingroup data
 *
 * This enumeration is used to refer to an element within GrammarModule's
 * meta data. It's needed by GrammarModule::metaChangeNotifier.
 */
s_enum(GrammarModuleMetaElement, START_REF=1,
                                      LEXER_MODULE_REF=2,
                                      ERROR_SYNC_BLOCK_PAIRS_REF=4,
                                      ALL=static_cast<Word>(-1));

/**
 * @brief An enumeration for change operations of SymbolDefinition.
 * @ingroup data
 *
 * This enumration is used to specify the type of the operation happening on
 * SymbolDefinition. This is used by SymbolDefinition::changeNotifier.
 *
 * UPDATE: An element is being updated.
 * DESTROY: The entire Symbol is being destroyed.
 */
s_enum(SymbolDefChangeOp, UPDATE, DESTROY);

/**
 * @brief An enumeration for the different member elements in SymbolDefinition.
 * @ingroup data
 *
 * This enumeration is used to refer to an element within SymbolDefinition.
 * This is needed by SymbolDefinition::changeNotifier as well as the
 * constructors.
 */
s_enum(SymbolDefElement, PARENT_REF=1, TERM=2, VAR_DEFS=4, VARS=8, HANDLER=16, PRIORITY=32, FLAGS=64,
            ATTRIBUTES=128, ALL=static_cast<Word>(-1));

/**
 * @brief An enumeration for the different member elements of Term objects.
 * @ingroup data
 */
s_enum(TermElement, FLAGS=1, REF=2, DATA=4, TERM=8, ESPI=16, PRIORITY=32, MIN=64, MAX=128, ID=256, TEXT=512);


//==============================================================================
// Forward Class Definitions

class Node;
class Module;
class Provider;
class CharGroupUnit;


//==============================================================================
// Functions

/**
 * A helper function to check whether 'op' is any of the perform values.
 * @ingroup data
 */
Bool isPerform(RefOp op);

/**
 * A helper function to check whether 'op' is any of the move values.
 * @ingroup data
 */
Bool isMove(RefOp op);

/**
 * Reset the indexes of all references in a specific range within a tree.
 * @ingroup data
 * Call the unsetIndexes method of DataOwner interface, if implemented by
 * the object.
 */
void unsetIndexes(TiObject *obj, Int from, Int to);

/**
 * @brief Set the IDs of all elements in a given tree.
 * @ingroup data
 * Sets the ID of the given object, and the IDs of any objects contained within
 * the given object if the given object is a container. The ids will be the
 * concatenated container keys that lead to the given object from the root.
 * @sa generateId()
 */
void setTreeIds(TiObject *obj);

/**
 * @brief Set the IDs of all elements in a given tree.
 * @ingroup data
 * Sets the ID of the given object, and the IDs of any objects contained within
 * the given object if the given object is a container. The IDs of inner
 * objects will have the format: <id>.<childName>
 */
void setTreeIds(TiObject *obj, const Char *id);

/**
 * @brief Generate an ID for the given object.
 * @ingroup data
 * This function will generate the ID by tracing the owners of the given object
 * all the way to the root and generate the ID from the concatenated keys that
 * leads to the given object.
 * @param obj The object for which to generate the ID.
 * @param id A string stream to hold the generated ID.
 */
void generateId(Node *obj, StrStream &id);

/**
 * @brief Match a given character to a character group hierarchy.
 * @ingroup data
 * Recursively matches the given character to the given character group. This
 * recursive function will descend into the entire character group tree to
 * match the given character.
 *
 * @param ch The character to match.
 * @param unit A pointer to the character group unit object to match. This
 *             object can be a the head of a tree of CharGroupUnit objects.
 * @return Returns true if the character matches, false otherwise.
 */
Bool matchCharGroup(WChar ch, CharGroupUnit *unit);

/**
 * @brief Print the given object to the given stream.
 * @ingroup data
 *
 * If the object implements the Printable interface, it will pass the call to
 * that interface, otherwise it will print the object type and the production
 * id if available.
 */
void dumpData(OutStream &stream, TiObject *ptr, int indents);

// TODO: Find module for other dimensions.


//==============================================================================
// Function Signature Definitions

typedef std::function<RefOp(Int index, TiObject *&obj)> ReferenceSetLambda;
typedef std::function<RefOp(Int index, TiObject *obj)> ReferenceRemoveLambda;
typedef std::function<RefOp(Int index, TiObject *obj)> ReferenceForeachLambda;

typedef std::function<RefOp(Int index, TiObject *&obj)> SeekerSetLambda;
typedef std::function<RefOp(Int index, TiObject *obj)> SeekerRemoveLambda;
typedef std::function<RefOp(Int index, TiObject *obj, TiObject *parent)> SeekerForeachLambda;


//==============================================================================
// Macros

/**
 * @brief Set the owner of ptr to this if ptr is of type Node.
 * @ingroup data
 */
#define OWN_SHAREDPTR(ptr) \
  { \
    auto __ptr = (ptr).s_cast_get<Node>(); \
    if (__ptr != 0 && __ptr->isDerivedFrom<Node>()) { \
        __ptr->setOwner(this); \
    } \
  }

/**
 * @brief A plain pointer version of OWN_SHAREDPTR.
 * @ingroup data
 * @sa OWN_SHAREDPTR()
 */
#define OWN_PLAINPTR(ptr) \
  { \
    auto __ptr = ptr; \
    if (__ptr != 0 && __ptr->isDerivedFrom<Node>()) { \
        static_cast<Node*>(__ptr)->setOwner(this); \
    } \
  }

/**
 * @brief Reset the owner of ptr if ptr is of type Node.
 * @ingroup data
 */
#define DISOWN_SHAREDPTR(ptr) \
  { \
    auto __ptr = (ptr).s_cast_get<Node>(); \
    if (__ptr != 0 && __ptr->isDerivedFrom<Node>() && __ptr->getOwner() == this) { \
        __ptr->setOwner(0); \
    } \
  }

/**
 * @brief A plain pointer version of DIWOWN_SHAREDPTR.
 * @ingroup data
 * @sa DISOWN_SHAREDPTR()
 */
#define DISOWN_PLAINPTR(ptr) \
  { \
    auto __ptr = static_cast<Node*>(ptr); \
    if (__ptr != 0 && __ptr->isDerivedFrom<Node>() && __ptr->getOwner() == this) { \
        __ptr->setOwner(0); \
    } \
  }

/**
 * @brief A macro to make it easy to set and update owned object pointers.
 * @ingroup data
 * This macro will first reset the owner of the current object before setting
 * the new object and setting the owner of the new object.
 */
#define UPDATE_OWNED_SHAREDPTR(ptr, val) \
  { \
    DISOWN_SHAREDPTR(ptr); \
    (ptr) = val; \
    OWN_SHAREDPTR(ptr); \
  }

/**
 * @brief A plain pointer version of UPDATED_OWNED_SHAREDPTR.
 * @ingroup data
 * @sa UPDATED_OWNED_SHARED_PTR()
 */
#define UPDATE_OWNED_PLAINPTR(ptr, val) \
  { \
    DISOWN_PLAINPTR(ptr); \
    (ptr) = val; \
    OWN_PLAINPTR(ptr); \
  }

/**
 * @brief A macro to simplify resetting a pointer to an owned object.
 * @ingroup data
 * This macro will first reset the owner of the object before resetting the
 * pointer.
 */
#define RESET_OWNED_SHAREDPTR(ptr) \
  {\
    DISOWN_SHAREDPTR(ptr); \
    (ptr).reset(); \
  }

/**
 * @brief A plain pointer version of RESET_OWNED_SHAREDPTR.
 * @ingroup data
 * @sa RESET_OWNED_SHAREDPTR()
 */
#define RESET_OWNED_PLAINPTR(ptr) \
  {\
    DISOWN_PLAINPTR(ptr); \
    (ptr) = 0; \
  }

} } // namespace


//==============================================================================
// Classes

#include "Node.h"

// Basic Data Types
#include "Integer.h"
#include "String.h"

// Generic Data Interfaces
#include "DataOwner.h"
#include "AttributesHolder.h"
#include "Initializable.h"
#include "IdHolder.h"
#include "Clonable.h"
#include "Printable.h"

// Helpers
#include "paired_pointers.h"
#include "IdGenerator.h"
#include "SourceLocation.h"

// References Subsystem Classes
//-----------------------------
// Interfaces
#include "Container.h"
#include "ListContainer.h"
#include "NamedListContainer.h"
#include "MapContainer.h"
#include "Provider.h"
// Validators
#include "Validator.h"
#include "StrAttributeValidator.h"
// References
#include "Reference.h"
#include "IndexReference.h"
#include "StrKeyReference.h"
#include "IndirectReference.h"
#include "RawIndirectReference.h"
#include "ScopeReference.h"
#include "SearchReference.h"
#include "SelfReference.h"
#include "ReferenceParser.h"
// Seekers and Data Providers
#include "ReferenceSeeker.h"
#include "QualifierSeeker.h"

// Generic Containers
//-------------------
#include "SharedMap.h"
#include "SharedList.h"
#include "SharedNamedList.h"
#include "PlainNamedList.h"
#include "Module.h"
#include "VariableStack.h"
#include "SharedRepository.h"
#include "PlainRepository.h"

// Grammar Classes
//----------------
#include "Token.h"
// Character Groups
#include "CharGroupUnit.h"
#include "SequenceCharGroupUnit.h"
#include "RandomCharGroupUnit.h"
#include "UnionCharGroupUnit.h"
#include "InvertCharGroupUnit.h"
#include "CharGroupDefinition.h"
// Terms
#include "Term.h"
#include "ConstTerm.h"
#include "CharGroupTerm.h"
#include "ListTerm.h"
#include "ConcatTerm.h"
#include "AlternateTerm.h"
#include "MultiplyTerm.h"
#include "ReferenceTerm.h"
#include "TokenTerm.h"
// Other Grammar Classes
#include "OperationHandler.h"
#include "SymbolDefinition.h"
// TODO: #include "SymbolGroup.h"
#include "GrammarModule.h"
#include "Tracer.h"
#include "GrammarRepository.h"
#include "GrammarContext.h"
#include "GrammarPlant.h"
// TODO: #include "ExtensionManager.h"
// TODO: ExtensionManager manages extensions to grammar (loaded using import for example). And it automatically
//       manages the IDs of those extension definitions (rather than the IDs remain invalid until merged with DataStore).


// Abstract Syntax Tree Classes
//-----------------------------

namespace Core { namespace Data { namespace Ast
{

ti_s_enum(BracketType, Integer, "Core.Data.Ast", "Core", "alusus.net", ROUND, SQUARE);

} } } // namespace

#include "Ast/MetadataHolder.h"
#include "Ast/Token.h"
#include "Ast/Route.h"
#include "Ast/List.h"
#include "Ast/ExpressionList.h"
#include "Ast/StatementList.h"
#include "Ast/Module.h"
#include "Ast/ParamPass.h"
#include "Ast/InfixOperator.h"
#include "Ast/OutfixOperator.h"
#include "Ast/Text.h"
#include "Ast/Bracket.h"

namespace Core { namespace Data { namespace Ast
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

} } } // namespace

#endif
