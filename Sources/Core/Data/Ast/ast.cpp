/**
 * @file Core/Data/Ast/ast.cpp
 * Contains the global implementations of Data::Ast namespace's declarations.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data { namespace Ast
{

//============================================================================
// Global Functions

SharedPtr<SourceLocation> const& findSourceLocation(TiObject *obj)
{
  auto metadata = ti_cast<Metadata>(obj);
  if (metadata != 0) return metadata->findSourceLocation();
  else return SharedPtr<SourceLocation>::null;
}

} } } // namespace
