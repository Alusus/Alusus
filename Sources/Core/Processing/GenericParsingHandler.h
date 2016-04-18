/**
 * @file Core/Processing/GenericParsingHandler.h
 * Contains the header of class Core::Processing::GenericParsingHandler.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_GENERICPARSINGHANDLER_H
#define PROCESSING_GENERICPARSINGHANDLER_H

namespace Core { namespace Processing
{

/**
 * @brief Creates a data tree for parsed data.
 * @ingroup processing_parser
 *
 * This generic parsing handler simply creates a data tree that represents the
 * parsed code in a generic way. This is the default parsing handler that will
 * be used wherever a specific parsing handler is not defined. The generated
 * data can then be used by other parsing handlers up in the grammar tree, or
 * it can be simply passed to the calling function/applictaion.<br>
 * This parsing handler uses PrtList, PrtRoute, and PrtToken to
 * compose the parsed tree.
 */
class GenericParsingHandler : public ParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(GenericParsingHandler, ParsingHandler, "Core.Standard", "Core", "alusus.net");


  //============================================================================
  // Constructor

  public: GenericParsingHandler()
  {
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
  public: virtual void onProdEnd(Processing::Parser *parser, Processing::ParserState *state);

  /**
   * @brief Called when parsing exits a term level.
   * Passes the data of the top level to the first level above it that can
   * receive the data, i.e. that isn't set to pass the data up. If the top
   * level is set to pass the data up we will skip because the data would
   * already be passed up by now.
   *
   * @sa ParsingHandler::onTermEnd()
   */
  public: virtual void onTermEnd(Processing::Parser *parser, Processing::ParserState *state);

  /**
   * @brief Called after parsing exists a state level.
   * Set the data of a reference term with the data received from the referenced
   * production. If the term type at the top state level is not reference this
   * function will do nothing.
   *
   * @sa ParsingHandler::onLevelExit()
   */
  public: virtual void onLevelExit(Processing::Parser *parser, Processing::ParserState *state,
                                   SharedPtr<IdentifiableObject> const &data);

  /**
   * @brief Called when a new successful token is received.
   * Create a PrtToken object and associate it with the current state level.
   * If the token term is omissible and the token is a constant token, no data
   * will be created.
   *
   * @sa ParsingHandler::onNewToken()
   */
  public: virtual void onNewToken(Processing::Parser *parser, Processing::ParserState *state,
                                  const Data::Token *token);

  /**
   * @brief Called when a step is to be made on a concat term.
   * Create a PrtList object if this level's term has a FORCE_LIST parsing
   * flag set and no list is already created. If this term has the PASS_UP flag
   * nothing will be done.
   *
   * @sa ParsingHandler::onConcatStep()
   */
  public: virtual void onConcatStep(Processing::Parser *parser, Processing::ParserState *state,
                                    Int newPos);

  /**
   * @brief Called when a route decision is made on an alternative term.
   * Create a PrtRoute object and associate it with the current state level
   * if this level's term is not PASS_UP flagged.
   *
   * @sa ParsingHandler::onAlternateRouteDecision()
   */
  public: virtual void onAlternateRouteDecision(Processing::Parser *parser, Processing::ParserState *state,
                                                Int route);

  /**
   * @brief Called when a route decision is made on a duplicate term.
   * For optional multiply terms (min==0 && max==1):<br>
   * This function simply calls onAlternateRouteDecision since the
   * implementation for optional terms is similar to that of alternative terms.
   * <br>
   * For other multiply terms:<br>
   * Create a PrtList object if this level's term has a FORCE_LIST parsing
   * flag set and no list is already created. If this term has the PASS_UP flag
   * nothing will be done.
   *
   * @sa ParsingHandler::onMultiplyRouteDecision()
   */
  public: virtual void onMultiplyRouteDecision(Processing::Parser *parser, Processing::ParserState *state,
                                               Int route);

  /**
   * @brief Wipe out any generated data from the canceled top level.
   * Wipe out any generated data from the canceled term level.
   * @sa ParsingHandler::onTermCancelling()
   */
  public: virtual void onTermCancelling(Processing::Parser *parser, Processing::ParserState *state);

  /**
   * @brief Wipe out any generated data from the canceled top level.
   * Wipe out any generated data from the canceled production level.
   * This function will simply call onTermCancelling.
   * @sa ParsingHandler::onProdCancelling()
   */
  public: virtual void onProdCancelling(Processing::Parser *parser, Processing::ParserState *state);

  /// @}

  /// @name Member Functions
  /// @{

  /// Add the given data to the given state level.
  protected: virtual void addData(SharedPtr<IdentifiableObject> const &data, ParserState *state, Int levelIndex);

  protected: virtual Bool isListObjEnforced(ParserState *state, Int levelIndex);
  protected: virtual Bool isListItemEnforced(ParserState *state, Int levelIndex);
  protected: virtual Bool isRouteObjEnforced(ParserState *state, Int levelIndex);
  protected: virtual Bool isPassUpList(ParserState *state, Int levelIndex);
  protected: virtual Bool isProdObjEnforced(ParserState *state);

  protected: virtual SharedPtr<IdentifiableObject> createListNode(ParserState *state, Int levelIndex);
  protected: virtual SharedPtr<IdentifiableObject> createRouteNode(ParserState *state, Int levelIndex, Int route);
  protected: virtual SharedPtr<IdentifiableObject> createTokenNode(ParserState *state, Int levelIndex,
                                                                   Word tokenId, Char const *tokenText);
  protected: virtual SharedPtr<IdentifiableObject> createEnforcedProdNode(ParserState *state);

  protected: Bool isRouteTerm(ParserState *state, Int levelIndex);
  protected: Bool isListTerm(ParserState *state, Int levelIndex);

  /**
   * @brief Make sure the top term level is not shared.
   * If the top level is shared (the shared pointer is not unique) this function
   * will duplicate that term.
   */
  private: void prepareToModifyData(Processing::ParserState *state, Int levelIndex);

  /// @}

}; // class

} } // namespace

#endif
