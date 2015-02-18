/**
 * @file Core/main.cpp
 * Contains the program's entry point.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <stdlib.h>
#include <string.h>

/**
 * @defgroup main Main
 * @brief Contains elements related to the main program.
 */

namespace Core
{

/**
 * @brief Print the provided build message to the console.
 *
 * Printed message includes severity, msg code, location, as well as
 * description.
 */
void printBuildMsg(const SharedPtr<Processing::BuildMsg> &msg)
{
  // Print severity.
  switch (msg->getSeverity()) {
    case 0: outStream << STR("BLOCKER "); break;
    case 1: outStream << STR("ERROR "); break;
    case 2: case 3: outStream << STR("WARNING "); break;
    case 4: outStream << STR("ATTN "); break;
  }
  // Print msg code.
  outStream << msg->getCode() << " @ ";
  // Print location.
  outStream << msg->getFileName() << " (" << msg->getLine() << "," << msg->getColumn() << "): ";
  // Print description.
  outStream << msg->getDescription() << NEW_LINE;
}

} // namespace


using namespace Core;

/**
 * @brief The entry point of the program.
 * @ingroup main
 *
 * Parse the file with the name passed from the command line.
 */
int main(int argCount, char * const args[])
{
  Bool help = false;
  if (argCount < 2) help = true;
  for (Int i = 1; i < argCount; ++i) {
    if (strcmp(args[i], STR("--help")) == 0) help = true;
#ifdef USE_LOGS
    // Parse the log option.
    if (i < argCount-1) {
      if (strcmp(args[i], STR("--log")) == 0) {
        Logger::filter = atoi(args[i+1]);
        break;
      }
    }
#endif
  }

  if (help) {
    outStream << STR("Alusus Core\n"
                "Version " ALUSUS_VERSION "\n"
                "Copyright (C) " ALUSUS_RELEASE_YEAR " Sarmad Khalid Abdullah\n\n");
    outStream << STR("This software is released under Alusus Public License, Version 1.0.\n"
                "For details on usage and copying conditions read the full license at\n"
                "<http://alusus.net/alusus_license_1_0>. By using this software you acknowledge\n"
                "that you have read the terms in the license and agree with and accept all such\n"
                "terms.\n\n");
#if defined(USE_LOGS)
    outStream << STR("Usage: alusus <source> [<options>]\n");
#else
    outStream << STR("Usage: alusus <source>\n");
#endif
    outStream << STR("source = filename.\n");
#if defined(USE_LOGS)
    outStream << STR("\nOptions:\n");
    outStream << STR("\t--log  A 3 bit value to control the level of details of the log.\n");
#endif
    return EXIT_SUCCESS;
  }

  // Parse the provided source file.
  try {
    // Prepare the root object;
    Standard::RootManager root;
    root.buildMsgNotifier.connect(&printBuildMsg);

    // Parse the provided filename.
    SharedPtr<IdentifiableObject> ptr = root.processFile(args[1]);
    if (ptr == 0) return EXIT_SUCCESS;

    // Check if we have orphan data to print.
    SharedPtr<Data::ParsedList> orphan;
    if (ptr->isDerivedFrom<Data::Module>()) {
      Int orphanIndex = ptr.s_cast<Data::Module>()->findIndex(STR("_ORPHAN_"));
      if (orphanIndex != -1) {
        orphan = ptr.s_cast<Data::Module>()->getShared(orphanIndex).io_cast<Data::ParsedList>();
      }
    } else if (ptr->isDerivedFrom<Data::ParsedList>()) {
      orphan = ptr.s_cast<Data::ParsedList>();
    }
    if (orphan == 0 || orphan->getCount() == 0) return 0;

    // Print the parsed data.
    outStream << NEW_LINE << STR("-- BUILD COMPLETE --") << NEW_LINE << NEW_LINE <<
            STR("Build Results:") << NEW_LINE << NEW_LINE;
    dumpParsedData(orphan.get());
  } catch (Exception &e) {
    outStream << e.getErrorMessage() << NEW_LINE;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
