/**
 * @file Scg/Values/Value.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_VALUE_H
#define SCG_VALUE_H

#include <AstNode.h>

namespace Scg
{

// TODO: GetValueTypeSpec() was moved to Expression class, so this class can be
// deleted.
/**
 * Base class for any class representing values in the language, including
 * constants, primitive variables, structures, classes, etc.
 */
class Value : public AstNode
{
  TYPE_INFO(Value, AstNode, "Scg", "Scg", "alusus.net");
};

} // namespace

#endif
