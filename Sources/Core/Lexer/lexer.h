/**
 * @file Core/Lexer/lexer.h
 * Contains the definitions and include statements for all types used by the
 * lexer.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_LEXER_H
#define LEXER_LEXER_H

namespace Core { namespace Lexer
{

/**
 * @defgroup lexer Lexer
 * @ingroup core
 */

/**
 * @brief The base value for ignored token ids.
 * @ingroup lexer
 *
 * Ignored tokens (tokens used for spaces and comments) have their ids equal or
 * greater than this value.
 */
#define IGNORED_TOKEN_BASE	0x1000000

/**
 * @brief The maximum number of characters in the input buffer.
 * @ingroup lexer
 *
 * The number of characters in the characters buffer of InputBuffer will not
 * exceed this number.
 */
#define INPUT_BUFFER_MAX_CHARACTERS	16000

/**
 * @brief The maximum number of character groups in the input buffer.
 * @ingroup lexer
 *
 * The number of characters groups in InputBuffer will not exceed this number.
 */
#define INPUT_BUFFER_MAX_GROUPS	64

/**
 * @brief The maximum number of entries in the states array.
 * @ingroup lexer
 */
#define STATES_ARRAY_MAX_SIZE	128

/**
 * @brief The maximum number of characters in the error buffer.
 * @ingroup lexer
 *
 * If the error text is larger than this value, it will be clipped to this
 * number of characters.
 */
#define ERROR_BUFFER_MAX_CHARACTERS 80

/**
 * @brief An enumeration used to define char group unit type identifiers.
 * @ingroup lexer
 */
enumeration(CharGroupUnitType, SEQUENCE, RANDOM, UNION, INVERT);

/**
 * @brief An enumeration used to define term type identifiers.
 * @ingroup lexer
 */
enumeration(TermType, CONST, CHAR_GROUP, CONCAT, ALTERNATIVE, OPTIONAL, DUPLICATE, REFERENCE);

} } // namespace


//==============================================================================
// Classes

#include "CharGroupUnit.h"
#include "SequenceCharGroupUnit.h"
#include "RandomCharGroupUnit.h"
#include "UnionCharGroupUnit.h"
#include "InvertCharGroupUnit.h"

#include "CharGroupDefinition.h"
#include "CharGroupDefinitionList.h"

#include "Term.h"
#include "ListTerm.h"
#include "ConstTerm.h"
#include "CharGroupTerm.h"
#include "ConcatTerm.h"
#include "AlternativeTerm.h"
#include "OptionalTerm.h"
#include "DuplicateTerm.h"
#include "ReferenceTerm.h"

#include "TokenDefinition.h"
#include "TokenDefinitionList.h"

#include "BufferFullMsg.h"
#include "TokenClampedMsg.h"
#include "UnrecognizedCharMsg.h"

#include "InputBuffer.h"
#include "State.h"
#include "StateMachine.h"

#endif
