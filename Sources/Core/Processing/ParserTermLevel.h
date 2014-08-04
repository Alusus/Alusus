/**
 * @file Core/Processing/ParserTermLevel.h
 * Contains the header of class Core::Processing::ParserTermLevel.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_PARSERTERMLEVEL_H
#define PROCESSING_PARSERTERMLEVEL_H

namespace Core { namespace Processing
{

/**
 * @brief Defines one level in the state's hierarchy position stack.
 * @ingroup processing
 *
 * The State object has a stack that defines the current parsing position within
 * the terms hierarchy. Each level in this stack is represented by a single
 * instance of this structure.
 */
class ParserTermLevel
{
  //============================================================================
  // Friend Classes

  friend class ParserState;


  //============================================================================
  // Member Variables

  /**
   * @brief Defines where at this level the state machine is standing.
   *
   * The meaning of the number returned depends on the term using it.
   */
  private: Word posId;

  /// Pointer to the current level's term object.
  private: Data::Term *term;

  /// @sa setParam1()
  private: Data::PlainModulePairedPtr param1;

  /// @sa setParam2()
  private: Data::PlainModulePairedPtr param2;

  /// @sa setParam3()
  private: Data::PlainModulePairedPtr param3;


  //============================================================================
  // Constructors / Destructor

  /// Initialize all members to 0.
  public: ParserTermLevel() : posId(0), term(0)
  {
  }

  /// Copy the state level and increment data users count.
  public: ParserTermLevel(const ParserTermLevel &level) : posId(level.getPosId()),
    term(level.getTerm()),
    param1(*(level.getParam1())),
    param2(*(level.getParam2())),
    param3(*(level.getParam3()))
  {
  }

  public: ~ParserTermLevel()
  {
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Set the identifier of ths current position within this level.
   *
   * The meaning of the number returned depends on the term using it.
   */
  protected: void setPosId(Word i)
  {
    this->posId = i;
  }

  /**
   * @brief Get the identifier of the current position within this level.
   *
   * The meaning of the number returned depends on the term using it.
   */
  public: Word getPosId() const
  {
    return this->posId;
  }

  /// Set the term object of this level.
  protected: void setTerm(Data::Term *t)
  {
    this->term = t;
  }

  /// Get the term object of this level.
  public: Data::Term* getTerm() const
  {
    return this->term;
  }

  /**
   * @brief Get a first parameter for the level's term.
   * This is used for optimization purposes, to avoid tracing the term's
   * parameters everytime the level is revisited. This is needed mainly for
   * concat terms, alternate terms, as well as multiply terms.
   */
  protected: Data::PlainModulePairedPtr* getParam1()
  {
    return &this->param1;
  }
  protected: Data::PlainModulePairedPtr const* getParam1() const
  {
    return &this->param1;
  }

  /**
   * @brief Set a second parameter for the level's term.
   * This is used for optimization purposes, to avoid tracing the term's
   * parameters everytime the level is revisited. This is needed mainly for
   * concat terms, alternate terms, as well as multiply terms.
   */
  protected: Data::PlainModulePairedPtr* getParam2()
  {
    return &this->param2;
  }
  protected: Data::PlainModulePairedPtr const* getParam2() const
  {
    return &this->param2;
  }

  /**
   * @brief Set a third parameter for the level's term.
   * This is used for optimization purposes, to avoid tracing the term's
   * parameters everytime the level is revisited. This is needed mainly for
   * concat terms, alternate terms, as well as multiply terms.
   */
  protected: Data::PlainModulePairedPtr* getParam3()
  {
    return &this->param3;
  }
  protected: Data::PlainModulePairedPtr const* getParam3() const
  {
    return &this->param3;
  }

  public: void copyFrom(ParserTermLevel *src)
  {
    this->setTerm(src->getTerm());
    this->setPosId(src->getPosId());
    this->param1 = *src->getParam1();
    this->param2 = *src->getParam2();
    this->param3 = *src->getParam3();
  }

}; // class

} } // namespace

#endif
