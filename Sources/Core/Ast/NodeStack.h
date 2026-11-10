/**
 * @file Core/Ast/NodeStack.h
 * Contains the header of class Core::Ast::NodeStack.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_NODESTACK_H
#define CORE_AST_NODESTACK_H

namespace Core::Ast
{

/**
 * @brief A stack of AST nodes.
 */
class NodeStack : public Node, public DynamicContaining<Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(NodeStack, Node, "Core.Ast", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(DynamicContaining<Node>)
  ));


  //============================================================================
  // Member Variables

  // A plain vector is used rather than a SharedList because this stack is at the heart of the parser's operations and
  // doesn't need the SharedList's inheritance or change notifications.
  private: std::vector<SharedPtr<Node>> stack;

  private: NodeStack *trunkStack;
  private: Int trunkIndex;


  //============================================================================
  // Constructor & Destructor

  public: NodeStack() : trunkStack(0), trunkIndex(-1)
  {
  }

  public: virtual ~NodeStack()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Data Functions
  /// @{

  public: void reserve(Int size)
  {
    this->stack.reserve(size);
  }

  public: void push(SharedPtr<Node> const &obj)
  {
    this->stack.push_back(obj);
  }

  public: void pop();

  public: void set(SharedPtr<Node> const &obj, Int index = -1);

  public: SharedPtr<Node> const& get(Int index = -1) const;

  public: Word getCount() const
  {
    return this->trunkIndex + 1 + this->stack.size();
  }

  /**
   * @brief Copy the content of another stack into this stack.
   * This will wipe all contents currently in the stack.
   */
  public: void copyFrom(NodeStack const *src);

  public: void clear()
  {
    this->stack.clear();
    this->trunkStack = 0;
    this->trunkIndex = -1;
  }

  /// Checks whether another SharedPtr is sharing the object at given level.
  public: Bool isShared(Int index = -1) const;

  /// @}

  /// @name Branching Functions
  /// @{

  public: void setBranchingInfo(NodeStack *ds, Int ti);

  public: NodeStack* getTrunkStack() const
  {
    return this->trunkStack;
  }

  private: void ownTop();

  /// @}

  /// @name DynamicContaining Implementation
  /// @{

  public: virtual void setElement(Int index, Node *val)
  {
    this->set(getSharedPtr(val), index);
  }

  public: virtual void removeElement(Int index);

  public: virtual Word getElementCount() const
  {
    return this->getCount();
  }

  public: virtual Node* getElement(Int index) const
  {
    return this->get(index).get();
  }

  public: virtual Int addElement(Node *val)
  {
    this->push(getSharedPtr(val));
    return this->getCount() - 1;
  }

  public: virtual void insertElement(Int index, Node *val);

  public: virtual TypeInfo const* getElementsNeededType() const
  {
    return Node::getTypeInfo();
  }

  public: virtual HoldMode getElementsHoldMode() const
  {
    return HoldMode::SHARED_REF;
  }

  /// @}

}; // class

} // namespace

#endif
