/**
 * @file Core/Data/data.h
 * Contains the definitions and include statements of all types in the Data
 * namespace.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_DATA_H
#define DATA_DATA_H

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


//==============================================================================
// Data Types

/**
 * @brief Specifies the repository index of each grammar scope.
 * Each of the scopes will have a fixed index within the repository.
 */
enumeration(GrammarScopeIndex, ROOT=0, MODULE=1, PMODULE=2, STACK=3, ARGS=4);

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
enumeration(ReferenceUsageCriteria, SINGLE_DATA_SINGLE_MATCH, SINGLE_DATA_MULTI_MATCH, MULTI_DATA);

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
enumeration(TermFlags, ERROR_SYNC_TERM=(2<<16), ONE_ROUTE_TERM=(2<<17));

/**
 * @brief An enumeration used to define symbol flags.
 * @ingroup data
 *
 * ROOT_TOKEN: Specifies that the symbol is a root token that should be checked
 *             by the lexer when it starts a new token. The lexer loops through
 *             all symbols in the lexer module that has this flag set and skips
 *             other symbol definitions.
 */
enumeration(SymbolFlags, ROOT_TOKEN=(2<<16), IGNORED_TOKEN=(2<<17));

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
enumeration(ContentChangeOp, ADD, UPDATE, REMOVE);

/**
 * @brief An enumeration for metadata elements of GrammarModule.
 * @ingroup data
 *
 * This enumeration is used to refer to an element within GrammarModule's
 * meta data. It's needed by GrammarModule::metaChangeNotifier.
 */
enumeration(GrammarModuleMetaElement, START_REF=1, LEXER_MODULE_REF=2, ALL=static_cast<Word>(-1));

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
enumeration(SymbolDefChangeOp, UPDATE, DESTROY);

/**
 * @brief An enumeration for the different member elements in SymbolDefinition.
 * @ingroup data
 *
 * This enumeration is used to refer to an element within SymbolDefinition.
 * This is needed by SymbolDefinition::changeNotifier as well as the
 * constructors.
 */
enumeration(SymbolDefElement, PARENT_REF=1, TERM=2, VAR_DEFS=4, VARS=8, HANDLER=16, PRIORITY=32, FLAGS=64,
            ATTRIBUTES=128, ALL=static_cast<Word>(-1));

/**
 * @brief An enumeration for the different member elements of Term objects.
 * @ingroup data
 */
enumeration(TermElement, FLAGS=1, REF=2, DATA=4, TERM=8, ESPI=16, PRIORITY=32, MIN=64, MAX=128, ID=256, TEXT=512);


//==============================================================================
// Forward Class Definitions

class Module;
class Provider;
class PlainProvider;
class CharGroupUnit;


//==============================================================================
// Functions

/// Reset the indexes of all references in a specific range within a tree.
void unsetIndexes(IdentifiableObject *obj, Int from, Int to);

/**
 * @brief Set the IDs of all elements in a given tree.
 * Sets the ID of the given object, and the IDs of any objects contained within
 * the given object if the given object is a container. The IDs of inner
 * objects will have the format: <id>.<childName>
 */
void setTreeIds(IdentifiableObject *obj, const Char *id);

/**
 * @brief Match a given character to a character group hierarchy.
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
 * @brief Recursive function to print a tree of parsed data.
 * @ingroup main
 *
 * The given tree should have default parsing data (ParsedRoute,
 * ParsedList, or ParsedToken). Anything other than the
 * default data types will be represented by [UNKNOWN TYPE].
 */
void dumpParsedData(IdentifiableObject *ptr, int indents=0, Bool start_indent=true);

// TODO: Find module for other dimensions.

} } // namespace


//==============================================================================
// Classes

// Helpers
#include "module_paired_pointers.h"
#include "IdGenerator.h"
#include "SourceLocation.h"

// Data Classes
//-------------
// Simple Data
#include "Integer.h"
#include "String.h"
// Generic Data Interfaces
#include "DataOwner.h"
#include "AttributesHolder.h"
#include "IdHolder.h"
#include "Initializable.h"
// Container Interfaces
#include "Container.h"
#include "ListContainer.h"
#include "NamedListContainer.h"
#include "MapContainer.h"
// Complex Data
#include "SharedMap.h"
#include "SharedList.h"
#include "SharedNamedList.h"
#include "PlainNamedList.h"
#include "Module.h"
#include "VariableStack.h"
#include "Token.h"

// Data Provider Classes
//----------------------
// Interfaces
#include "Provider.h"
#include "Tracer.h"
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
#include "SharedRepository.h"
#include "PlainRepository.h"

// Grammar Classes
//----------------
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
#include "GrammarRepository.h"
#include "GrammarContext.h"
#include "GrammarPlant.h"
// TODO: #include "ExtensionManager.h"
// TODO: ExtensionManager manages extensions to grammar (loaded using import for example). And it automatically
//       manages the IDs of those extension definitions (rather than the IDs remain invalid until merged with DataStore).

// Parsed Data Classes
//--------------------
#include "ParsingMetadataHolder.h"
#include "ParsedToken.h"
#include "ParsedRoute.h"
#include "ParsedList.h"
#include "ParsedModule.h"

#endif
