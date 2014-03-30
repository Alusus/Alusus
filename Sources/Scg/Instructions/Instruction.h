/**
 * @file Scg/Instructions/Instruction.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Instruction_h__
#define __Instruction_h__

#include <Expression.h>

namespace Scg
{
  /**
   * Base class for all instruction classes in the language.
   */
  class Instruction : public Expression
  {
  };
}

#endif // __Instruction_h__
