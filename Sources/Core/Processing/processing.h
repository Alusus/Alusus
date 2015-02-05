/**
 * @file Core/Processing/processing.h
 * Contains the definitions and include statements for all types used for
 * processing.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_PROCESSING_H
#define PROCESSING_PROCESSING_H

namespace Core { namespace Processing
{

/**
 * @defgroup processing Processing
 * @ingroup core
 */

/**
 * @defgroup processing_lexer Lexer
 * @ingroup processing
 */

/**
 * @defgroup processing_parser Parser
 * @ingroup processing
 */

/**
 * @brief The state level index flag for this processing pass.
 * @ingroup processing
 *
 * This flag is set in the state level's index to specify that the route has
 * been visited during this parsing pass.
 */
#define THIS_PROCESSING_PASS 0x10000000

/**
 * @brief The state level index flag for this testing pass.
 * @ingroup processing
 *
 * This flag is set in the state level's index to specify that the route has
 * been visited during this testing pass, i.e. the operation that determines
 * whether a specific route can be taken during this parsing pass.
 */
#define THIS_TESTING_PASS 0x20000000

/**
 * @brief The number of preallocated parser state variable levels.
 * @ingroup processing
 *
 * These state variable levels are preallocated when the state is instantiated.
 * It's done for performance purposes.
 */
#define RESERVED_VARIABLE_LEVEL_COUNT 100

/**
 * @brief The number of parser's preallocated variable stack records.
 * @ingroup processing
 *
 * This value is used for state variable stacks.
 */
#define RESERVED_VARIABLE_COUNT 1000

/**
 * @brief The maximum number of characters allowed for variable names.
 * @ingroup processing
 *
 * This value is used for state variable stacks.
 */
#define VARIABLE_NAME_MAX_LENGTH 256


//==============================================================================
// Lexer Definitions

/**
 * @brief The maximum number of characters in the input buffer.
 * @ingroup processing_lexer
 *
 * The number of characters in the characters buffer of InputBuffer will not
 * exceed this number.
 */
#define INPUT_BUFFER_MAX_CHARACTERS	16000

/**
 * @brief The maximum number of character groups in the input buffer.
 * @ingroup processing_lexer
 *
 * The number of characters groups in InputBuffer will not exceed this number.
 */
#define INPUT_BUFFER_MAX_GROUPS	64

/**
 * @brief The maximum number of entries in the states array.
 * @ingroup processing_lexer
 */
#define LEXER_STATES_ARRAY_MAX_SIZE	128

/**
 * @brief The maximum number of characters in the error buffer.
 * @ingroup processing_lexer
 *
 * If the error text is larger than this value, it will be clipped to this
 * number of characters.
 */
#define LEXER_ERROR_BUFFER_MAX_CHARACTERS 80

/**
 * @brief Compute the next position based on the given character.
 * @ingroup processing_lexer
 *
 * Update a given line number and column values based on the value of the given
 * char. The character is checked for a new line or carriage return values to
 * determine whether the next position is at the beginning of a new line or
 * at the beginning of the same line respectively. If the character value is
 * anything other than new line or carriage return, the next position is simply
 * the following column on the same line.
 *
 * @param ch The character used to determine the next position.
 * @param line A reference to the value of the current line number. This
 *             value will be replaced with the new line number value.
 * @param column A reference to the value of the current column. This value
 *               will be replaced with the new column value.
 */
void computeNextCharPosition(WChar ch, Int &line, Int &column);


//==============================================================================
// Parser Definitions

/**
 * @brief The default value for tokensToLive.
 * @ingroup processing_parser
 *
 * When parsing branches, the branch with the lower priority is given a number
 * of tokens to live before it's forced to die, if none of the two branches
 * died naturally before the end of the end of tokensToLive. The value of
 * tokensToLive should come from the grammar, but if it's not provided by
 * the grammar, this default value will be considered instead.
 */
#define DEFAULT_TOKENS_TO_LIVE 20

/**
 * @brief The number of preallocated parser state term levels.
 * @ingroup processing_parser
 *
 * These state term levels are preallocated when the state is instantiated. It's
 * done for performance purposes.
 */
#define RESERVED_PARSER_TERM_LEVEL_COUNT  1000

/**
 * @brief The number of preallocated parser state production levels.
 * @ingroup processing_parser
 *
 * These state production levels are preallocated when the state is instantiated.
 * It's done for performance purposes.
 */
#define RESERVED_PARSER_PRODUCTION_LEVEL_COUNT 100

/**
 * @brief The temporary processing status of the state object.
 * @ingroup processing_parser
 *
 * Before the processing of a new token, the processing status of all current
 * states gets reset to IN_PROGRESS, which indicates that these states are
 * waiting to be processed using the new token. Then during the processing of
 * the token the state objects get updated by the recursive processState
 * function, which will accordingly change the processing status in order to
 * know how to deal with the state object when the execution goes up in the
 * recursion stack, or when the state object is revisited again during the
 * processing of the same token.<br>
 *
 * The following values are possible:<br>
 * IN_PROGRESS : Indicates that the state is waiting to be processed, or still
 *               needs more processing (went out of a branch but still didn't
 *               use the token).<br>
 * COMPLETE : The update is complete and the state is waiting for the next
 *            token.<br>
 * ERROR : The given token caused the state to go into a syntax error.
 */
enumeration(ParserProcessingStatus, IN_PROGRESS = 0, COMPLETE, ERROR);

/**
 * @brief The cause of termination for a given state.
 * @ingroup processing_parser
 *
 * The meaning for those values are as follows:<br>
 * SYNTAX_ERROR: Obviously, parsing couldn't find a matching path.<br>
 * MERGED_WITH_HIGHER_PRIORITY_STATE: The deleted state has merged with another
 *                                    state that is of a higher priority.<br>
 * CONSUMED_TOKENS_TO_LIVE: The tokens-to-live counter of the deleted state has
 *                          reached 0 so we'll delete this and choose the branch
 *                          that is of a higher priority.<br>
 * FOLDED_OUT_TOO_SOON: The deleted state has folded out of the parsing tree
 *                      while other successful and higher priority states are
 *                      still in processing.<br>
 * NOT_NEEDED_ANYMORE: A highest priority state has folded out of the parsing
 *                     tree, so we'll consider parsing complete and drop this
 *                     lower priority state.
 */
enumeration(ParserStateTerminationCause, UNKNOWN = 0, SYNTAX_ERROR, MERGED_WITH_HIGHER_PRIORITY_STATE,
                                         CONSUMED_TOKENS_TO_LIVE, FOLDED_OUT_TOO_SOON, NOT_NEEDED_ANYMORE);

/**
 * @brief A set of parsing flags to use with grammar terms.
 * @ingroup processing_parser
 *
 * OMISSIBLE: Specifies that the parsed data for this term can be omitted if the
 *            data has no info to provide. For token terms, this is true if the
 *            token is constants. For other terms, it's true if the child term
 *            returned null contents. For route terms this will only be
 *            effective if the PASS_UP flag is also provided. For list terms
 *            (duplicate and concat) omission happens to individual items of the
 *            list if no data were received from the parent. Productions can be
 *            omitted if it has only one item which is a data of a referenced
 *            production.<br>
 * PASS_UP: This flag specifies that this term is to pass its data up to the
 *          parent. Typically, the parent's element will be a list type element
 *          so the data will be added to that list. This flag applies to route
 *          as well as list term types.<br>
 * FORCE_LIST: For list items (concat and duplicate). This flag will force the
 *             creation of a list item even if there is only one item in the
 *             list. This flag conflicts with PASS_UP.
 */
enumeration(ParsingFlags,
            OMISSIBLE = 1,
            FORCE_LIST = 2,
            PASS_UP = 4);

} } // namespace


//==============================================================================
// Classes

// Build Messages
#include "BuildMsg.h"
#include "CustomBuildMsg.h"
#include "UnrecognizedErrorMsg.h"

// Lexer
// Lexer Specific Build Messages
#include "BufferFullMsg.h"
#include "TokenClampedMsg.h"
#include "UnrecognizedCharMsg.h"
// Tokenizing Classes
#include "InputBuffer.h"
#include "LexerState.h"
#include "TokenizingHandler.h"
#include "Lexer.h"

// Parser
// Parser Specific Build Messages
#include "SyntaxErrorMsg.h"
#include "UnexpectedEofMsg.h"
#include "UnexpectedTokenMsg.h"
#include "AmbiguityMsg.h"
// Parsing Classes
#include "ParserTermLevel.h"
#include "ParserProdLevel.h"
#include "ParserState.h"
#include "ParsingHandler.h"
#include "Parser.h"
// Parsing Handlers
#include "GenericParsingHandler.h"
#include "RootParsingHandler.h"

// Main Class
#include "Engine.h"

#endif
