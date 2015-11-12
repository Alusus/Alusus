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
class PlainRepository : public IdentifiableObject, public virtual Provider
{
  //============================================================================
  // Type Info

  TYPE_INFO(PlainRepository, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(IdentifiableObject, Provider);


  //============================================================================
  // Member Variables

  private: PlainNamedList stack;
  private: Bool owningEnabled;

  private: PlainRepository *trunkRepo;
  private: Int trunkIndex;

  protected: ReferenceSeeker referenceSeeker;
  protected: QualifierSeeker qualifierSeeker;


  //============================================================================
  // Constructor & Destructor

  public: PlainRepository() : stack(false), owningEnabled(false), trunkRepo(0), trunkIndex(-1),
    referenceSeeker(static_cast<Provider*>(this)), qualifierSeeker(static_cast<Provider*>(this))
  {
  }

  public: PlainRepository(Word maxScopeNameSize, Word reservedCount) :
    stack(maxScopeNameSize, reservedCount, false), owningEnabled(false), trunkRepo(0), trunkIndex(-1),
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

  public: void setOwningEnabled(Bool e)
  {
    this->owningEnabled = e;
  }

  public: Bool isOwningEnabled() const
  {
    return this->owningEnabled;
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

  /// @name Provider Implementation
  /// @{

  public: using Provider::set;
  public: using Provider::remove;

  public: virtual void set(Reference const *ref, SeekerSetLambda handler);

  public: virtual void set(Char const *qualifier, SeekerSetLambda handler);

  public: virtual void remove(Reference const *ref, SeekerRemoveLambda handler);

  public: virtual void remove(Char const *qualifier, SeekerRemoveLambda handler);

  public: virtual void forEach(Reference const *ref, SeekerForeachLambda handler,
                               TypeInfo const *parentTypeInfo=0);

  public: virtual void forEach(Char const *qualifier, SeekerForeachLambda handler,
                               TypeInfo const *parentTypeInfo=0);

  /// @}

}; // class

} } // namespace

#endif
