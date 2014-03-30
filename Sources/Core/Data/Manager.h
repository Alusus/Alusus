/**
 * @file Core/Data/Manager.h
 * Contains the header of class Data::Manager.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

namespace Core { namespace Data
{

// TODO: DOC

class Manager : public IdentifiableObject, public virtual Provider
{
  //============================================================================
  // Type Info

  TYPE_INFO(Manager, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(IdentifiableObject, Provider);


  //============================================================================
  // Member Variables

  // TODO: Add a signal to notify the user of changes in definitions. This should be done by adding the signal to the
  //       Container interface and have each container link itself to any owned container and have the signals
  //       cascade up until it reaches the Manager, which in turn cascades it again to the user.
  //       This should be implemented by changing signals to use IdentifiableObject derived signal receivers which
  //       implements a Signal_Target interface if it needs bi-directional linking, otherwise the linking is one
  //       directional, which is gonna be the case for Containers since containers already have pointers to their
  //       children so having pointers to their signals as well is redundant.
  // TODO: This signal should also be used to inform of Reference index changes. And the Manager should provide such
  //       signal externally so that external Reference owners can also reset their references in case of changes
  //       in the index.

  // TODO: Set definition IDs in the response to definition signals.

  // TODO: Should we prevent users from making definitions directly into modules?

  protected: SharedPtr<Module> root;

  protected: Server server;
  protected: RawServer rawServer;


  //============================================================================
  // Constructor & Destructor

  public: Manager() : server(static_cast<Provider*>(this)), rawServer(static_cast<Provider*>(this))
  {
  }

  public: virtual ~Manager()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  public: void setRootModule(const SharedPtr<Module> &r)
  {
    this->root = r;
    if (r != 0) {
      r->setId(0);
      this->setChildIds(r.get(), STR(""));
    }
  }

  public: const SharedPtr<Module>& getRootModule() const
  {
    return this->root;
  }

  /// @}

  /// @name Definitions Functions
  /// @{

  public: void setValue(Reference *ref, const SharedPtr<IdentifiableObject> &obj);

  public: void setValue(const Char *qualifier, const SharedPtr<IdentifiableObject> &obj);

  public: void removeValue(Reference *ref);

  public: void removeValue(const Char *qualifier);

  public: const SharedPtr<IdentifiableObject>& getValue(Reference *ref);

  public: const SharedPtr<IdentifiableObject>& getValue(const Char *qualifier);

  public: void getValue(Reference *ref, SharedPtr<IdentifiableObject> &retVal,
                        SharedPtr<Module> &retModule);

  public: void getValue(const Char *qualifier, SharedPtr<IdentifiableObject> &retVal,
                        SharedPtr<Module> &retModule);

  protected: void setId(IdentifiableObject *obj, const Char *id);

  protected: void setChildIds(IdentifiableObject *obj, const Char *id);

  /// @}


  ////////////////////////////////////////////////////////////////////////////
  // Provider Implementation

  public: virtual IdentifiableObject* getPlainValue(Reference *ref) const;

  public: virtual void getPlainValue(Reference *ref, IdentifiableObject *&retVal, Module *&retModule) const;

  public: virtual IdentifiableObject* getPlainValue(const Char *qualifier) const;

  public: virtual void getPlainValue(const Char *qualifier, IdentifiableObject *&retVal,
                                     Module *&retModule) const;

  public: virtual Bool tryGetPlainValue(Reference *ref, IdentifiableObject *&retVal) const;

  public: virtual Bool tryGetPlainValue(Reference *ref, IdentifiableObject *&retVal, Module *&retModule) const;

  public: virtual Bool tryGetPlainValue(const Char *qualifier, IdentifiableObject *&retVal) const;

  public: virtual Bool tryGetPlainValue(const Char *qualifier, IdentifiableObject *&retVal,
                                        Module *&retModule) const;

}; // class

} } // namespace

#endif
