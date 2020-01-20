/**
 * @file Core/Data/Grammar/BuildHandler.h
 * Contains the header of class Core::Data::Grammar::BuildHandler.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_BUILDHANDLER_H
#define CORE_DATA_GRAMMAR_BUILDHANDLER_H

namespace Core::Data::Grammar
{

// TODO: DOC

class BuildHandler : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(BuildHandler, TiObject, "Core.Data.Grammar", "Core", "alusus.org");


  //============================================================================
  // Protected Constructor

  protected: BuildHandler()
  {
  }

}; // class

} // namespace

#endif
