/**
 * @file Core/Data/ConcatTerm.h
 * Contains the header of class Data::ConcatTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//============================================================================

#ifndef DATA_CONCAT_TERM_H
#define DATA_CONCAT_TERM_H

namespace Core { namespace Data
{

// TODO: Check the ownership of objects during the constructor. Will the objects be properly deleted in the destructor
//       in the case of an exception?

/**
 * @brief Handles concat grammar terms.
 * @ingroup data_terms
 *
 * This class handles concatenation in grammar formulas.
 */
class ConcatTerm : public ListTerm
{
  //============================================================================
  // Type Info

  TYPE_INFO(ConcatTerm, ListTerm, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  /// @sa setErrorSyncPosId()
  private: Int errorSyncPosId;


  //============================================================================
  // Constructors

  /// Initialize an empty concat term.
  public: ConcatTerm() : errorSyncPosId(-1) {}

  /**
   * @brief Initialize the object with either a static list or a dynamic list.
   * In addition to the arguments ListTerm expects, this constructor also
   * expects the following optional argument:<br/>
   * ESPI: Error sync position id. This position id is used by the parser to
   *       sync with new statements in case of syntax errors.
   * @sa ListTerm::ListTerm()
   */
  public: ConcatTerm(const std::initializer_list<Argument<TermElement>> &args) : ListTerm(args)
  {
    const Argument<TermElement> *arg;
    if ((arg = findArgument(args, TermElement(TermElement::ESPI))) != 0) {
      errorSyncPosId = arg->intVal;
    } else errorSyncPosId = -1;
  }

  public: virtual ~ConcatTerm()
  {
  }

  public: static SharedPtr<ConcatTerm> create(const std::initializer_list<Argument<TermElement>> &args)
  {
    return std::make_shared<ConcatTerm>(args);
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Set the position id to switch to in case of syntax errors.
   *
   * When a syntax error occures, the parser tries to skip the statement in
   * which the error occured. To skip that statement, it searches for the
   * place which denotes the end of the statement and sits their waiting for
   * the correct token to reach in order to help synchronizing the tokens
   * with the grammar again. The position where the parser should wait is
   * marked by this variable. If this value is -1, then the end of the
   * statement can't be found in this concat term and the parser should move
   * up in the tree and keep looking.<br>
   * The value should be equal to the 0 based index of the term that marks the
   * end of statement (for example the index of the ; token).
   */
  public: void setErrorSyncPosId(Int id)
  {
    this->errorSyncPosId = id;
  }

  /**
   * @brief Get the position id to switch to in case of syntax errors.
   *
   * When a syntax error occures, the parser tries to skip the statement in
   * which the error occured. To skip that statement, it searches for the
   * place which denotes the end of the statement and sits their waiting for
   * the correct token to reach in order to help synchronizing the tokens
   * with the grammar again. The position where the parser should wait is
   * marked by this variable. If this value is -1, then the end of the
   * statement can't be found in this concat term and the parser should move
   * up in the tree and keep looking.<br>
   * The value should be equal to the 0 based index of the term that marks the
   * end of statement (for example the index of the ; token).
   */
  public: Int getErrorSyncPosId() const
  {
    return this->errorSyncPosId;
  }

}; // class

} } // namespace

#endif
