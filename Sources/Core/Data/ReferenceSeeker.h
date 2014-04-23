/**
 * @file Core/Data/ReferenceSeeker.h
 * Contains the header of class Core::Data::ReferenceSeeker.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_REFERENCESEEKER_H
#define DATA_REFERENCESEEKER_H

namespace Core { namespace Data
{

// TODO: DOC

class ReferenceSeeker
{
  //============================================================================
  // Member Variables

  private: const Provider *dataProvider;


  //============================================================================
  // Constructor & Destructor

  public: ReferenceSeeker(const Provider *prov=0) : dataProvider(prov)
  {
  }

  public: ~ReferenceSeeker()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  public: void setDataProvider(const Provider *prov)
  {
    this->dataProvider = prov;
  }

  public: const Provider* getDataProvider() const
  {
    return this->dataProvider;
  }

  /// @}

  /// @name Helper Functions
  /// @{

  public: Bool getImmediateContainer(ReferenceSegment *seg, const SharedPtr<IdentifiableObject> &parent,
                                     ReferenceSegment *&retSeg, IdentifiableObject *&retParent,
                                     SharedPtr<Module> &retModule) const;

  public: Bool getImmediateContainer(ReferenceSegment *seg, IdentifiableObject *parent,
                                     ReferenceSegment *&retSeg, IdentifiableObject *&retParent,
                                     SharedPtr<Module> &retModule) const;

  public: Bool getImmediateContainer(ReferenceSegment *seg, IdentifiableObject *parent,
                                     ReferenceSegment *&retSeg, IdentifiableObject *&retParent,
                                     Module *&retModule) const;

  public: Bool getImmediateContainer(ReferenceSegment *seg, IdentifiableObject *parent,
                                     ReferenceSegment *&retSeg, IdentifiableObject *&retParent) const;

  /// @}

  /// @name Data Read Functions
  /// @{

  public: const SharedPtr<IdentifiableObject>& getShared(ReferenceSegment *seg, IdentifiableObject *parent) const;

  public: Bool tryGetShared(ReferenceSegment *seg, IdentifiableObject *parent,
                      SharedPtr<IdentifiableObject> &result) const;

  public: void getShared(ReferenceSegment *seg, IdentifiableObject *parent,
                   SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const;

  public: void getShared(ReferenceSegment *seg, const SharedPtr<IdentifiableObject> &parent,
                   SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const;

  public: Bool tryGetShared(ReferenceSegment *seg, IdentifiableObject *parent,
                      SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const;

  public: Bool tryGetShared(ReferenceSegment *seg, const SharedPtr<IdentifiableObject> &parent,
                      SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const;

  /// @}

  /// @name Plain Data Read Functions
  /// @{

  public: IdentifiableObject* getPlain(ReferenceSegment *seg, IdentifiableObject *parent) const;

  public: Bool tryGetPlain(ReferenceSegment *seg, IdentifiableObject *parent, IdentifiableObject *&result) const;

  public: void getPlain(ReferenceSegment *seg, IdentifiableObject *parent,
                        IdentifiableObject *&retVal, Module *&retModule) const;

  public: Bool tryGetPlain(ReferenceSegment *seg, IdentifiableObject *parent,
                           IdentifiableObject *&retVal, Module *&retModule) const;

  /// @}

  /// @name Data Write Functions
  /// @{

  public: void setShared(ReferenceSegment *seg, IdentifiableObject *parent,
                   const SharedPtr<IdentifiableObject> &val) const;

  public: Bool trySetShared(ReferenceSegment *seg, IdentifiableObject *parent,
                      const SharedPtr<IdentifiableObject> &val) const;

  public: void setPlain(ReferenceSegment *seg, IdentifiableObject *parent, IdentifiableObject *val) const;

  public: Bool trySetPlain(ReferenceSegment *seg, IdentifiableObject *parent, IdentifiableObject *val) const;

  /// @}

  /// @name Data Delete Functions
  /// @{

  public: void remove(ReferenceSegment *seg, IdentifiableObject *parent) const;

  public: Bool tryRemove(ReferenceSegment *seg, IdentifiableObject *parent) const;

  /// @}

}; // class

} } // namespace

#endif
