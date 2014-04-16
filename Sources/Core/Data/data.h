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
 * @brief An enumeration used to define reference scopes.
 * @ingroup data
 *
 * The scope of the reference specifies whether the reference is within the
 * argument list, variable stack, current lexer module module, current parser
 * module, parent parser module, or the root module. For performance reasons,
 * this enum is used by the Reference class to set flags within the Reference's
 * index instead of using an entire Reference object to specify the scope of the
 * reference.
 */
enumeration(ReferenceScope, UNKNOWN, ARGS, STACK, MODULE, PMODULE, ROOT);

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
 * @brief An enumeration for change operation of Containers' contents.
 * @ingroup data
 *
 * This enumeration is used to specifiy the type of the operation happneing on
 * Containers. This is used by Container::contentChangeNotifier.
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
// Functions

/// Reset the indexes of all references in a specific range within a tree.
void unsetIndexes(IdentifiableObject *obj, Int from, Int to);

} } // namespace


//==============================================================================
// Classes

#include "IdGenerator.h"

// Interfaces
#include "DataOwner.h"
#include "IdOwner.h"
#include "Initializable.h"
#include "Container.h"
#include "ListContainer.h"
#include "MapContainer.h"
#include "PlainContainer.h"
#include "ListPlainContainer.h"
#include "MapPlainContainer.h"
#include "Provider.h"

// Data Types
#include "Integer.h"
#include "String.h"
#include "List.h"
#include "Map.h"
#include "Module.h"
#include "VariableStack.h"

// References
#include "ReferenceSegment.h"
#include "IntReferenceSegment.h"
#include "StrReferenceSegment.h"
#include "IndirectReferenceSegment.h"
#include "RawIndirectReferenceSegment.h"
#include "Reference.h"
#include "ReferenceParser.h"

// Servers and Data Holders
#include "Server.h"
#include "RawServer.h"
#include "DataStore.h"
#include "DataStack.h"

// Grammar Terms
#include "Term.h"
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
#include "GrammarPlant.h"

#include "ParsingGrammarContext.h"
#include "GrammarHelper.h"

// TODO: #include "ExtensionManager.h"
// TODO: ExtensionManager manages extensions to grammar (loaded using import for example). And it automatically
//       manages the IDs of those extension definitions (rather than the IDs remain invalid until merged with DataStore).

#endif
