/**
 * @file Core/Data/Grammar/ParsingDimension.h
 * Contains the header of class Core::Data::Grammar::ParsingDimension.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_PARSINGDIMENSION_H
#define CORE_DATA_GRAMMAR_PARSINGDIMENSION_H

namespace Core::Data::Grammar
{

class ParsingDimension : public Node, public Binding
{
  //============================================================================
  // Type Info

  TYPE_INFO(ParsingDimension, Node, "Core.Data.Grammar", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(Binding)
  ));


  //============================================================================
  // Member Variables

  private: SharedPtr<TiInt> entryTokenId;

  private: SharedPtr<TiStr> entryTokenText;

  private: SharedPtr<Reference> startRef;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (entryTokenId, TiInt, SHARED_REF, setEntryTokenId(value), entryTokenId.get()),
    (entryTokenText, TiStr, SHARED_REF, setEntryTokenText(value), entryTokenText.get()),
    (startRef, Reference, SHARED_REF, setStartRef(value), startRef.get())
  );


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ParsingDimension);

  IMPLEMENT_ATTR_CONSTRUCTOR(ParsingDimension);

  public: virtual ~ParsingDimension()
  {
    RESET_OWNED_SHAREDPTR(this->startRef);
  }


  //============================================================================
  // Member Variables

  public: void setEntryTokenId(SharedPtr<TiInt> const &id)
  {
    this->entryTokenId = id;
  }

  private: void setEntryTokenId(TiInt *id)
  {
    this->setEntryTokenId(getSharedPtr(id));
  }

  public: SharedPtr<TiInt> const& getEntryTokenId() const
  {
    return this->entryTokenId;
  }

  public: void setEntryTokenText(SharedPtr<TiStr> const &text)
  {
    this->entryTokenText = text;
  }

  private: void setEntryTokenText(TiStr *text)
  {
    this->setEntryTokenText(getSharedPtr(text));
  }

  public: SharedPtr<TiStr> const& getEntryTokenText() const
  {
    return this->entryTokenText;
  }

  public: void setStartRef(SharedPtr<Reference> const &sr)
  {
    UPDATE_OWNED_SHAREDPTR(this->startRef, sr);
  }

  private: void setStartRef(Reference *sr)
  {
    this->setStartRef(getSharedPtr(sr));
  }

  public: SharedPtr<Reference> const& getStartRef() const
  {
    return this->startRef;
  }

}; // class

} // namespace

#endif
