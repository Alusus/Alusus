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

#include "core_tests.h"
#include <catch_runner.hpp>

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
      outStream << STR("\t-l, --list  <tests | reporters> [xml]\n");
      outStream << STR("\t-t, --test  <testspec> [<testspec>...]\n");
      outStream << STR("\t-r, --reporter  <reporter name>\n");
      outStream << STR("\t-o, --out  <file name>|<%stream name>\n");
      outStream << STR("\t-s, --success\n");
      outStream << STR("\t-b, --break\n\n");
      outStream << STR("For more detail usage please see: https://github.com/philsquared/Catch/wiki/Command-line\n");
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
  for (Int i = 1; i < argCount; i++) {
    if (strcmp(args[i], STR("--log")) == 0) {
      ++i;
      continue;
    }
    utargs[utcount] = const_cast<char*>(args[i]);
    utcount++;
  }
  // Run the unit tests.
  int ret = Catch::Main(utcount, utargs);
  delete[] utargs;
  return ret;
}
