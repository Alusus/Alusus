/**
 * @file Spp/Ast/PointerTypeTemplate.cpp
 * Contains the implementation of class Spp::Ast::PointerTypeTemplate.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace Ast
{

//==============================================================================
// Member Functions

// TODO:

ValueType* PointerTypeTemplate::getValueType(TiObject *templateInput, Seeker *seeker);

ValueType* PointerTypeTemplate::getValueType(ValueType *contentType);

} } // namespace
