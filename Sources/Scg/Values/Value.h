/**
 * @file Scg/Values/Value.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Value_h__
#define __Value_h__

#include <Expression.h>

namespace Scg
{
// TODO: GetValueTypeSpec() was moved to Expression class, so this class can be
// deleted.
/**
 * Base class for any class representing values in the language, including
 * constants, primitive variables, structures, classes, etc.
 */
class Value : public Expression
{
};
}

#endif // __Value_h__
