/**
 * @file Core/Standard/GenericParsingHandler.h
 * Contains the header of class Core::Standard::GenericParsingHandler.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef STANDARD_GENERICPARSINGHANDLER_H
#define STANDARD_GENERICPARSINGHANDLER_H

namespace Core { namespace Standard
{

/**
 * @brief Creates a data tree for parsed data.
 * @ingroup standard
 *
 * This generic parsing handler simply creates a data tree that represents the
 * parsed code in a generic way. This is the default parsing handler that will
 * be used wherever a specific parsing handler is not defined. The generated
 * data can then be used by other parsing handlers up in the grammar tree, or
 * it can be simply passed to the calling function/applictaion.<br>
 * This parsing handler uses ParsedList, ParsedRoute, and ParsedToken to
 * compose the parsed tree.
 */
class GenericParsingHandler : public Processing::ParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(GenericParsingHandler, Processing::ParsingHandler, "Core.Standard", "Core", "alusus.net");


  //============================================================================
  // Constructor

  public: GenericParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Overloaded Functions
  /// @{

  /// Called when the parsing exists a production level.
  public: virtual void onProdEnd(Processing::Parser *machine, Processing::ParserState *state);

  /// Called when parsing exits a term level.
  public: virtual void onTermEnd(Processing::Parser *machine, Processing::ParserState *state);

  /// Called after parsing exists a state level.
  public: virtual void onLevelExit(Processing::Parser *machine, Processing::ParserState *state,
                                   SharedPtr<IdentifiableObject> const &data);

  /// Called when a new successful token is received.
  public: virtual void onNewToken(Processing::Parser *machine, Processing::ParserState *state,
                                  const Data::Token *token);

  /// Called when a step is to be made on a concat term.
  public: virtual void onConcatStep(Processing::Parser *machine, Processing::ParserState *state,
                                    Int newPos);

  /// Called when a route decision is made on an alternative term.
  public: virtual void onAlternateRouteDecision(Processing::Parser *machine, Processing::ParserState *state,
                                                Int route);

  /// Called when a route decision is made on a duplicate term.
  public: virtual void onMultiplyRouteDecision(Processing::Parser *machine, Processing::ParserState *state,
                                               Int route);

  /// Wipe out any generated data from the canceled top level.
  public: virtual void onTermCancelling(Processing::Parser *machine, Processing::ParserState *state);

  /// Wipe out any generated data from the canceled top level.
  public: virtual void onProdCancelling(Processing::Parser *machine, Processing::ParserState *state);

  /// @}

  /// @name Member Functions
  /// @{

  // TODO:

  /// Set the given data (of a child term) into the specified state level.
  private: void setChildData(SharedPtr<IdentifiableObject> const &data, Processing::ParserState *state, Int levelIndex);

  /// Conver a string literal text to the actual string.
  private: void parseStringLiteralControlCharacters(const Str &src, Str &dest);

  /// Make sure the top term level is not shared.
  private: void prepareToModifyData(Processing::ParserState *state, Int levelIndex);

  /// @}

}; // class

} } // namespace

#endif
