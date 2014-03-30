/**
 * @file Scg/main.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

// Scg include files
#include <core.h>   // Alusus core header files.
#include <llvm/IR/IRBuilder.h>
#include <scg.h>
#include <LibraryGateway.h>

using namespace Scg;
using namespace Core;

__attribute__((constructor)) void constructor()
{
}


__attribute__((destructor)) void destructor()
{
}


DL_EXPORTED LIBRARY_GATEWAY_GETTER_DEF
{
    static Scg::LibraryGateway libraryGateway;
    return &libraryGateway;
}
