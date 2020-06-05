/**
 * @file Core/Processing/processing.h
 * Contains the definitions and include statements for all types used for
 * processing.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_PROCESSING_H
#define CORE_PROCESSING_PROCESSING_H

namespace Core::Processing
{

/**
 * @defgroup core_processing Processing
 * @ingroup core
 */

/**
 * @brief The state level index flag for this processing pass.
 * @ingroup core_processing
 *
 * This flag is set in the state level's index to specify that the route has
 * been visited during this parsing pass.
 */
#define THIS_PROCESSING_PASS 0x10000000

/**
 * @brief The state level index flag for this testing pass.
 * @ingroup core_processing
 *
 * This flag is set in the state level's index to specify that the route has
 * been visited during this testing pass, i.e. the operation that determines
 * whether a specific route can be taken during this parsing pass.
 */
#define THIS_TESTING_PASS 0x20000000


//==============================================================================
// Lexer Definitions

/**
 * @brief The maximum number of characters in the input buffer.
 * @ingroup core_processing
 *
 * The number of characters in the characters buffer of InputBuffer will not
 * exceed this number.
 */
#define INPUT_BUFFER_MAX_CHARACTERS	16000

/**
 * @brief The maximum number of character groups in the input buffer.
 * @ingroup core_processing
 *
 * The number of characters groups in InputBuffer will not exceed this number.
 */
#define INPUT_BUFFER_MAX_GROUPS	64

/**
 * @brief The maximum number of entries in the states array.
 * @ingroup core_processing
 */
#define LEXER_STATES_ARRAY_MAX_SIZE	64

/**
 * @brief The maximum size of the lexer state levels stack.
 * @ingroup core_processing
 */
#define LEXER_STATE_LEVEL_MAX_COUNT 64

/**
 * @brief The maximum number of characters in the error buffer.
 * @ingroup core_processing
 *
 * If the error text is larger than this value, it will be clipped to this
 * number of characters.
 */
#define LEXER_ERROR_BUFFER_MAX_CHARACTERS 80

/**
 * @brief Compute the next position based on the given character.
 * @ingroup core_processing
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
 * @ingroup core_processing
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
 * @ingroup core_processing
 *
 * These state term levels are preallocated when the state is instantiated. It's
 * done for performance purposes.
 */
#define RESERVED_PARSER_TERM_LEVEL_COUNT  1000

/**
 * @brief The number of preallocated parser state production levels.
 * @ingroup core_processing
 *
 * These state production levels are preallocated when the state is instantiated.
 * It's done for performance purposes.
 */
#define RESERVED_PARSER_PRODUCTION_LEVEL_COUNT 100

/**
 * @brief The temporary processing status of the state object.
 * @ingroup core_processing
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
s_enum(ParserProcessingStatus, IN_PROGRESS = 0, COMPLETE, ERROR);

/**
 * @brief The cause of termination for a given state.
 * @ingroup core_processing
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
s_enum(ParserStateTerminationCause, UNKNOWN = 0, SYNTAX_ERROR, MERGED_WITH_HIGHER_PRIORITY_STATE,
                                         CONSUMED_TOKENS_TO_LIVE, FOLDED_OUT_TOO_SOON, NOT_NEEDED_ANYMORE);

/**
 * @brief A set of parsing flags to use with grammar terms by parsing handlers.
 * @ingroup core_processing
 *
 * ENFORCE_LIST_OBJ: For list items (concat and duplicate). This flag will force
 *                   the creation of a list item even if there is only one item
 *                   in the list or even if the list is empty. This flag
 *                   conflicts with PASS_UP.<br>
 * ENFORCE_LIST_ITEM: For list items (concat and duplicate). This flag will
 *                    force the insertion of an item in to the list even if the
 *                    item is null.<br>
 * ENFORCE_ROUTE_OBJ: For routes (alternate and optional). This flag will force
 *                    the creation of a route item to capture the index of the
 *                    selected route. Without this flag selected route indexes
 *                    will be ignored.<br>
 * ENFORCE_PROD_OBJ: For productions. This item enforces the creation of a prod
 *                   object even if it just includes a single child prod obj.<br>
 * ENFORCE_TOKEN_OBJ: Used to enforce the creation of a token object even if the
 *                    matched token is a const token.
 * ENFORCE_TOKEN_OMIT: Used with token terms to specify that the term should be
 *                     omitted even if the omission will lead to loss of info.
 *                     For example, if a token term accepts one of multiple
 *                     constant tokens then omitting the data will lead to loss
 *                     of knowledge about which one of the constant tokens was
 *                     received. With this flag the parsing handler should omit
 *                     the data even if such loss of knowledge happens.
 * PASS_ITEMS_UP: For list items (concat and duplicate). This flag specifies
 *                that this term is to pass its data up to the parent.
 *                Typically, the parent's element will be a list type element so
 *                the data will be added to that list instead of to a child list.
 */
s_enum(ParsingFlags,
  ENFORCE_LIST_OBJ = 1,
  ENFORCE_LIST_ITEM = 2,
  ENFORCE_ROUTE_OBJ = 4,
  ENFORCE_PROD_OBJ = 8,
  ENFORCE_TOKEN_OBJ = 16,
  ENFORCE_TOKEN_OMIT = 32,
  PASS_ITEMS_UP = 64
);

} // namespace


//==============================================================================
// Classes

// Lexer
#include "InputBuffer.h"
#include "LexerState.h"
#include "TokenizingHandler.h"
#include "Lexer.h"

// Parser
#include "ParserTermLevel.h"
#include "ParserProdLevel.h"
#include "ParserModifierLevel.h"
#include "ParserState.h"
#include "ParsingHandler.h"
#include "Parser.h"

// Streams
#include "CharInStreaming.h"
#include "StdCharInStream.h"
#include "InteractiveCharInStream.h"

// Main Class
#include "Engine.h"

// Parsing Handlers
#include "Handlers/handlers.h"


//==============================================================================
// Type Names

DEFINE_TYPE_NAME(Core::Processing::ParserState, "alusus.org/Core/Core.Processing.ParserState");

#endif
