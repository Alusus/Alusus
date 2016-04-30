/**
 * @file Core/Data/AlternateTerm.h
 * Contains the header of class Core::Data::AlternateTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_ALTERNATE_TERM_H
#define CORE_DATA_ALTERNATE_TERM_H

namespace Core { namespace Data
{

// TODO: Check the ownership of objects during the constructor. Will the objects be properly deleted in the destructor
//       in the case of an exception?

/**
 * @brief Handles alternate grammar terms.
 * @ingroup data_terms
 *
 * This class handles alternatives in grammar formulas. This class holds two
 * (or more) paths of terms and indicates that either of these two paths is
 * accepted. Priority by default is for the paths with the lower index, but
 * it can be overriden.
 */
class AlternateTerm : public ListTerm
{
  //============================================================================
  // Type Info

  TYPE_INFO(AlternateTerm, ListTerm, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Constructors

  /// Initialize an empty alternative term.
  public: AlternateTerm()
  {
  }

  /**
   * @brief Initialize the object with either a static list or a dynamic list.
   * @sa ListTerm::ListTerm()
   */
  public: AlternateTerm(const std::initializer_list<Argument<TermElement>> &args) : ListTerm(args)
  {
  }

  public: virtual ~AlternateTerm()
  {
  }

  public: static SharedPtr<AlternateTerm> create(const std::initializer_list<Argument<TermElement>> &args)
  {
    return std::make_shared<AlternateTerm>(args);
  }

}; // class

} } // namespace

#endif
