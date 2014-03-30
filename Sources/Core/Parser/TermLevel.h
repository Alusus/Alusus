/**
 * @file Core/Parser/TermLevel.h
 * Contains the header of class Parser::TermLevel.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PARSER_TERM_LEVEL_H
#define PARSER_TERM_LEVEL_H

namespace Core { namespace Parser
{

/**
 * @brief Defines one level in the state's hierarchy position stack.
 * @ingroup parser
 *
 * The State object has a stack that defines the current parsing position within
 * the terms hierarchy. Each level in this stack is represented by a single
 * instance of this structure.
 */
class TermLevel
{
  //============================================================================
  // Friend Classes

  friend class State;


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
  private: IdentifiableObject *param1;

  /// @sa setParam2()
  private: IdentifiableObject *param2;

  /// @sa setParam3()
  private: IdentifiableObject *param3;

  /// Holds arbitrary data needed by the parsing handler.
  private: SharedPtr<IdentifiableObject> data;


  //============================================================================
  // Constructors / Destructor

  /// Initialize all members to 0.
  public: TermLevel() : posId(0), term(0), param1(0), param2(0), param3(0)
  {
  }

  /// Copy the state level and increment data users count.
  public: TermLevel(const TermLevel &level) : posId(level.getPosId()),
    term(level.getTerm()),
    param1(level.getParam1()),
    param2(level.getParam2()),
    param3(level.getParam3()),
    data(level.getData())
  {
  }

  public: ~TermLevel()
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
   * @brief Set a first parameter for the level's term.
   * This is used for optimization purposes, to avoid tracing the term's
   * parameters everytime the level is revisited. This is needed mainly for
   * concat terms, alternate terms, as well as multiply terms.
   */
  protected: void setParam1(IdentifiableObject *p)
  {
    this->param1 = p;
  }

  /// @sa setParam1()
  public: IdentifiableObject* getParam1() const
  {
    return this->param1;
  }

  /**
   * @brief Set a second parameter for the level's term.
   * This is used for optimization purposes, to avoid tracing the term's
   * parameters everytime the level is revisited. This is needed mainly for
   * concat terms, alternate terms, as well as multiply terms.
   */
  protected: void setParam2(IdentifiableObject *p)
  {
    this->param2 = p;
  }

  /// @sa setParam2()
  public: IdentifiableObject* getParam2() const
  {
    return this->param2;
  }

  /**
   * @brief Set a third parameter for the level's term.
   * This is used for optimization purposes, to avoid tracing the term's
   * parameters everytime the level is revisited. This is needed mainly for
   * concat terms, alternate terms, as well as multiply terms.
   */
  protected: void setParam3(IdentifiableObject *p)
  {
    this->param3 = p;
  }

  /// @sa setParam3()
  public: IdentifiableObject* getParam3() const
  {
    return this->param3;
  }

  /**
   * @brief Set the parsing data associated with this level.
   * This is an arbitrary data needed by the parsing handler.
   */
  public: void setData(const SharedPtr<IdentifiableObject> &d)
  {
    this->data = d;
  }

  /// Get a (smart) pointer to the data associated with this level.
  public: const SharedPtr<IdentifiableObject>& getData() const
  {
    return this->data;
  }

  public: Bool isDataShared()
  {
    return !this->data.unique();
  }

  public: void copyFrom(TermLevel *src)
  {
    this->setTerm(src->getTerm());
    this->setPosId(src->getPosId());
    this->setParam1(src->getParam1());
    this->setParam2(src->getParam2());
    this->setParam3(src->getParam3());
    this->setData(src->getData());
  }

}; // class

} } // namespace

#endif
