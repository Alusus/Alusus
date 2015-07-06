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
class SharedRepository : public IdentifiableObject, public virtual Provider
{
  //============================================================================
  // Type Info

  TYPE_INFO(SharedRepository, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(IdentifiableObject, Provider);


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

  /// @name Provider Implementation
  /// @{

  public: virtual Bool trySet(Reference const *ref, IdentifiableObject *val);

  public: virtual Bool trySet(Char const *qualifier, IdentifiableObject *val);

  public: virtual Bool tryRemove(Reference const *ref);

  public: virtual Bool tryRemove(Char const *qualifier);

  using Provider::tryGet;

  public: virtual Bool tryGet(Reference const *ref, IdentifiableObject *&retVal,
                              TypeInfo const *parentTypeInfo=0, IdentifiableObject **retParent=0);

  public: virtual Bool tryGet(Char const *qualifier, IdentifiableObject *&retVal,
                              TypeInfo const *parentTypeInfo=0, IdentifiableObject **retParent=0);

  /// @}

}; // class

} } // namespace

#endif
