/**
 * @file Core/Processing/DecisionNodePool.cpp
 * Contains the implementation of Processing::DecisionNodePool.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing
{

Int DecisionNodePool::addNode(ParserState *state, Int posId)
{
  Int index = this->decisionNodes.size();
  this->decisionNodes.push_back(DecisionNode(posId, state->getTermLevelCount()-1, state->getDecisionNodeIndex()));
  if (state->getDecisionNodeIndex() != -1) {
    if (this->decisionNodes[state->getDecisionNodeIndex()].childIndex == -1) {
      this->decisionNodes[state->getDecisionNodeIndex()].childIndex = index;
    } else {
      Int sibling = this->decisionNodes[state->getDecisionNodeIndex()].childIndex;
      while (this->decisionNodes[sibling].siblingIndex != -1) {
        sibling = this->decisionNodes[sibling].siblingIndex;
      }
      this->decisionNodes[sibling].siblingIndex = index;
    }
  }
  state->setDecisionNodeIndex(index);
  return index;
}


Int DecisionNodePool::addSiblingNode(ParserState *state, Int posId, Int childId)
{
  Int index = this->decisionNodes.size();
  this->decisionNodes.push_back(DecisionNode(posId, state->getTermLevelCount()-1, -1));
  this->decisionNodes[index].childIndex = childId;
  if (state->getDecisionNodeIndex() == -1) {
    state->setDecisionNodeIndex(index);
  } else {
    Int sibling = state->getDecisionNodeIndex();
    while (this->decisionNodes[sibling].siblingIndex != -1) {
      sibling = this->decisionNodes[sibling].siblingIndex;
    }
    this->decisionNodes[sibling].siblingIndex = index;
    this->decisionNodes[index].parentIndex = this->decisionNodes[sibling].parentIndex;
  }
  return index;
}


Int DecisionNodePool::removeNode(ParserState *state)
{
  ASSERT(state->getDecisionNodeIndex() != -1);
  Int index = this->decisionNodes[state->getDecisionNodeIndex()].parentIndex;
  if (index != -1) {
    if (this->decisionNodes[index].childIndex == state->getDecisionNodeIndex()) {
      this->decisionNodes[index].childIndex = this->decisionNodes[state->getDecisionNodeIndex()].siblingIndex;
    } else {
      Int sibling = this->decisionNodes[index].childIndex;
      while (this->decisionNodes[sibling].siblingIndex != state->getDecisionNodeIndex()) {
        sibling = this->decisionNodes[sibling].siblingIndex;
      }
      this->decisionNodes[sibling].siblingIndex = this->decisionNodes[state->getDecisionNodeIndex()].siblingIndex;
    }
  }
  state->setDecisionNodeIndex(index);
  return index;
}


Int DecisionNodePool::moveToCurrentNode(ParserState *state)
{
  Int index = state->getDecisionNodeIndex();
  ASSERT(index != -1);
  while (this->decisionNodes[index].levelIndex != state->getTermLevelCount()-1) {
    index = this->decisionNodes[index].parentIndex;
    state->setDecisionNodeIndex(index);
    if (index == -1) return index;
  }
  return state->getDecisionNodeIndex();
}

} } // namespace
