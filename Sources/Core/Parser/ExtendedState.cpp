/**
 * @file Core/Parser/ExtendedState.cpp
 * Contains the implementation of Parser::ExtendedState.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Parser
{

//==============================================================================
// Constructor

ExtendedState::ExtendedState() :
  processingStatus(ProcessingStatus::IN_PROGRESS),
  prevProcessingStatus(ProcessingStatus::IN_PROGRESS),
  tokensToLive(-1)
{
}


ExtendedState::ExtendedState(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                             Word reservedVarCount, Word reservedVarLevelCount,
                             Data::GrammarModule *root) :
  processingStatus(ProcessingStatus::IN_PROGRESS),
  prevProcessingStatus(ProcessingStatus::IN_PROGRESS),
  tokensToLive(-1),
  State(reservedTermLevelCount, reservedProdLevelCount, maxVarNameLength,
        reservedVarCount, reservedVarLevelCount, root)
{
}


ExtendedState::ExtendedState(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                             Word reservedVarCount, Word reservedVarLevelCount,
                             const Data::ParsingGrammarContext *context) :
  processingStatus(ProcessingStatus::IN_PROGRESS),
  prevProcessingStatus(ProcessingStatus::IN_PROGRESS),
  tokensToLive(-1),
  State(reservedTermLevelCount, reservedProdLevelCount, maxVarNameLength,
        reservedVarCount, reservedVarLevelCount, context)
{
}


//==============================================================================
// Term Stack Member Functions

/**
 * Delete all state levels and reset branching information.
 */
void ExtendedState::reset()
{
  this->processingStatus = ProcessingStatus::IN_PROGRESS;
  this->tokensToLive = -1;
  State::reset();
}


//==============================================================================
// Other Member Functions

/**
 * Decrement the tokens-to-live counter, if this state branched from another
 * state.
 *
 * @return Returns true if the counter reaches 0, false otherwise.
 */
bool ExtendedState::decrementTokensToLive()
{
  if (this->tokensToLive == -1) {
    return false;
  } else if (this->tokensToLive > 1) {
    this->tokensToLive--;
    return false;
  } else {
    this->tokensToLive = 0;
    return true;
  }
}

} } // namespace
