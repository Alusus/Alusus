/**
 * @file Tests/CoreTests/main.cpp
 * Contains the program's entry point.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core_tests_pch.h"

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

using namespace Core;

/**
 * @defgroup core_tests Core Tests
 * @brief Contains elements related to Core tests.
 */


/**
 * @brief The entry point of the program.
 * @ingroup core_tests
 *
 * Parse the file with the name passed from the command line.
 */
int main(int argCount, char * const args[])
{
  // Validate arguments.
  for (int i = 0; i < argCount; ++i) {
    if (strcmp(args[i], STR("--help")) == 0) {
      Char alususReleaseYear[5];
      copyStr(ALUSUS_RELEASE_DATE, alususReleaseYear, 4);
      alususReleaseYear[4] = 0;
      outStream << STR("Alusus Core Tests\n"
                  "Version " ALUSUS_VERSION ALUSUS_REVISION " (" ALUSUS_RELEASE_DATE ")\n"
                  "Copyright (C) " << alususReleaseYear << " Sarmad Khalid Abdullah\n\n");
      outStream << STR("Usage: alusus_tests [<options>]\n");
      outStream << STR("\nOptions:\n");
#if defined(USE_LOGS)
      outStream << STR("\t--log  A 3 bit value to control the level of details of the log.\n");
#endif
      outStream << STR("\nUnit Test Specific Options:\n");
      Catch::Session().run(argCount, args);
      return 0;
    }
  }

#ifdef USE_LOGS
  // Parse the log option.
  for (Int i = 1; i < argCount-1; i++) {
    if (strcmp(args[i], STR("--log")) == 0) {
      Logger::filter = atoi(args[i+1]);
      break;
    }
  }
#endif

  // Create the args list, skipping non CATCH arguments.
  char **utargs = new char*[argCount];
  int utcount = 0;
  for (Int i = 0; i < argCount; i++) {
    if (strcmp(args[i], STR("--log")) == 0) {
      continue;
    }
    utargs[utcount] = const_cast<char*>(args[i]);
    utcount++;
  }
  // Run the unit tests.
  int ret = Catch::Session().run(utcount, utargs);
  delete[] utargs;
  return ret;
}
