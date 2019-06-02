/**
 * @file Core/Processing/LexerState.h
 * Contains the header of class Core::Processing::LexerState.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_LEXERSTATE_H
#define CORE_PROCESSING_LEXERSTATE_H

namespace Core { namespace Processing
{

/**
 * @brief Contains state information used by the state machine.
 * @ingroup core_processing
 *
 * This class contains information that defines a single state in the state
 * machine. The information contained specify where within the formula's terms
 * hierarchy the state is at. The object contains a stack of index entries,
 * each of which specifies the state at a certain level within the hierarchy.
 */
class LexerState
{
  //============================================================================
  // Types

  public: struct Level
  {
    public: Int posId;
    public: Data::Grammar::Term *term;
    Level() : posId(0), term(0) {}
    Level(Int id, Data::Grammar::Term *t) : posId(id), term(t) {}
  };


  //============================================================================
  // Member Variables

  private: Int tokenDefIndex;

  private: std::vector<Level> levels;

  /**
   * @brief The length of the token.
   *
   * This variable holds the length of the token if the state has reached an
   * end, or 0 if the state is still active.
   */
  private: Int tokenLength;


  //============================================================================
  // Constructors / Destructor

  public: LexerState() : tokenLength(0), tokenDefIndex(-1)
  {
  }

  /// Initializes the object's value from another object.
  public: LexerState(const LexerState &src)
  {
    this->copyFrom(&src);
  }

  public: ~LexerState()
  {
  }


  //============================================================================
  // Operators

  /// Copies the values of the object from another object.
  public: LexerState & operator=(const LexerState &src)
  {
    this->copyFrom(&src);
    return *this;
  }


  //============================================================================
  // Member Functions

  public: void setTokenDefIndex(Int i)
  {
    this->tokenDefIndex = i;
  }

  public: Int getTokenDefIndex() const
  {
    return this->tokenDefIndex;
  }

  public: void pushTermLevel(Int posId, Data::Grammar::Term *term)
  {
    this->levels.push_back(Level(posId, term));
  }

  public: void popTermLevel()
  {
    this->levels.pop_back();
  }

  public: void resizeLevels(Word size)
  {
    this->levels.resize(size);
  }

  /// Get the size of the levels stack.
  public: Word getLevelCount() const
  {
    return this->levels.size();
  }

  /// Get an entry in the levels stack.
  public: Level& refLevel(Int i)
  {
    return this->levels[i];
  }

  public: Level const& refLevel(Int i) const
  {
    return this->levels[i];
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
  public: void copyFrom(LexerState const *src);

}; // class

} } // namespace

#endif
