/**
 * @file Core/Data/ParsingDimension.h
 * Contains the header of class Core::Data::ParsingDimension.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_PARSINGDIMENSION_H
#define CORE_DATA_PARSINGDIMENSION_H

namespace Core { namespace Data
{

class ParsingDimension : public Node
{
  //============================================================================
  // Type Info

  TYPE_INFO(ParsingDimension, Node, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: SharedPtr<Integer> entryTokenId;
  
  private: SharedPtr<String> entryTokenText;
  
  private: SharedPtr<Reference> startRef;


  //============================================================================
  // Constructor / Destructor

  public: ParsingDimension(
    SharedPtr<String> const &ett=SharedPtr<String>(),
    SharedPtr<Reference> const &sr=SharedPtr<Reference>()
  ) : entryTokenText(ett), startRef(sr)
  {
    if (this->entryTokenText != 0) this->entryTokenText->setOwner(this);
    if (this->startRef != 0) this->startRef->setOwner(this);
  }

  public: virtual ~ParsingDimension()
  {
    RESET_OWNED_SHAREDPTR(this->entryTokenId);
    RESET_OWNED_SHAREDPTR(this->entryTokenText);
    RESET_OWNED_SHAREDPTR(this->startRef);
  }

  public: static SharedPtr<ParsingDimension> create(
    SharedPtr<String> const &ett=SharedPtr<String>(),
    SharedPtr<Reference> const &sr=SharedPtr<Reference>()
  ) {
    return std::make_shared<ParsingDimension>(ett, sr);
  }


  //============================================================================
  // Member Variables

  public: void setEntryTokenId(SharedPtr<Integer> const &id)
  {
    UPDATE_OWNED_SHAREDPTR(this->entryTokenId, id);
  }

  public: SharedPtr<Integer> const& getEntryTokenId() const
  {
    return this->entryTokenId;
  }

  public: void setEntryTokenText(SharedPtr<String> const &text)
  {
    UPDATE_OWNED_SHAREDPTR(this->entryTokenText, text);
  }

  public: SharedPtr<String> const& getEntryTokenText() const
  {
    return this->entryTokenText;
  }

  public: void setStartRef(SharedPtr<Reference> const &sr)
  {
    UPDATE_OWNED_SHAREDPTR(this->startRef, sr);
  }

  public: SharedPtr<Reference> const& getStartRef() const
  {
    return this->startRef;
  }

}; // class

} } // namespace

#endif
