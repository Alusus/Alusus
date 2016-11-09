/**
 * @file Core/Data/BuildHandler.h
 * Contains the header of class Core::Data::BuildHandler.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_BUILDHANDLER_H
#define CORE_DATA_BUILDHANDLER_H

namespace Core { namespace Data
{

// TODO: DOC

class BuildHandler : public Node
{
  //============================================================================
  // Type Info

  TYPE_INFO(BuildHandler, Node, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Protected Constructor

  protected: BuildHandler()
  {
  }

}; // class

} } // namespace

#endif
