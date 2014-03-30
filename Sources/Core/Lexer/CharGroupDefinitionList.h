/**
 * @file Core/Lexer/CharGroupDefinitionList.h
 * Contains the header of class Lexer::CharGroupDefinitionList.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_CHAR_GROUP_DEFINITION_LIST_H
#define LEXER_CHAR_GROUP_DEFINITION_LIST_H

namespace Core { namespace Lexer
{

/**
 * @brief This class holds and manages char group definitions.
 * @ingroup parser
 *
 * This list stores the char group definitions in an array of smart pointers.
 */
class CharGroupDefinitionList
{
  //============================================================================
  // Member Variables

  /// The list of char group definition pointers.
  private: std::vector<SharedPtr<CharGroupDefinition> > list;


  //============================================================================
  // Signals

  /**
     * @brief A signal for enquiring whether the given char group is in use.
     *
     * This signal is fired before removing any char group from the list. This
     * is to make sure no one is still using this char group. Anyone handling
     * this signal should return true if he's using the char group with the
     * provided id.
     */
  public: SIGNAL_WITH_ACK(charGroupInUseInquirer, (Word charGroupId), (charGroupId));


  //============================================================================
  // Constructor & Destructor

  public: CharGroupDefinitionList()
  {
  }

  /// Release all char group definitions.
  public: ~CharGroupDefinitionList()
  {
    this->release();
  }


  //============================================================================
  // Member Functions

  /// @name List Manipulation
  /// @{

  /// Add the given char group definition to the list.
  public: Int add(const SharedPtr<CharGroupDefinition> &ptr);

  /// Create a new char group definition and add it to the list.
  public: Int add(Word id, const SharedPtr<CharGroupUnit> &unit);

  /// Create a new char group definition and add it to the list.
  public: Int add(Word id, CharGroupUnit *unit)
  {
    return this->add(id, SharedPtr<CharGroupUnit>(unit));
  }

  /// Remove the char group definition with the given index.
  public: void remove(Int index);

  /// Remove the char group definition with the given id from the list.
  public: void removeById(Word id);

  /// Remove all char group definitions from the list.
  public: void release();

  /// @}

  /// @name Retrieval and Search Operations
  /// @{

  /// Get the count of char group definitions in the list.
  public: Word getCount()
  {
    return this->list.size();
  }

  /// Get the char group definition with the given index.
  public: const SharedPtr<CharGroupDefinition>& get(Int index)
  {
    if (static_cast<Word>(index) >= this->list.size()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Lexer::CharGroupDefinitionList::get"),
                                     STR("Index out of range."));
    }
    return this->list[index];
  }

  /// Find and return the char group definition with the given id.
  public: const SharedPtr<CharGroupDefinition>& find(Word id)
  {
    static SharedPtr<CharGroupDefinition> null(0);
    Int index = this->findIndex(id);
    if (index == -1) return null;
    else return this->list[index];
  }

  /// Find the index of the char group definition with the given id.
  public: Int findIndex(Word id);

  /// Binary search the list for the index of the given id.
  private: Int findIndex(Word id, Int startPos, Int endPos);

  /// @}

  /// @name Char Group Manipulation Operations
  /// @{

  /* TODO:
    /// Combine the provided char group attachment with the current char groups.
    public: void combineCharGroupAttachment(CharGroupUnit *attachment, Int prodIds[]=0);

    /// Combine the provided char group list with the current char groups.
    public: void combineCharGroupAttachments(std::vector<CharGroupUnit*> list);
    */

  /// @}

}; // class

} } // namespace

#endif
