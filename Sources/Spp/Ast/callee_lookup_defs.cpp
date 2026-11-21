/**
 * @file Spp/Ast/callee_lookup_defs.cpp
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Ast
{

void CalleeLookupRequest::setObjectTarget(Type *type, Type *thisType)
{
  this->target = type;
  this->thisType = thisType;
  this->mode = CalleeLookupMode::OBJECT_MEMBER;
  this->varTargetDef = 0;
}

} // namespace
