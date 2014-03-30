/**
 * @file Core/Lexer/TokenDefinitionList.h
 * Contains the header of class Lexer::TokenDefinitionList.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_TOKEN_DEFINITION_LIST_H
#define LEXER_TOKEN_DEFINITION_LIST_H

namespace Core { namespace Lexer
{

/**
 * @brief This class holds and manages token definitions.
 * @ingroup parser
 *
 * This list stores the token definitions in an array of smart pointers.
 */
class TokenDefinitionList : public SignalReceiver
{
  //============================================================================
  // Type Info

  TYPE_INFO(TokenDefinitionList, SignalReceiver, "Core.Lexer", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  /// The list of token definition pointers.
  private: std::vector<SharedPtr<TokenDefinition> > list;


  //============================================================================
  // Signals

  /**
   * @brief A signal for enquiring whether the given token is in use.
   *
   * This signal is fired before removing any token from the list. This
   * is to make sure no one is still using this token. Anyone handling
   * this signal should return true if he's using the token with the
   * provided id.
   */
  public: SIGNAL_WITH_ACK(tokenInUseInquirer, (Word tokenId), (tokenId));


  //============================================================================
  // Constructor & Destructor

  public: TokenDefinitionList()
  {
  }

  /// Release all token definitions.
  public: ~TokenDefinitionList()
  {
    this->release();
  }


  //============================================================================
  // Member Functions

  /// @name List Manipulation
  /// @{

  /// Add the given token definition to the list.
  public: Int add(const SharedPtr<TokenDefinition> &ptr);

  /// Create a new token definition and add it to the list.
  public: Int add(Word id, const SharedPtr<Term> &formulaHead, Bool internal=false);

  /// Create a new token definition and add it to the list.
  public: Int add(Word id, Term *formulaHead, Bool internal=false)
  {
    return this->add(id, SharedPtr<Term>(formulaHead), internal);
  }

  /// Create a new token definition for a constant token.
  public: Word addConstToken(const Char *str);

  /// Remove the token definition with the given index.
  public: void remove(Int index);

  /// Remove the token definition with the given id from the list.
  public: void removeById(Word id);

  /// Remove all token definitions from the list.
  public: void release();

  /// Clear all references to a specific token id.
  private: void clearAllReferences(Word tokenId);

  /// Clear all references of a given term tree.
  private: static void clearTreeReferences(Term *term, Word tokenId=UNKNOWN_ID);

  /// @}

  /// @name Retrieval and Search Operations
  /// @{

  /// Get the count of token definitions in the list.
  public: Word getCount()
  {
    return this->list.size();
  }

  /// Get the token definition with the given index.
  public: const SharedPtr<TokenDefinition>& get(Int index)
  {
    if (static_cast<Word>(index) >= this->list.size()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Lexer::TokenDefinitionList::get"),
                                     STR("Index out of range."));
    }
    return this->list[index];
  }

  /// Find and return the token definition with the given id.
  public: const SharedPtr<TokenDefinition>& find(Word id)
  {
    static SharedPtr<TokenDefinition> null(0);
    Int index = this->findIndex(id);
    if (index == -1) return null;
    else return this->list[index];
  }

  /// Find the index of the token definition with the given id.
  public: Int findIndex(Word id);

  /// Binary search the list for the index of the given id.
  private: Int findIndex(Word id, Int startPos, Int endPos);

  /// Check whether the given char group id is referenced by anyone.
  public: Bool isCharGroupInUse(Word charGroupId);

  /// Check whether a char group is referenced by the given term tree.
  private: static Bool _isCharGroupInUse(const Term *term, Word charGroupId);

  /// @}

  /// @name Token Manipulation Operations
  /// @{

  /* TODO:
    /// Combine the provided token attachment with the current tokens.
    public: void combineTokenAttachment(Term *attachment, Int prodIds[]=0);

    /// Combine the provided token list with the current tokens.
    public: void combineTokenAttachments(std::vector<Term*> list);
    */

  /// @}

}; // class

} } // namespace

#endif
