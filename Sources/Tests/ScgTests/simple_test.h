/**
 * @file Tests/ScgTests/simple_test.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

// Alusus header files
#include <scg.h>

namespace Tests { namespace ScgTests
{

Scg::ValueTypeSpec *CreateTypeSpecByName(const std::string &name);
Scg::ValueTypeSpec *CreatePointerToPrimitiveTypeSpec(const std::string &name);
Scg::ValueTypeSpec *CreateArrayOfPrimitiveTypeSpec(const std::string &name, int size);
//std::string BuildSimpleTest(const Scg::ExpressionArray &mainBody,
//    const std::vector<Scg::Expression*> &linkStatements = {});
std::string RunSimpleTest(const Scg::ExpressionArray &mainBody,
    const std::vector<Scg::Expression*> &linkStatements = {});

} } // namespace
