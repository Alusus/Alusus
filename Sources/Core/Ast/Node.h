/**
 * @file Core/Ast/Node.h
 * Contains the definitions of Core::Ast::Node.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_NODE_H
#define CORE_AST_NODE_H

namespace Core::Ast
{

/**
 * @brief The root of all AST classes.
 * @ingroup core_data_ast
 * This class links AST objects to their owners. This is needed to allow
 * moving upwards through AST trees nodes. It also holds the metadata shared by
 * all AST nodes: the production id, the source location, and any extra
 * named objects attached to the node.
 */
class Node : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Node, TiObject, "Core.Ast", "Core", "alusus.org");


  //============================================================================
  // Member Variables

  private: Node *owner;

  protected: Core::Basic::TiWord prodId = UNKNOWN_ID;
  protected: Core::Basic::SharedPtr<Core::Ast::SourceLocation> sourceLocation;
  private: Core::Basic::SharedMap<Core::Basic::TiObject> extras;


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

  /**
   * @brief Set the production id this node refers to.
   *
   * This value refers to the id of the production definition this element
   * represents. If this value is 0, then the element is an inner term, not
   * a production root.
   */
  public: void setProdId(Word id)
  {
    this->prodId = id;
  }
  public: void setProdId(TiWord const *id)
  {
    this->setProdId(id == 0 ? UNKNOWN_ID : id->get());
  }

  /**
   * @brief Get the production id this node refers to.
   *
   * This value refers to the id of the production definition this element
   * represents. If this value is 0, then the element is an inner term, not
   * a production root.
   */
  public: TiWord& getProdId()
  {
    return this->prodId;
  }
  public: TiWord const& getProdId() const
  {
    return this->prodId;
  }

  /**
   * @brief Set the node's location within the source code.
   *
   * Set the location at which the node appeared in the source code. This
   * value refers to the location of the first character in the node. In the
   * case of non-token nodes, this refers to the location of the first token
   * detected inside that node.
   */
  public: void setSourceLocation(SharedPtr<SourceLocation> const &loc)
  {
    this->sourceLocation = loc;
  }
  public: void setSourceLocation(SourceLocation *loc)
  {
    this->setSourceLocation(getSharedPtr(loc));
  }

  public: SharedPtr<SourceLocation> const& getSourceLocation() const
  {
    return this->sourceLocation;
  }

  /**
   * @brief Get the node's location within the source code.
   *
   * If this node has no source location of its own, this function searches
   * its children for the first one that has a source location.
   */
  public: SharedPtr<SourceLocation> const& findSourceLocation() const
  {
    SharedPtr<SourceLocation> const &sl = this->getSourceLocation();
    if (sl == 0) {
      Containing<Node> const *container = this->getInterface<Containing<Node> const>();
      if (container != 0) {
        for (Int i = 0; i < container->getElementCount(); ++i) {
          Node *ptr = container->getElement(i);
          if (ptr != 0) {
            SharedPtr<SourceLocation> const &sl2 = ptr->findSourceLocation();
            if (sl2 != 0) return sl2;
          }
        }
      }
    }
    return sl;
  }

  public: void setExtra(Char const *name, TioSharedPtr const &obj)
  {
    this->extras.set(name, obj);
  }

  public: void removeExtra(Char const *name)
  {
    auto index = this->extras.findIndex(name);
    if (index != -1) this->extras.remove(index);
  }

  public: TioSharedPtr const& getExtra(Char const *name) const
  {
    auto index = this->extras.findIndex(name);
    if (index == -1) return TioSharedPtr::null;
    else return this->extras.get(index);
  }

  /**
   * @brief Print a textual representation of this node and its children.
   *
   * The default implementation prints the node's type name, its production id, and then all its children
   * (with keys if the node is a map container). Derived classes can override this to customize the output.
   */
  public: virtual void print(OutStream &stream, Int indents=0) const;

  public: Str toString(Int indents=0) const
  {
    StrStream stream;
    this->print(stream, indents);
    return stream.str().c_str();
  }

}; // class

} // namespace

#endif
