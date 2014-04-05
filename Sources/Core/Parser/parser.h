/**
 * @file Core/Parser/parser.h
 * Contains the definitions and include statements for all types used by the
 * parser.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

namespace Core { namespace Parser
{

/**
 * @defgroup parser Parser
 * @ingroup core
 */

/**
 * @brief An enumeration used to define term type identifiers.
 * @ingroup parser
 */
enumeration(TermType, TOKEN, CONCAT, ALTERNATIVE, OPTIONAL, DUPLICATE, REFERENCE);

/**
 * @brief The temporary processing status of the state object.
 * @ingroup parser
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
enumeration(ProcessingStatus, IN_PROGRESS = 0, COMPLETE, ERROR);

/**
 * @brief The cause of termination for a given state.
 * @ingroup parser
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
enumeration(StateTerminationCause, UNKNOWN = 0, SYNTAX_ERROR, MERGED_WITH_HIGHER_PRIORITY_STATE,
                                   CONSUMED_TOKENS_TO_LIVE, FOLDED_OUT_TOO_SOON, NOT_NEEDED_ANYMORE);

/**
 * @brief The state level index flag for this parsing pass.
 * @ingroup parser
 *
 * This flag is set in the state level's index to specify that the route has
 * been visited during this parsing pass.
 */
#define THIS_PARSING_PASS 0x10000000

/**
 * @brief The state level index flag for this testing pass.
 * @ingroup parser
 *
 * This flag is set in the state level's index to specify that the route has
 * been visited during this testing pass, i.e. the operation that determines
 * whether a specific route can be taken during this parsing pass.
 */
#define THIS_TESTING_PASS 0x20000000

/**
 * @brief The default value for tokensToLive.
 * @ingroup parser
 *
 * When parsing branches, the branch with the lower priority is given a number
 * of tokens to live before it's forced to die, if none of the two branches
 * died naturally before the end of the end of tokensToLive. The value of
 * tokensToLive should come from the grammar, but if it's not provided by
 * the grammar, this default value will be considered instead.
 */
#define DEFAULT_TOKENS_TO_LIVE 20

/**
 * @brief The number of preallocated state term levels.
 * @ingroup parser
 *
 * These state term levels are preallocated when the state is instantiated. It's
 * done for performance purposes.
 */
#define RESERVED_TERM_LEVEL_COUNT  1000

/**
 * @brief The number of preallocated state production levels.
 * @ingroup parser
 *
 * These state production levels are preallocated when the state is instantiated.
 * It's done for performance purposes.
 */
#define RESERVED_PRODUCTION_LEVEL_COUNT 100

/**
 * @brief The number of preallocated state variable levels.
 * @ingroup parser
 *
 * These state variable levels are preallocated when the state is instantiated.
 * It's done for performance purposes.
 */
#define RESERVED_VARIABLE_LEVEL_COUNT 100

/**
 * @brief The number of preallocated variable stack records.
 * @ingroup parser
 *
 * This value is used for state variable stacks.
 */
#define RESERVED_VARIABLE_COUNT 1000

/**
 * @brief The maximum number of characters allowed for variable names.
 * @ingroup parser
 *
 * This value is used for state variable stacks.
 */
#define VARIABLE_NAME_MAX_LENGTH 256

} } // namespace


//==============================================================================
// Classes

#include "SyntaxErrorMsg.h"
#include "UnexpectedEofMsg.h"
#include "UnexpectedTokenMsg.h"
#include "AmbiguityMsg.h"

#include "TermLevel.h"
#include "ProdLevel.h"

#include "State.h"

#include "ParsingHandler.h"

#include "StateMachine.h"

#endif
