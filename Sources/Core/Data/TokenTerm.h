/**
 * @file Core/Data/TokenTerm.h
 * Contains the header of class Core::Data::TokenTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_TOKENTERM_H
#define DATA_TOKENTERM_H

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

  private: SharedPtr<Node> tokenId;

  private: SharedPtr<Node> tokenText;


  //============================================================================
  // Constructor & Destructor

  public: TokenTerm(Word f=0, SharedPtr<Node> const &id=SharedPtr<Node>(),
                    SharedPtr<Node> const &text=SharedPtr<Node>()) :
    Term(f), tokenId(id), tokenText(text)
  {
    if (id != 0 && !id->isA<Integer>() && !id->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, STR("id"), STR("Must be of type Integer or Reference."));
    }
    if (text != 0 && !text->isA<String>() && !text->isA<SharedMap>() && !text->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, STR("text"), STR("Must be of type String or Reference."));
    }
    if (this->tokenId != 0) this->tokenId->setOwner(this);
    if (this->tokenText != 0) this->tokenText->setOwner(this);
  }

  public: TokenTerm(Word f, Word id, Char const *text=0) :
    Term(f), tokenId(std::make_shared<Integer>(id))
  {
    this->tokenId->setOwner(this);
    if (text != 0) {
      UPDATE_OWNED_SHAREDPTR(this->tokenText, std::make_shared<String>(text));
    }
  }

  public: TokenTerm(Word f, Char const *text) :
    Term(f), tokenText(std::make_shared<String>(text))
  {
    this->tokenText->setOwner(this);
  }

  public: virtual ~TokenTerm()
  {
    RESET_OWNED_SHAREDPTR(this->tokenId);
    RESET_OWNED_SHAREDPTR(this->tokenText);
  }

  public: static SharedPtr<TokenTerm> create(Word f=0,
                                             SharedPtr<Node> const &id=SharedPtr<Node>(),
                                             SharedPtr<Node> const &text=SharedPtr<Node>())
  {
    return std::make_shared<TokenTerm>(f, id, text);
  }

  public: static SharedPtr<TokenTerm> create(Word f, Word id, Char const *text=0)
  {
    return std::make_shared<TokenTerm>(f, id, text);
  }

  public: static SharedPtr<TokenTerm> create(Word f, Char const *text)
  {
    return std::make_shared<TokenTerm>(f, text);
  }


  //============================================================================
  // Member Functions

  public: void setTokenId(SharedPtr<Node> const &id)
  {
    if (id != 0 && !id->isA<Integer>() && !id->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, STR("s"), STR("Must be of type Integer or Reference."));
    }
    UPDATE_OWNED_SHAREDPTR(this->tokenId, id);
  }

  public: SharedPtr<Node> const& getTokenId() const
  {
    return this->tokenId;
  }

  public: void setTokenText(SharedPtr<Node> const &text)
  {
    if (text != 0 && !text->isA<String>() && !text->isA<SharedMap>() && !text->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, STR("text"), STR("Must be of type String or Reference."));
    }
    UPDATE_OWNED_SHAREDPTR(this->tokenText, text);
  }

  public: SharedPtr<Node> const& getTokenText() const
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
