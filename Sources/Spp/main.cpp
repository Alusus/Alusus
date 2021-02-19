/**
 * @file Spp/main.cpp
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

using namespace Spp;
using namespace Core;

__attribute__((constructor)) void constructor()
{
}


__attribute__((destructor)) void destructor()
{
}


DL_EXPORTED LIBRARY_GATEWAY_GETTER_DEF {
  static Spp::LibraryGateway libraryGateway;
  return &libraryGateway;
}
