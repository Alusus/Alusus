/**
 * @file Tests/ScgTests/main.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <stdlib.h>
#include <iostream>

namespace Tests { namespace ScgTests
{

bool TestDefineAndUseArray();
bool TestGlobalvariable();
bool RunAllVariableTests();
bool RunAllUnaryOperatorsTests();
bool RunAllBinaryOperatorsTests();
bool RunHelloWorldTest();
bool RunHelloWorldTwoModulesTest();
bool RunAllStructureTests();
bool RunCastIntToFloatTest();

} } // namespace

using namespace Tests::ScgTests;

int main(int argc, char **argv)
{
  std::cout << "Alusus SCG Tests\n"
               "Version " << ALUSUS_VERSION << "\n"
               "Copyright (C) " << ALUSUS_RELEASE_YEAR << " Rafid Khalid Abdullah\n\n";

  int result = EXIT_SUCCESS;
  if (!TestDefineAndUseArray()) result = EXIT_FAILURE;
  if (!TestGlobalvariable()) result = EXIT_FAILURE;
  if (!RunAllVariableTests()) result = EXIT_FAILURE;
  if (!RunAllUnaryOperatorsTests()) result = EXIT_FAILURE;
  if (!RunAllBinaryOperatorsTests()) result = EXIT_FAILURE;
  if (!RunHelloWorldTest()) result = EXIT_FAILURE;
  if (!RunHelloWorldTwoModulesTest()) result = EXIT_FAILURE;
  if (!RunAllStructureTests()) result = EXIT_FAILURE;
  if (!RunCastIntToFloatTest()) result = EXIT_FAILURE;

  return result;
}
