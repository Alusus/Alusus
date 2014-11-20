/**
 * @file Core/Data/SharedRepository.h
 * Contains the header of class Core::Data::SharedRepository.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_SHAREDREPOSITORY_H
#define DATA_SHAREDREPOSITORY_H

namespace Core { namespace Data
{

/**
 * @brief A stack of IdentifiableObject derived data objects.
 */
class SharedRepository : public IdentifiableObject, public virtual SharedProvider
{
  //============================================================================
  // Type Info

  TYPE_INFO(SharedRepository, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(IdentifiableObject, SharedProvider);


  //============================================================================
  // Member Variables

  private: SharedNamedList stack;
  private: Bool owner;

  private: SharedRepository *trunkRepo;
  private: Int trunkIndex;

  protected: ReferenceSeeker referenceSeeker;
  protected: QualifierSeeker qualifierSeeker;


  //============================================================================
  // Constructor & Destructor

  public: SharedRepository() : owner(false), trunkRepo(0), trunkIndex(-1),
    referenceSeeker(static_cast<Provider*>(this)), qualifierSeeker(static_cast<Provider*>(this))
  {
  }

  public: SharedRepository(Word maxScopeNameSize, Word reservedCount) :
    stack(maxScopeNameSize, reservedCount), owner(false), trunkRepo(0), trunkIndex(-1),
    referenceSeeker(static_cast<Provider*>(this)), qualifierSeeker(static_cast<Provider*>(this))
  {
  }

  public: virtual ~SharedRepository()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  public: void initialize(Word maxScopeNameSize, Word reservedCount)
  {
    this->stack.initialize(maxScopeNameSize, reservedCount);
    this->trunkRepo = 0;
    this->trunkIndex = -1;
  }

  public: void reinitialize(Word maxScopeNameSize, Word reservedCount)
  {
    this->stack.reinitialize(maxScopeNameSize, reservedCount);
  }

  public: Word getReservedLevelCount() const
  {
    return this->stack.getReservedCount();
  }

  public: void setOwner(Bool owner)
  {
    this->owner = owner;
  }

  public: Bool isOwner() const
  {
    return this->owner;
  }

  /// @}

  /// @name Data Functions
  /// @{

  public: void pushLevel(SharedPtr<IdentifiableObject> const &obj)
  {
    this->stack.add(obj);
  }

  public: void pushLevel(Char const *scope, SharedPtr<IdentifiableObject> const &obj)
  {
    this->stack.add(scope, obj);
  }

  public: void popLevel();

  public: void setLevel(SharedPtr<IdentifiableObject> const &obj, Int index = -1);

  public: void setLevel(Char const *scope, SharedPtr<IdentifiableObject> const &obj, Int index = -1);

  public: SharedPtr<IdentifiableObject> const& getLevelData(Int index = -1) const;

  public: const SbStr& getLevelScope(Int index = -1) const;

  /**
   * @brief Copy the content of another stack into this stack.
   * This will wipe all contents currently in the stack.
   */
  public: void copyFrom(SharedRepository const *src);

  public: void clear()
  {
    this->stack.clear();
    this->trunkRepo = 0;
    this->trunkIndex = -1;
  }

  public: Word getLevelCount() const
  {
    return this->trunkIndex + 1 + this->stack.getCount();
  }

  /// Checks whether another SharedPtr is sharing the object at given level.
  public: Bool isShared(Int index = -1) const;

  /// @}

  /// @name Branching Functions
  /// @{

  public: void setBranchingInfo(SharedRepository *ds, Int ti);

  public: SharedRepository* getTrunkRepository() const
  {
    return this->trunkRepo;
  }

  private: void ownTopLevel();

  /// @}

  /// @name SharedProvider Implementation
  /// @{

  public: virtual void setSharedValue(Reference const *ref, SharedPtr<IdentifiableObject> const &val);

  public: virtual void setSharedValue(Char const *qualifier, SharedPtr<IdentifiableObject> const &val);

  public: virtual Bool trySetSharedValue(Reference const *ref, SharedPtr<IdentifiableObject> const &val);

  public: virtual Bool trySetSharedValue(Char const *qualifier, SharedPtr<IdentifiableObject> const &val);

  public: virtual SharedPtr<IdentifiableObject> getSharedValue(Reference const *ref);

  public: virtual void getSharedValue(Reference const *ref, SharedModulePairedPtr &retVal);

  public: virtual SharedPtr<IdentifiableObject> getSharedValue(Char const *qualifier);

  public: virtual void getSharedValue(Char const *qualifier, SharedModulePairedPtr &retVal);

  public: virtual Bool tryGetSharedValue(Reference const *ref, SharedPtr<IdentifiableObject> &retVal);

  public: virtual Bool tryGetSharedValue(Reference const *ref, SharedModulePairedPtr &retVal);

  public: virtual Bool tryGetSharedValue(Char const *qualifier, SharedPtr<IdentifiableObject> &retVal);

  public: virtual Bool tryGetSharedValue(Char const *qualifier, SharedModulePairedPtr &retVal);

  /// @}

  /// @name Provider Implementation
  /// @{

  public: virtual void removeValue(Reference const *ref);

  public: virtual void removeValue(Char const *qualifier);

  public: virtual Bool tryRemoveValue(Reference const *ref);

  public: virtual Bool tryRemoveValue(Char const *qualifier);

  public: virtual IdentifiableObject* getPlainValue(Reference const *ref);

  public: virtual void getPlainValue(Reference const *ref, PlainModulePairedPtr &retVal);

  public: virtual IdentifiableObject* getPlainValue(Char const *qualifier);

  public: virtual void getPlainValue(Char const *qualifier, PlainModulePairedPtr &retVal);

  public: virtual Bool tryGetPlainValue(Reference const *ref, IdentifiableObject *&retVal);

  public: virtual Bool tryGetPlainValue(Reference const *ref, PlainModulePairedPtr &retVal);

  public: virtual Bool tryGetPlainValue(Char const *qualifier, IdentifiableObject *&retVal);

  public: virtual Bool tryGetPlainValue(Char const *qualifier, PlainModulePairedPtr &retVal);

  /// @}

}; // class

} } // namespace

#endif
