/**
 * @file Core/Processing/ParserTermLevel.h
 * Contains the header of class Core::Processing::ParserTermLevel.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_PARSERTERMLEVEL_H
#define CORE_PROCESSING_PARSERTERMLEVEL_H

namespace Core::Processing
{

/**
 * @brief Defines one level in the state's hierarchy position stack.
 * @ingroup core_processing
 *
 * The ParserState object has a stack that defines the current parsing position within
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
  private: Data::Grammar::Term *term;

  private: TiObject *param1;

  private: TiObject *param2;

  private: TiObject *flags;


  //============================================================================
  // Constructors / Destructor

  /// Initialize all members to 0.
  public: ParserTermLevel() : posId(0), term(0), param1(0), param2(0), flags(0)
  {
  }

  /// Copy the state level and increment data users count.
  public: ParserTermLevel(const ParserTermLevel &level) : posId(level.getPosId()),
    term(level.getTerm()),
    param1(level.getParam1()),
    param2(level.getParam2()),
    flags(level.getFlags())
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
  protected: void setTerm(Data::Grammar::Term *t)
  {
    this->term = t;
  }

  /// Get the term object of this level.
  public: Data::Grammar::Term* getTerm() const
  {
    return this->term;
  }

  /**
   * @brief Get a first parameter for the level's term.
   * This is used for optimization purposes, to avoid tracing the term's
   * parameters everytime the level is revisited. This is needed mainly for
   * concat terms, alternate terms, as well as multiply terms.
   */
  protected: void setParam1(TiObject *p)
  {
    this->param1 = p;
  }
  public: TiObject* getParam1() const
  {
    return this->param1;
  }

  /**
   * @brief Set a second parameter for the level's term.
   * This is used for optimization purposes, to avoid tracing the term's
   * parameters everytime the level is revisited. This is needed mainly for
   * concat terms, alternate terms, as well as multiply terms.
   */
  protected: void setParam2(TiObject *p)
  {
    this->param2 = p;
  }
  public: TiObject* getParam2() const
  {
    return this->param2;
  }

  protected: void setFlags(TiObject *f)
  {
    this->flags = f;
  }
  public: TiObject* getFlags() const
  {
    return this->flags;
  }

  public: void copyFrom(ParserTermLevel *src)
  {
    this->setTerm(src->getTerm());
    this->setPosId(src->getPosId());
    this->param1 = src->getParam1();
    this->param2 = src->getParam2();
    this->flags = src->getFlags();
  }

}; // class

} // namespace

#endif
