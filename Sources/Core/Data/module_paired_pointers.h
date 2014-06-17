/**
 * @file Core/Data/module_paired_pointers.h
 * Contains the definitions of Core::Data::SharedModulePairedPtr and
 * Core::Data::PlainModulePairedPtr classes.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_MODULEPAIREDPOINTERS_H
#define DATA_MODULEPAIREDPOINTERS_H

namespace Core { namespace Data
{

class Module;

/**
 * @brief A shared pointer pair of an object and its containing module.
 * This will be used when seeking objects in data trees to obtain both the
 * object itself and the module that contains it.
 */
class SharedModulePairedPtr : public IdentifiableObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(SharedModulePairedPtr, IdentifiableObject, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  public: SharedPtr<IdentifiableObject> object;

  public: SharedPtr<Module> module;


  //============================================================================
  // Constructors

  public: SharedModulePairedPtr()
  {
  }

  public: SharedModulePairedPtr(const SharedPtr<IdentifiableObject> &o, const SharedPtr<Module> &m) :
    object(o), module(m)
  {
  }

  public: SharedModulePairedPtr(const SharedModulePairedPtr &ptr) : object(ptr.object), module(ptr.module)
  {
  }


  //============================================================================
  // Operators

  public: SharedModulePairedPtr& operator=(const SharedModulePairedPtr &ptr)
  {
    this->object = ptr.object;
    this->module = ptr.module;
    return *this;
  }


  //============================================================================
  // Member Functions

  public: void reset()
  {
    this->object.reset();
    this->module.reset();
  }

}; // class


/**
 * @brief A plain pointer pair of an object and its containing module.
 * This will be used when seeking objects in data trees to obtain both the
 * object itself and the module that contains it.
 */
class PlainModulePairedPtr : public IdentifiableObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(PlainModulePairedPtr, IdentifiableObject, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  public: IdentifiableObject *object;

  public: Module *module;


  //============================================================================
  // Constructors

  public: PlainModulePairedPtr(IdentifiableObject *o = 0, Module *m = 0) :
    object(o), module(m)
  {
  }

  public: PlainModulePairedPtr(const PlainModulePairedPtr &ptr) : object(ptr.object), module(ptr.module)
  {
  }


  //============================================================================
  // Operators

  public: PlainModulePairedPtr& operator=(const PlainModulePairedPtr &ptr)
  {
    this->object = ptr.object;
    this->module = ptr.module;
    return *this;
  }


  //============================================================================
  // Member Functions

  public: void reset()
  {
    this->object = 0;
    this->module = 0;
  }

}; // class

} } // namespace

#endif
