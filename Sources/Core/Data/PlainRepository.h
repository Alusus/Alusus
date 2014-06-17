/**
 * @file Core/Data/PlainRepository.h
 * Contains the header of class Core::Data::PlainRepository.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_PLAINREPOSITORY_H
#define DATA_PLAINREPOSITORY_H

namespace Core { namespace Data
{

/**
 * @brief A stack of IdentifiableObject derived data objects.
 */
class PlainRepository : public IdentifiableObject, public virtual PlainProvider
{
  //============================================================================
  // Type Info

  TYPE_INFO(PlainRepository, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(IdentifiableObject, PlainProvider);


  //============================================================================
  // Member Variables

  private: PlainNamedList stack;
  private: Bool owner;

  private: PlainRepository *trunkRepo;
  private: Int trunkIndex;

  protected: ReferenceSeeker referenceSeeker;
  protected: QualifierSeeker qualifierSeeker;


  //============================================================================
  // Constructor & Destructor

  public: PlainRepository() : owner(false), trunkRepo(0), trunkIndex(-1),
    referenceSeeker(static_cast<Provider*>(this)), qualifierSeeker(static_cast<Provider*>(this))
  {
  }

  public: PlainRepository(Word maxScopeNameSize, Word reservedCount) :
    stack(maxScopeNameSize, reservedCount), owner(false), trunkRepo(0), trunkIndex(-1),
    referenceSeeker(static_cast<Provider*>(this)), qualifierSeeker(static_cast<Provider*>(this))
  {
  }

  public: virtual ~PlainRepository()
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

  public: void pushLevel(IdentifiableObject *obj)
  {
    this->stack.add(obj);
  }

  public: void pushLevel(Char const *scope, IdentifiableObject *obj)
  {
    this->stack.add(scope, obj);
  }

  public: void popLevel();

  public: void setLevel(IdentifiableObject *obj, Int index = -1);

  public: void setLevel(Char const *scope, IdentifiableObject *obj, Int index = -1);

  public: IdentifiableObject* getLevelData(Int index = -1) const;

  public: const SbStr& getLevelScope(Int index = -1) const;

  /**
   * @brief Copy the content of another stack into this stack.
   * This will wipe all contents currently in the stack.
   */
  public: void copyFrom(PlainRepository const *src);

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

  /// @}

  /// @name Branching Functions
  /// @{

  public: void setBranchingInfo(PlainRepository *ds, Int ti);

  public: PlainRepository* getTrunkRepository() const
  {
    return this->trunkRepo;
  }

  private: void ownTopLevel();

  /// @}

  /// @name PlainProvider Implementation
  /// @{

  public: virtual void setPlainValue(Reference const *ref, IdentifiableObject *val);

  public: virtual void setPlainValue(Char const *qualifier, IdentifiableObject *val);

  public: virtual Bool trySetPlainValue(Reference const *ref, IdentifiableObject *val);

  public: virtual Bool trySetPlainValue(Char const *qualifier, IdentifiableObject *val);

  /// @}

  /// @name Provider Implementation
  /// @{

  public: virtual void removeValue(Reference const *ref);

  public: virtual void removeValue(Char const *qualifier);

  public: virtual Bool tryRemoveValue(Reference const *ref);

  public: virtual Bool tryRemoveValue(Char const *qualifier);

  public: virtual IdentifiableObject* getPlainValue(Reference const *ref) const;

  public: virtual void getPlainValue(Reference const *ref, PlainModulePairedPtr &retVal);

  public: virtual IdentifiableObject* getPlainValue(Char const *qualifier) const;

  public: virtual void getPlainValue(Char const *qualifier, PlainModulePairedPtr &retVal);

  public: virtual Bool tryGetPlainValue(Reference const *ref, IdentifiableObject *&retVal) const;

  public: virtual Bool tryGetPlainValue(Reference const *ref, PlainModulePairedPtr &retVal);

  public: virtual Bool tryGetPlainValue(Char const *qualifier, IdentifiableObject *&retVal) const;

  public: virtual Bool tryGetPlainValue(Char const *qualifier, PlainModulePairedPtr &retVal);

  /// @}

}; // class

} } // namespace

#endif
