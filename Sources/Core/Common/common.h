/**
 * @file Core/Common/common.h
 * Contains the definitions and include statements of all types in the Common
 * namespace.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef COMMON_COMMON_h
#define COMMON_COMMON_H

namespace Core { namespace Common
{

/**
 * @defgroup common Common
 * @ingroup core
 * @brief Common classes needed by the Core as and the external libs.
 *
 * This namespace contains definitions for common classes and types that are
 * needed by the Core as well as any external dynamic library built for it.
 */

} } // namespace


//==============================================================================
// Classes

#include "BuildMsg.h"
#include "CustomBuildMsg.h"
#include "UnrecognizedErrorMsg.h"

#include "Token.h"

#endif
