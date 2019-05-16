/**
 * @file Core/Data/Grammar/AlternateTerm.h
 * Contains the header of class Core::Data::Grammar::AlternateTerm.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_ALTERNATETERM_H
#define CORE_DATA_GRAMMAR_ALTERNATETERM_H

namespace Core::Data::Grammar
{

// TODO: Check the ownership of objects during the constructor. Will the objects be properly deleted in the destructor
//       in the case of an exception?

/**
 * @brief Handles alternate grammar terms.
 * @ingroup core_data_grammar
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

  TYPE_INFO(AlternateTerm, ListTerm, "Core.Data.Grammar", "Core", "alusus.org");


  //============================================================================
  // Constructors

  IMPLEMENT_EMPTY_CONSTRUCTOR(AlternateTerm);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(AlternateTerm, this->validate());

  public: virtual ~AlternateTerm()
  {
  }

}; // class

} // namespace

#endif
