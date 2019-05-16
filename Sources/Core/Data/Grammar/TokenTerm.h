/**
 * @file Core/Data/Grammar/TokenTerm.h
 * Contains the header of class Core::Data::Grammar::TokenTerm.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_TOKENTERM_H
#define CORE_DATA_GRAMMAR_TOKENTERM_H

namespace Core::Data::Grammar
{

// TODO: DOC

class TokenTerm : public Term, public DataHaving
{
  //============================================================================
  // Type Info

  TYPE_INFO(TokenTerm, Term, "Core.Data.Grammar", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(DataHaving)
  ));


  //============================================================================
  // Member Variables

  private: TioSharedPtr tokenId;

  private: TioSharedPtr tokenText;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Term,
    (tokenId, TiObject, SHARED_REF, setTokenId(value), tokenId.get()),
    (tokenText, TiObject, SHARED_REF, setTokenText(value), tokenText.get())
  );


  //============================================================================
  // Constructor & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(TokenTerm);

  IMPLEMENT_ATTR_CONSTRUCTOR(TokenTerm);

  public: virtual ~TokenTerm()
  {
  }


  //============================================================================
  // Member Functions

  public: void setTokenId(SharedPtr<TiObject> const &id)
  {
    if (id != 0 && !id->isA<TiInt>() && !id->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, S("s"), S("Must be of type TiInt or Reference."));
    }
    this->tokenId = id;
  }

  private: void setTokenId(TiObject *id)
  {
    this->setTokenId(getSharedPtr(id));
  }

  public: SharedPtr<TiObject> const& getTokenId() const
  {
    return this->tokenId;
  }

  public: void setTokenText(SharedPtr<TiObject> const &text)
  {
    if (text != 0 && !text->isA<TiStr>() && !text->isA<Map>() && !text->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, S("text"), S("Must be of type TiStr or Reference."));
    }
    this->tokenText = text;
  }

  private: void setTokenText(TiObject *text)
  {
    this->setTokenText(getSharedPtr(text));
  }

  public: SharedPtr<TiObject> const& getTokenText() const
  {
    return this->tokenText;
  }


  //============================================================================
  // DataHaving Implementation

  /// @sa DataHaving::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    if (this->tokenId != 0) Data::unsetIndexes(this->tokenId.get(), from, to);
    if (this->tokenText != 0) Data::unsetIndexes(this->tokenText.get(), from, to);
  }

}; // class

} // namespace

#endif
