/**
 * @file Core/Processing/ParsingHandler.h
 * Contains the header of class Core::Processing::ParsingHandler.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_PARSINGHANDLER_H
#define CORE_PROCESSING_PARSINGHANDLER_H

namespace Core { namespace Processing
{

class Parser;

/**
 * @brief The root class of all parsing handlers.
 * @ingroup processing_parser
 *
 * Parsing handlers are the classes responsible for handling different parsing
 * events. Each production can be associated with its own parsing handler that
 * can handle the parsing events according to the needs of that production. The
 * parsing handler is the bridge between the code generator and the parser. Code
 * generators can also include their own parsing handlers that they associate
 * with the desired production.
 */
class ParsingHandler : public Data::OperationHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(ParsingHandler, Data::OperationHandler, "Core.Parser", "Core", "alusus.net");


  //============================================================================
  // Protected Constructor

  protected: ParsingHandler()
  {
  }


  //============================================================================
  // Virtual Functions

  /**
   * @brief Called when the parsing enters a new production.
   *
   * This event is raised by the state machine when parsing enters a new
   * production (after the production's state level is entered).
   *
   * @param machine The parser state machine that fired the event.
   * @param state The state object that entered the given production.
   */
  public: virtual void onProdStart(Parser *parser, ParserState *state)
  {
  }

  /**
   * @brief Called when the parsing exists a production.
   *
   * This event is raised by the state machine when parsing is about to exit
   * a production (before the production's state level is exit).
   *
   * @param machine The parser state machine that fired the event.
   * @param state The state object that exited the given production.
   */
  public: virtual void onProdEnd(Parser *parser, ParserState *state)
  {
  }

  /**
   * @brief Called when parsing enters a new term level.
   *
   * This event is raised by the state machine when parsing enters a new
   * term level (after the new term level is created).
   *
   * @param machine The parser state machine that fired the event.
   * @param state The subject state.
   */
  public: virtual void onTermStart(Parser *parser, ParserState *state)
  {
  }

  /**
   * @brief Called when parsing exits a term level.
   *
   * This event is raised by the state machine when parsing is about to exit
   * a term level (before the term level is deleted).
   *
   * @param machine The parser state machine that fired the event.
   * @param state The subject state.
   */
  public: virtual void onTermEnd(Parser *parser, ParserState *state)
  {
  }

  /**
   * @brief Called after parsing exists a state level.
   *
   * This event is raised by the state machine after a state level is deleted.
   * This is called whether the deleted level is a production level or a
   * term level.
   *
   * @param machine The parser state machine that fired the event.
   * @param state The subject state.
   * @param data A smart pointer to the data of the level that has just been
   *             deleted. At this point, if this data isn't shared with any
   *             other state/user, the data will have a users count of 0 and
   *             will be referenced by only the local smart pointer of the
   *             caller, so if the parsing handler didn't retain a smart
   *             pointer to this data it will end up being deleted once this
   *             call is complete.
   */
  public: virtual void onLevelExit(Parser *parser, ParserState *state, SharedPtr<TiObject> const &data)
  {
  }

  /**
   * @brief Called when a new successful token is received.
   *
   * This event is raised by the state machine when a new token is received
   * and the token matches the grammar. The state machine doesn't do anything
   * with received tokens and it's completely the responsibility of the
   * parsing handler how to process that token.
   *
   * @param machine The parser state machine that fired the event.
   * @param state The subject state.
   * @param token A pointer to the received token.
   */
  public: virtual void onNewToken(Parser *parser, ParserState *state, const Data::Token *token)
  {
  }

  /**
   * @brief Called when a new error token is received.
   *
   * This event is raised by the state machine when a new token is received
   * and the token couldn't make any match with the grammar at the position
   * of the given state. If the token matches one state and mismatches
   * another, the matching state will receive a newToken event while the
   * mismatching state will receive an error_token event.
   *
   * @note This event is received even for states that are to be cancelled due
   *       to this error. In other words, even if we have existing successful
   *       states, the error states will receive this event.
   *
   * @param machine The parser state machine that fired the event.
   * @param state The subject state.
   * @param token A pointer to the received token. If this value is 0 it means
   *              parsing has ended without satisfying the grammar (more
   *              tokens are still needed).
   *
   * @return Returns true if this handler raised a build msg for this error.
   *         If false is returned, the state machine will raise a syntax error
   *         for this token. If true is returned, the state machine will not
   *         raise any msg. The parsing handler should only return true if
   *         it wants to raise a more detailed build msg than the generic
   *         syntax error generated by the state machine.
   */
  public: virtual Bool onErrorToken(Parser *parser, ParserState *state, const Data::Token *token)
  {
    return false;
  }

  /**
   * @brief Called when a route decision is made on an alternative term.
   *
   * This event is raised by the state machine when a routing decision is made
   * on an alternative term. This is called after the decision is made but
   * before the state is updated.
   *
   * @param machine The parser state machine that fired the event.
   * @param state The subject state.
   * @param route The index of the alternative term to be taken.
   */
  public: virtual void onAlternateRouteDecision(Parser *parser, ParserState *state, Int route)
  {
  }

  /**
   * @brief Called when a route decision is made on a multiply term.
   *
   * This event is raised by the state machine when a routing decision is made
   * on a multiply term. This is called after the decision is made but before
   * the state is updated.
   *
   * @param machine The parser state machine that fired the event.
   * @param state The subject state.
   * @param route Specifies whether the duplicate term is to be taken (1) or
   *              not (0). Naturally, multiple events with a route of 1 can
   *              happen on a duplicate route.
   */
  public: virtual void onMultiplyRouteDecision(Parser *parser, ParserState *state, Int route)
  {
  }

  /**
   * @brief Called when a step is to be made on a concat term.
   *
   * This event is raised by the state machine when parsing is to move to a
   * next position within a concat term. It's called before the state is
   * modified.
   *
   * @param machine The parser state machine that fired the event.
   * @param state The subject state.
   * @param newPos The index of the new position within the concat term.
   */
  public: virtual void onConcatStep(Parser *parser, ParserState *state, Int newPos)
  {
  }

  /**
   * @brief Called when parsing is about to branch at a specific location.
   *
   * This event is raised by the state machine when the given state is about
   * to branch into two.
   *
   * @param machine The parser state machine that fired the event.
   * @param state The subject state (state that is about to be duplicated).
   */
  public: virtual void onBranching(Parser *parser, ParserState *state)
  {
  }

  /**
   * @brief Called after parsing branches into two states.
   *
   * This event is raised by the state machine after branching happens to a
   * state.
   *
   * @param machine The parser state machine that fired the event.
   * @param state1 The branching state that received the higher priority.
   * @param state2 The branching state that received the lower priority.
   */
  public: virtual void onBranched(Parser *parser, ParserState *state1, ParserState *state2)
  {
  }

  /**
   * @brief Called before a specific term level is to be duplicated.
   *
   * Unlike onBranching, this event is raised for each level in the state
   * stack whereas onBranching is raised only for the topmost level.
   *
   * @param machine The parser state machine that fired the event.
   * @param state The subject state (state that is about to be duplicated).
   * @param prodOffset The offset of the production level to which the
   *                    duplicating term level belongs.
   * @param termOffset The offset of the term level that is about to be
   *                    duplicated.
   */
  public: virtual void onTermLevelDuplicating(Parser *parser, ParserState *state, Int prodOffset, Int termOffset)
  {
  }

  /**
   * @brief Called before a specific production level is to be duplicated.
   *
   * Unlike onBranching, this event is raised for each level in the state
   * stack whereas onBranching is raised only for the topmost level.
   *
   * @param machine The parser state machine that fired the event.
   * @param state The subject state (state that is about to be duplicated).
   * @param prodOffset The offset of the production level that is about to
   *                    be duplicated.
   * @param termOffset The offset of the production level's root term level.
   */
  public: virtual void onProdLevelDuplicating(Parser *parser, ParserState *state, Int prodOffset, Int termOffset)
  {
  }

  /**
   * @brief Called after a specific term level is duplicated.
   *
   * Unlike onBranched, this event is raised for each level in the state
   * stack whereas onBranched is raised only for the topmost level.
   *
   * @param machine The parser state machine that fired the event.
   * @param state1 The branching state that received the higher priority.
   * @param state2 The branching state that received the lower priority.
   * @param prodOffset The offset of the production level to which the
   *                    duplicated term level belongs.
   * @param termOffset The offset of the term level that has been duplicated
   * @note The offset are the same in both states.
   */
  public: virtual void onTermLevelDuplicated(Parser *parser, ParserState *state1, ParserState *state2,
                                             Int prodOffset, Int termOffset)
  {
  }

  /**
   * @brief Called after a specific state level is duplicated.
   *
   * Unlike onBranched, this event is raised for each level in the state
   * stack whereas onBranched is raised only for the topmost level.
   *
   * @param machine The parser state machine that fired the event.
   * @param state1 The branching state that received the higher priority.
   * @param state2 The branching state that received the lower priority.
   * @param prodOffset The offset of the production level that has been
   *                    duplicated.
   * @param termOffset The offset of the production level's root term level.
   * @note The offset are the same in both states.
   */
  public: virtual void onProdLevelDuplicated(Parser *parser, ParserState *state1, ParserState *state2,
                                             Int prodOffset, Int termOffset)
  {
  }

  /**
   * @brief Called when a term level is about to be cancelled.
   *
   * This event is raised in case of an error when there is only error states
   * and no successful states. In this case the state machine will be forced
   * to move out of state levels until it reaches a place where it can wait
   * for synchronizing tokens. This event is raised for each of those levels,
   * within each of the error states, that is not a production level.
   *
   * @param machine The parser state machine that fired the event.
   * @param state The subject state.
   */
  public: virtual void onTermCancelling(Parser *parser, ParserState *state)
  {
  }

  /**
   * @brief Called when a production state level is about to be cancelled.
   *
   * This event is raised in case of an error when there is only error states
   * and no successful states. In this case the state machine will be forced
   * to move out of state levels until it reaches a place where it can wait
   * for synchronizing tokens. This event is raised for each of those levels,
   * within each of the error states, that is a production level.
   *
   * @param machine The parser state machine that fired the event.
   * @param state The subject state.
   */
  public: virtual void onProdCancelling(Parser *parser, ParserState *state)
  {
  }

  /**
   * @brief Called when a state is being cancelled due to an error.
   *
   * This event is raised in case of an error when there are successful states
   * alongside the error states. In this case the error states will be
   * cancelled in favour of the successful states. This event will be raised
   * for each state level within each one of those error states.
   *
   * @param machine The parser state machine that fired the event.
   * @param canceledState The state being cancelled.
   * @param winnerState The favoured state.
   * @param levelOffset The index of the state level belonging to this
   *                     parsing handler. For each state, this event will be
   *                     raised for each of its levels. A specific parsing
   *                     handler can receive this event multiple times for
   *                     a single state.
   */
  public: virtual void onStateCancelling(Parser *parser, ParserState *canceledState, ParserState *winnerState)
  {
  }

}; // class

} } // namespace

#endif
