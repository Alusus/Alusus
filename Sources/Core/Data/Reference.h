/**
 * @file Core/Data/Reference.h
 * Contains the header of class Core::Data::Reference.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_REFERENCE_H
#define DATA_REFERENCE_H

namespace Core { namespace Data
{

// TODO: DOC

/**
 * @brief A reference to a variable.
 * @ingroup data
 */
class Reference : public IdentifiableObject, public virtual DataOwner
{
  //============================================================================
  // Type Info

  TYPE_INFO(Reference, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(IdentifiableObject, DataOwner);


  //============================================================================
  // Member Variables

  private: ReferenceScope scope;

  private: SharedPtr<ReferenceSegment> segment;


  //============================================================================
  // Constructor

  /**
     * @brief Initializes the id with a given object.
     * @sa setId()
     */
  public: Reference(const SharedPtr<ReferenceSegment> &seg=SharedPtr<ReferenceSegment>(),
                    ReferenceScope t=ReferenceScope::UNKNOWN) : segment(seg), scope(t)
  {
  }

  public: virtual ~Reference()
  {
  }


  //============================================================================
  // Operators

  /// @sa compare()
  public: Bool operator==(const Reference &r) const
  {
    return this->compare(&r) == 0;
  }

  public: Bool operator==(const SharedPtr<Reference> &r) const
  {
    return this->compare(r.get()) == 0;
  }


  //============================================================================
  // Member Functions

  public: void setSegment(const SharedPtr<ReferenceSegment> &seg)
  {
    this->segment = seg;
  }

  public: const SharedPtr<ReferenceSegment>& getSegment() const
  {
    return this->segment;
  }

  /// Return the last segment in the chain.
  public: const SharedPtr<ReferenceSegment>& getLastSegment();

  public: void setScope(ReferenceScope s)
  {
    this->scope = s;
  }

  public: ReferenceScope getScope() const
  {
    return this->scope;
  }

  /// Compares this reference with another reference.
  public: Bool compare(const Reference *r) const;


  //============================================================================
  // DataOwner Implementation

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    if (this->segment != 0) this->segment->unsetIndexes(from, to);
  }

}; // class

} } // namespace

#endif
