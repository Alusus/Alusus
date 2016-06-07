/**
 * @file Scg/Containers/List.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_LIST_H
#define SCG_LIST_H

#include <typedefs.h>
#include <AstNode.h>
#include <llvm_fwd.h>

namespace Scg
{

// TODO: Why is List derived from AstNode?

/**
 * Represent a return statement.
 */
class List : public AstNode, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(List, AstNode, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(AstNode, Core::Data::Container);


  //============================================================================
  // Member Variables

  private: Core::Data::SharedList children;


  //============================================================================
  // Constructor & Destructor

  /**
   * Construct a return statement that returns the value of the given expression.
   *
   * @param[in] expression  A pointer to the expression to be returned. Notice that
   *                        this expression will automatically get deleted, so it
   *                        should be allocated in the heap and not deleted.
   */
  public: List(AstNodeSharedArray const &elements)
  {
    for (auto element : elements) {
      this->children.add(element);
      OWN_SHAREDPTR(element);
    }
  }

  public: virtual ~List()
  {
    for (Int i = 0; i < this->getCount(); ++i) {
      DISOWN_PLAINPTR(this->get(i));
    }
  }

  public: static SharedPtr<List> create(AstNodeSharedArray const &elements)
  {
    return std::make_shared<List>(elements);
  }


  //============================================================================
  // Member Functions

  public: SharedPtr<AstNode> getShared(Int index)
  {
    return this->children.getShared(index).s_cast<AstNode>();
  }

  //! @copydoc AstNode::toString()
  virtual std::string toString();


  //============================================================================
  // Container Implementation

  public: virtual void set(Int index, TiObject *val)
  {
    DISOWN_PLAINPTR(this->children.get(index));
    this->children.set(index, val);
    OWN_PLAINPTR(val);
  }

  public: virtual void remove(Int index)
  {
    DISOWN_PLAINPTR(this->children.get(index));
    this->children.remove(index);
  }

  public: virtual Word getCount() const
  {
    return this->children.getCount();
  }

  public: virtual TiObject* get(Int index) const
  {
    return this->children.get(index);
  }

}; // class

} // namespace

#endif
