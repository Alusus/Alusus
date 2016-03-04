/**
 * @file Scg/Instructions/Instruction.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_INSTRUCTION_H
#define SCG_INSTRUCTION_H

#include "core.h"
#include <AstNode.h>

namespace Scg
{

/**
 * Base class for all instruction classes in the language.
 */
class Instruction : public AstNode
{
  TYPE_INFO(Instruction, AstNode, "Scg", "Scg", "alusus.net");
};

} // namespace

#endif
