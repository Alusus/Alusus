/**
 * @file Core/Data/paired_pointers.h
 * Contains the definitions of Core::Data::SharedPairedPtr and
 * Core::Data::PlainPairedPtr classes.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_PAIREDPOINTERS_H
#define DATA_PAIREDPOINTERS_H

namespace Core { namespace Data
{

/**
 * @brief A shared pointer pair of an object and its container.
 * This will be used when seeking objects in data trees to obtain both the
 * object itself and the module that contains it.
 */
class SharedPairedPtr : public Node
{
  //============================================================================
  // Type Info

  TYPE_INFO(SharedPairedPtr, Node, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  public: SharedPtr<IdentifiableObject> object;

  public: SharedPtr<IdentifiableObject> parent;


  //============================================================================
  // Constructors

  public: SharedPairedPtr()
  {
  }

  public: SharedPairedPtr(SharedPtr<IdentifiableObject> const &o, SharedPtr<IdentifiableObject> const &p) :
    object(o), parent(p)
  {
  }

  public: SharedPairedPtr(SharedPairedPtr const &ptr) : object(ptr.object), parent(ptr.parent)
  {
  }


  //============================================================================
  // Operators

  public: SharedPairedPtr& operator=(const SharedPairedPtr &ptr)
  {
    this->object = ptr.object;
    this->parent = ptr.parent;
    return *this;
  }


  //============================================================================
  // Member Functions

  public: void reset()
  {
    this->object.reset();
    this->parent.reset();
  }

}; // class


/**
 * @brief A plain pointer pair of an object and its container.
 * This will be used when seeking objects in data trees to obtain both the
 * object itself and the module that contains it.
 */
class PlainPairedPtr : public Node
{
  //============================================================================
  // Type Info

  TYPE_INFO(PlainPairedPtr, Node, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  public: IdentifiableObject *object;

  public: IdentifiableObject *parent;


  //============================================================================
  // Constructors

  public: PlainPairedPtr(IdentifiableObject *o = 0, IdentifiableObject *m = 0) :
    object(o), parent(m)
  {
  }

  public: PlainPairedPtr(const PlainPairedPtr &ptr) : object(ptr.object), parent(ptr.parent)
  {
  }


  //============================================================================
  // Operators

  public: PlainPairedPtr& operator=(const PlainPairedPtr &ptr)
  {
    this->object = ptr.object;
    this->parent = ptr.parent;
    return *this;
  }


  //============================================================================
  // Member Functions

  public: void reset()
  {
    this->object = 0;
    this->parent = 0;
  }

}; // class

} } // namespace

#endif
