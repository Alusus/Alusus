/**
 * @file Core/Data/Grammar/ConcatTerm.h
 * Contains the header of class Core::Data::Grammar::ConcatTerm.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//============================================================================

#ifndef CORE_DATA_GRAMMAR_CONCATTERM_H
#define CORE_DATA_GRAMMAR_CONCATTERM_H

namespace Core::Data::Grammar
{

// TODO: Check the ownership of objects during the constructor. Will the objects be properly deleted in the destructor
//       in the case of an exception?

/**
 * @brief Handles concat grammar terms.
 * @ingroup core_data_grammar
 *
 * This class handles concatenation in grammar formulas.
 */
class ConcatTerm : public ListTerm
{
  //============================================================================
  // Type Info

  TYPE_INFO(ConcatTerm, ListTerm, "Core.Data.Grammar", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  /// @sa setErrorSyncPosId()
  private: TiInt errorSyncPosId = -1;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(ListTerm,
    (errorSyncPosId, TiInt, VALUE, setErrorSyncPosId(value), &errorSyncPosId)
  );


  //============================================================================
  // Constructors

  IMPLEMENT_EMPTY_CONSTRUCTOR(ConcatTerm);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(ConcatTerm, this->validate());

  public: virtual ~ConcatTerm()
  {
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
  public: void setErrorSyncPosId(TiInt const *id)
  {
    this->errorSyncPosId = id == 0 ? 0 : id->get();
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

} // namespace

#endif
