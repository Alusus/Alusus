/**
 * @file Core/Data/Grammar/grammar.h
 * Contains the definitions and include statements of all types in the Data::Grammar
 * namespace.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_GRAMMAR_H
#define CORE_DATA_GRAMMAR_GRAMMAR_H

/**
 * @defgroup core_data_grammar Grammar
 * @ingroup core_data
 * @brief Classes related to the grammar definitions.
 */

//==============================================================================
// Forward Class Declaration

namespace Core::Data::Grammar
{
  class Reference;
  class Module;
  class CharGroupUnit;
}

namespace Core::Processing::Handlers
{
  class ConstTokenizingHandler;
  class StringLiteralTokenizingHandler;
  class GenericParsingHandler;
  class ImportParsingHandler;
  class DumpAstParsingHandler;
  class ModifierParsingHandler;
  class GenericCommandParsingHandler;
}

namespace Core::Main
{
  class RootManager;
}


//==============================================================================
// Data Types

namespace Core::Data::Grammar
{

/**
 * @brief An enumeration used to define term flags.
 * @ingroup core_data
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
 * @ingroup core_data
 *
 * ROOT_TOKEN: Specifies that the symbol is a root token that should be checked
 *             by the lexer when it starts a new token. The lexer loops through
 *             all symbols in the lexer module that has this flag set and skips
 *             other symbol definitions.
 */
s_enum(SymbolFlags, ROOT_TOKEN=(1<<16), IGNORED_TOKEN=(1<<17));

} // namespace


//==============================================================================
// Global Functions

namespace Core::Data::Grammar
{

SharedPtr<Reference> createReference(Char const *qualifier, std::vector<SharedPtr<Reference>> *referenceCache = 0);

Module* getGrammarRoot(ListContaining<TiObject> *rootScope, Bool createIfMissing = false);

/**
 * @brief Match a given character to a character group hierarchy.
 * @ingroup core_data
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

} // namespace


//==============================================================================
// Macros

#define PARSE_REF(qualifier) Core::Data::Grammar::createReference(qualifier)


//==============================================================================
// Classes

#include "Reference.h"

// Containers
#include "List.h"
#include "Map.h"
#include "Module.h"

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

// Other Grammar Definitions
#include "BuildHandler.h"
#include "SymbolDefinition.h"
// TODO: #include "SymbolGroup.h"
#include "ParsingDimension.h"

// Grammar Helpers
#include "Context.h"
#include "Factory.h"
#include "StandardFactory.h"

#endif
