/**
 * @file Core/Data/Node.h
 * Contains the definitions of Core::Data::Node.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_NODE_H
#define CORE_DATA_NODE_H

namespace Core { namespace Data
{

/**
 * @brief The root of all data classes.
 * This class links data objects to their owners. This is needed to allow
 * moving upwards through data trees nodes.
 */
class Node : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Node, TiObject, "Core.Data", "Core", "alusus.net");


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

} } // namespace

#endif
