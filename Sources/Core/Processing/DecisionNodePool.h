/**
 * @file Core/Processing/DecisionNodePool.h
 * Contains the header of class Core::Processing::DecisionNodePool.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_DECISIONNODEPOOL_H
#define PROCESSING_DECISIONNODEPOOL_H

namespace Core { namespace Processing
{

// TODO: DOC

class DecisionNodePool
{
  //============================================================================
  // Data Types

  private: struct DecisionNode
  {
    Int posId;
    Int levelIndex;
    Int parentIndex;
    Int childIndex;
    Int siblingIndex;

    DecisionNode() : posId(-1), levelIndex(-1), parentIndex(-1), childIndex(-1), siblingIndex(-1)
    {
    }

    DecisionNode(Int posId, Int levelIndex, Int parentIndex) :
      posId(posId), levelIndex(levelIndex), parentIndex(parentIndex), childIndex(-1), siblingIndex(-1)
    {
    }
  };


  //============================================================================
  // Member Variables

  private: std::vector<DecisionNode> decisionNodes;


  //============================================================================
  // Constructor / Destructor

  public: DecisionNodePool()
  {
    this->decisionNodes.reserve(1000);
  }

  public: ~DecisionNodePool()
  {
  }


  //============================================================================
  // Member Functions

  public: void clear()
  {
    this->decisionNodes.clear();
  }

  public: Int addNode(ParserState *state, Int posId);

  public: Int addSiblingNode(ParserState *state, Int posId, Int childId=-1);

  public: Int removeNode(ParserState *state);

  public: Int moveToCurrentNode(ParserState *state);

  public: Int getPosId(Int index)
  {
    return this->decisionNodes[index].posId;
  }

  public: Int getPosId(ParserState *state)
  {
    return this->decisionNodes[state->getDecisionNodeIndex()].posId;
  }

  public: Int getLevelIndex(Int index)
  {
    return this->decisionNodes[index].levelIndex;
  }

  public: Int getLevelIndex(ParserState *state)
  {
    return this->decisionNodes[state->getDecisionNodeIndex()].levelIndex;
  }

  public: Int getParentIndex(Int index)
  {
    return this->decisionNodes[index].parentIndex;
  }

  public: Int getParentIndex(ParserState *state)
  {
    return this->decisionNodes[state->getDecisionNodeIndex()].parentIndex;
  }

  public: Int getChildIndex(Int index)
  {
    return this->decisionNodes[index].childIndex;
  }

  public: Int getChildIndex(ParserState *state)
  {
    return this->decisionNodes[state->getDecisionNodeIndex()].childIndex;
  }

  public: Int getSiblingIndex(Int index)
  {
    return this->decisionNodes[index].siblingIndex;
  }

  public: Int getSiblingIndex(ParserState *state)
  {
    return this->decisionNodes[state->getDecisionNodeIndex()].siblingIndex;
  }

}; // class

} } // namespace

#endif
