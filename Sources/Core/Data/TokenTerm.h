/**
 * @file Core/Data/TokenTerm.h
 * Contains the header of class Data::TokenTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_TOKEN_TERM_H
#define DATA_TOKEN_TERM_H

namespace Core { namespace Data
{

// TODO: DOC

class TokenTerm : public Term, public virtual DataOwner
{
  //============================================================================
  // Type Info

  TYPE_INFO(TokenTerm, Term, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(Term, DataOwner);


  //============================================================================
  // Member Variables

  private: SharedPtr<IdentifiableObject> tokenId;

  private: SharedPtr<IdentifiableObject> tokenText;


  //============================================================================
  // Constructor & Destructor

  public: TokenTerm(Word f=0, const SharedPtr<IdentifiableObject> &id=SharedPtr<IdentifiableObject>(),
                    const SharedPtr<IdentifiableObject> &text=SharedPtr<IdentifiableObject>()) :
    Term(f), tokenId(id), tokenText(text)
  {
    if (id != 0 && !id->isA<Integer>() && !id->isA<Reference>()) {
      throw InvalidArgumentException(STR("id"), STR("Core::Data::TokenTerm::TokenTerm"),
                                     STR("Must be of type Integer or Reference."));
    }
    if (text != 0 && !text->isA<String>() && !text->isA<Map>() && !text->isA<Reference>()) {
      throw InvalidArgumentException(STR("text"), STR("Core::Data::TokenTerm::TokenTerm"),
                                     STR("Must be of type String or Reference."));
    }
  }

  public: TokenTerm(Word f, Int id, const Char *text=0) :
    Term(f), tokenId(std::make_shared<Integer>(id))
  {
    if (text != 0) this->tokenText = std::make_shared<String>(text);
  }

  public: TokenTerm(Word f, const Char *text) :
    Term(f), tokenText(std::make_shared<String>(text))
  {
  }

  public: virtual ~TokenTerm()
  {
  }

  public: static SharedPtr<TokenTerm> create(Word f=0,
                                             const SharedPtr<IdentifiableObject> &id=SharedPtr<IdentifiableObject>(),
                                             const SharedPtr<IdentifiableObject> &text=SharedPtr<IdentifiableObject>())
  {
    return std::make_shared<TokenTerm>(f, id, text);
  }

  public: static SharedPtr<TokenTerm> create(Word f, Int id, const Char *text=0)
  {
    return std::make_shared<TokenTerm>(f, id, text);
  }

  public: static SharedPtr<TokenTerm> create(Word f, const Char *text)
  {
    return std::make_shared<TokenTerm>(f, text);
  }


  ////////////////////////////////////////////////////////////////////////////
  // Member Functions

  public: void setTokenId(const SharedPtr<IdentifiableObject> &id)
  {
    if (id != 0 && !id->isA<Integer>() && !id->isA<Reference>()) {
      throw InvalidArgumentException(STR("s"), STR("Core::Data::TokenTerm::setTokenId"),
                                     STR("Must be of type Integer or Reference."));
    }
    this->tokenId = id;
  }

  public: const SharedPtr<IdentifiableObject>& getTokenId() const
  {
    return this->tokenId;
  }

  public: void setTokenText(const SharedPtr<IdentifiableObject> &text)
  {
    if (text != 0 && !text->isA<String>() && !text->isA<Map>() && !text->isA<Reference>()) {
      throw InvalidArgumentException(STR("text"), STR("Core::Data::TokenTerm::setTokenText"),
                                     STR("Must be of type String or Reference."));
    }
    this->tokenText = text;
  }

  public: const SharedPtr<IdentifiableObject>& getTokenText() const
  {
    return this->tokenText;
  }


  //============================================================================
  // DataOwner Implementation

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    if (this->tokenId != 0) Data::unsetIndexes(this->tokenId.get(), from, to);
    if (this->tokenText != 0) Data::unsetIndexes(this->tokenText.get(), from, to);
  }

}; // class

} } // namespace

#endif
