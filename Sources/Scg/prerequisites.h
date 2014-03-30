/**
 * @file Scg/prerequisites.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __prerequisites_h__
#define __prerequisites_h__

#define _DEBUG

// STL header files
#include <iostream>
#include <map>
#include <string>
#include <vector>

// Scg header files
#include <macros.h>
#include <typedefs.h>
#include <exceptions.h>

#ifdef _DEBUG
#include <DebuggingHelpers/llvm_helper.h>
#endif

#endif // __prerequisites_h__
