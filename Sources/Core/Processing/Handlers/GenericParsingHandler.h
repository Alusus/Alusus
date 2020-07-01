/**
 * @file Core/Processing/Handlers/GenericParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::GenericParsingHandler.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_GENERICPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_GENERICPARSINGHANDLER_H

namespace Core::Processing::Handlers
{

/**
 * @brief Creates a data tree for parsed data.
 * @ingroup core_processing_handlers
 *
 * This generic parsing handler simply creates a data tree that represents the
 * parsed code in a generic way. This is the default parsing handler that will
 * be used wherever a specific parsing handler is not defined. The generated
 * data can then be used by other parsing handlers up in the grammar tree, or
 * it can be simply passed to the calling function/applictaion.<br>
 * This parsing handler uses Ast::List, Ast::Route, and Ast::Token to
 * compose the parsed tree.
 */
class GenericParsingHandler : public ParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(GenericParsingHandler, ParsingHandler, "Core.Processing.Handlers", "Core", "alusus.org");


  //============================================================================
  // Constructor

  public: GenericParsingHandler()
  {
  }

  public: static SharedPtr<GenericParsingHandler> create()
  {
    return std::make_shared<GenericParsingHandler>();
  }


  //============================================================================
  // Member Functions

  /// @name Overloaded Functions
  /// @{

  /**
   * @brief Called when the parsing exists a production level.
   * Assign the produciton id to the production data. If the data is null or
   * belongs to another production and this production object is enforced, then
   * this function will create a new data item and assign the production to it
   * after assigning the existing data as a child of the newly created data.
   *
   * @sa ParsingHandler::onProdEnd()
   */
  public: virtual void onProdEnd(Parser *parser, ParserState *state);

  /**
   * @brief Called when parsing exits a term level.
   * Passes the data of the top level to the first level above it that can
   * receive the data, i.e. that isn't set to pass the data up. If the top
   * level is set to pass the data up we will skip because the data would
   * already be passed up by now.
   *
   * @sa ParsingHandler::onTermEnd()
   */
  public: virtual void onTermEnd(Parser *parser, ParserState *state);

  /**
   * @brief Called after parsing exists a state level.
   * Set the data of a reference term with the data received from the referenced
   * production. If the term type at the top state level is not reference this
   * function will do nothing.
   *
   * @sa ParsingHandler::onLevelExit()
   */
  public: virtual void onLevelExit(Parser *parser, ParserState *state,
                                   SharedPtr<TiObject> const &data);

  /**
   * @brief Called when a new successful token is received.
   * Create a Ast::Token object and associate it with the current state level.
   * If the token term is omissible and the token is a constant token, no data
   * will be created.
   *
   * @sa ParsingHandler::onNewToken()
   */
  public: virtual void onNewToken(Parser *parser, ParserState *state,
                                  Data::Token const *token);

  /**
   * @brief Called when a step is to be made on a concat term.
   * Create a Ast::List object if this level's term has a FORCE_LIST parsing
   * flag set and no list is already created. If this term has the PASS_UP flag
   * nothing will be done.
   *
   * @sa ParsingHandler::onConcatStep()
   */
  public: virtual void onConcatStep(Parser *parser, ParserState *state,
                                    Int newPos, Data::Token const *token);

  /**
   * @brief Called when a route decision is made on an alternative term.
   * Create a Ast::Route object and associate it with the current state level
   * if this level's term is not PASS_UP flagged.
   *
   * @sa ParsingHandler::onAlternateRouteDecision()
   */
  public: virtual void onAlternateRouteDecision(Parser *parser, ParserState *state,
                                                Int route, Data::Token const *token);

  /**
   * @brief Called when a route decision is made on a duplicate term.
   * For optional multiply terms (min==0 && max==1):<br>
   * This function simply calls onAlternateRouteDecision since the
   * implementation for optional terms is similar to that of alternative terms.
   * <br>
   * For other multiply terms:<br>
   * Create a Ast::List object if this level's term has a FORCE_LIST parsing
   * flag set and no list is already created. If this term has the PASS_UP flag
   * nothing will be done.
   *
   * @sa ParsingHandler::onMultiplyRouteDecision()
   */
  public: virtual void onMultiplyRouteDecision(Parser *parser, ParserState *state,
                                               Int route, Data::Token const *token);

  /**
   * @brief Wipe out any generated data from the canceled top level.
   * Wipe out any generated data from the canceled term level.
   * @sa ParsingHandler::onTermCancelling()
   */
  public: virtual void onTermCancelling(Parser *parser, ParserState *state);

  /**
   * @brief Wipe out any generated data from the canceled top level.
   * Wipe out any generated data from the canceled production level.
   * This function will simply call onTermCancelling.
   * @sa ParsingHandler::onProdCancelling()
   */
  public: virtual void onProdCancelling(Parser *parser, ParserState *state);

  /// @}

  /// @name Member Functions
  /// @{

  /// Add the given data to the given state level.
  protected: virtual void addData(SharedPtr<TiObject> const &data, Parser *parser, ParserState *state, Int levelIndex);

  protected: virtual Bool isListObjEnforced(ParserState *state, Int levelIndex);
  protected: virtual Bool isListItemEnforced(ParserState *state, Int levelIndex);
  protected: virtual Bool isRouteObjEnforced(ParserState *state, Int levelIndex);
  protected: virtual Bool isPassUpList(ParserState *state, Int levelIndex);
  protected: virtual Bool isProdObjEnforced(ParserState *state);

  protected: virtual SharedPtr<TiObject> createListNode(ParserState *state, Int levelIndex);
  protected: virtual SharedPtr<TiObject> createRouteNode(ParserState *state, Int levelIndex, Int route);
  protected: virtual SharedPtr<TiObject> createTokenNode(ParserState *state, Int levelIndex,
                                                         Word tokenId, Char const *tokenText);
  protected: virtual SharedPtr<TiObject> createEnforcedProdNode(ParserState *state);

  protected: Bool isRouteTerm(ParserState *state, Int levelIndex);
  protected: Bool isListTerm(ParserState *state, Int levelIndex);

  /// @}

}; // class

} // namespace

#endif
