/**
 * @file Core/Grammar/Node.h
 * Contains the definitions of Core::Grammar::Node.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_GRAMMAR_NODE_H
#define CORE_GRAMMAR_NODE_H

namespace Core::Grammar
{

/**
 * @brief The root of all grammar classes.
 * @ingroup core_data_grammar
 * This class links grammar objects to their owners. This is needed to allow
 * moving upwards through grammar trees nodes.
 */
class Node : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Node, TiObject, "Core.Grammar", "Core", "alusus.org");


  //============================================================================
  // Member Variables

  private: Node *owner;


  //============================================================================
  // Constructors

  public: Node() : owner(0)
  {
  }


  //============================================================================
  // Member Functions

  public: void setOwner(Node *o)
  {
    this->owner = o;
  }

  public: Node* getOwner() const
  {
    return this->owner;
  }

  /// Find a node's owner of a specific type.
  template<class T> T* findOwner() const
  {
    Node *node = this->getOwner();
    while (node != 0 && !node->isDerivedFrom<T>()) node = node->getOwner();
    return static_cast<T*>(node);
  }

}; // class

} // namespace

#endif
