/**
 * @file Core/Grammar/BuildHandler.h
 * Contains the header of class Core::Grammar::BuildHandler.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_GRAMMAR_BUILDHANDLER_H
#define CORE_GRAMMAR_BUILDHANDLER_H

namespace Core::Grammar
{

// TODO: DOC

class BuildHandler : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(BuildHandler, TiObject, "Core.Grammar", "Core", "alusus.org");


  //============================================================================
  // Protected Constructor

  protected: BuildHandler()
  {
  }

}; // class

} // namespace

#endif
