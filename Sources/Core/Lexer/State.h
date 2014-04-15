/**
 * @file Core/Lexer/State.h
 * Contains the header of class Core::Lexer::State.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_STATE_H
#define LEXER_STATE_H

namespace Core { namespace Lexer
{

/**
 * @brief Contains state information used by the state machine.
 * @ingroup lexer
 *
 * This class contains information that defines a single state in the state
 * machine. The information contained specify where within the formula's terms
 * hierarchy the state is at. The object contains a stack of index entries,
 * each of which specifies the state at a certain level within the hierarchy.
 */
class State
{
  //============================================================================
  // Member Variables

  /**
   * @brief The stack of indexes that defines the current state.
   *
   * Each entry in this stack defines the position within one level of the
   * terms hierarchy. The first entry is used to specify the token definition
   * used by this index (the index within the list of token definitions
   * defined in the lexer).
   */
  private: std::vector<Int> indexStack;

  /**
   * @brief The length of the token.
   *
   * This variable holds the length of the token if the state has reached an
   * end, or 0 if the state is still active.
   */
  private: Int tokenLength;


  //============================================================================
  // Constructors / Destructor

  public: State() : tokenLength(0)
  {
  }

  /// Initializes the object's value from another object.
  public: State(const State &src)
  {
    this->copyFrom(&src);
  }

  public: ~State()
  {
  }


  //============================================================================
  // Operators

  /// Copies the values of the object from another object.
  public: State & operator=(const State &src)
  {
    this->copyFrom(&src);
    return *this;
  }


  //============================================================================
  // Member Functions

  /// Get the state index stack.
  public: std::vector<Int> * getIndexStack()
  {
    return &this->indexStack;
  }

  /// Get the size of the state index stack.
  public: Int getIndexStackSize() const
  {
    return this->indexStack.size();
  }

  /// Get an entry in the state index stack
  public: Int getIndexStackEntry(Int i) const
  {
    return this->indexStack[i];
  }

  /// Set the token length.
  public: void setTokenLength(Int length)
  {
    this->tokenLength = length;
  }

  /**
   * @brief Get the token length value.
   *
   * The length of the token is set when the state reaches an end (no more
   * characters could be accepted to be part of the token). If the state
   * hasn't yet reached an end, this value is 0.
   */
  public: Int getTokenLength() const
  {
    return this->tokenLength;
  }

  /// Copies the values of the object from another object.
  private: void copyFrom(const State *src);

}; // class

} } // namespace

#endif
