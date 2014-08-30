/**
 * @file Core/main.cpp
 * Contains the program's entry point.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
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

// These references are declared to enable easily converting between unicode and ascii.
static std::ostream & Cout = std::cout;
static std::istream & Cin = std::cin;


/// Print 'indents' number of spaces.
void printIndents(int indents)
{
  for (Int i=0; i < indents; ++i) {
    Cout << STR(" ");
  }
}


/**
 * @brief Recursive function to print a tree of parsed data.
 * @ingroup main
 *
 * The given tree should have default parsing data (Default::ParsedItem,
 * Default::ParsedList, or Default::ParsedToken). Anything other than the
 * default data types will be represented by [UNKNOWN TYPE].
 */
void debugPrintParsedData(SharedPtr<IdentifiableObject> const &ptr, int indents=0)
{
  printIndents(indents);
  if (ptr == 0) {
    Cout << STR("NULL:\n");
    return;
  }

  // Is this a default data type?
  if (ptr->isDerivedFrom<Standard::ParsedItem>()) {
    // Print the production name.
    Word id = ptr.io_cast_get<Standard::ParsedItem>()->getProdId();
    if (id != UNKNOWN_ID) {
      Cout << Data::IdGenerator::getSingleton()->getDesc(id) << STR(" -- ");
    }
    // Print the data itself.
    if (ptr->isDerivedFrom<Standard::ParsedList>()) {
      Cout << STR("[LIST]:\n");
      for (Word i = 0; i < ptr.s_cast_get<Standard::ParsedList>()->getElementCount(); ++i) {
        debugPrintParsedData(ptr.s_cast_get<Standard::ParsedList>()->getElement(i), indents+1);
      }
    } else if (ptr->isDerivedFrom<Standard::ParsedRoute>()) {
      Cout << STR("[ROUTE]: ");
      Cout << ptr.s_cast_get<Standard::ParsedRoute>()->getRoute() << STR("\n");
      debugPrintParsedData(ptr.s_cast_get<Standard::ParsedRoute>()->getData(), indents+1);
    } else if (ptr->isDerivedFrom<Standard::ParsedToken>()) {
      Cout << STR("[TOKEN]: ");
      // Print the token type.
      Int id = ptr.s_cast_get<Standard::ParsedToken>()->getId();
      Cout << Data::IdGenerator::getSingleton()->getDesc(id);
      // Print the token text.
      Cout << STR(" (\"") << ptr.s_cast_get<Standard::ParsedToken>()->getText() << STR("\")\n");
    } else {
      // A default parsed item but not one of the three known types.
      Cout << STR("[UNKNOWN ParsedItem]\n");
    }
  } else {
    // Unkown datat type not even derived form ParsedItem.
    Cout << STR("[UNKNOWN TYPE]\n");
  }
}


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
    case 0: Cout << STR("BLOCKER "); break;
    case 1: Cout << STR("ERROR "); break;
    case 2: case 3: Cout << STR("WARNING "); break;
    case 4: Cout << STR("ATTN "); break;
  }
  // Print msg code.
  Cout << msg->getCode() << " @ ";
  // Print location.
  Cout << msg->getFileName() << " (" << msg->getLine() << "," << msg->getColumn() << "): ";
  // Print description.
  Cout << msg->getDescription() << NEW_LINE;
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
    Cout << STR("Alusus Core\n"
                "Version " ALUSUS_VERSION "\n"
                "Copyright (C) " ALUSUS_RELEASE_YEAR " Sarmad Khalid Abdullah\n\n");
    Cout << STR("This software is released under Alusus Public License, Version 1.0.\n"
                "For details on usage and copying conditions read the full license at\n"
                "<http://alusus.net/alusus_license_1_0>. By using this software you acknowledge\n"
                "that you have read the terms in the license and agree with and accept all such\n"
                "terms.\n\n");
#if defined(USE_LOGS)
    Cout << STR("Usage: alusus <source> [<options>]\n");
#else
    Cout << STR("Usage: alusus <source>\n");
#endif
    Cout << STR("source = filename.\n");
#if defined(USE_LOGS)
    Cout << STR("\nOptions:\n");
    Cout << STR("\t--log  A 3 bit value to control the level of details of the log.\n");
#endif
    return EXIT_SUCCESS;
  }

  // Parse the provided source file.
  try {
    // Prepare the root object;
    Standard::RootManager root;
    root.buildMsgNotifier.connect(&printBuildMsg);

    // Parse the provided filename.
    SharedPtr<Standard::ParsedItem> ptr = root.processFile(args[1]).io_cast<Standard::ParsedItem>();
    if (ptr == 0) return EXIT_SUCCESS;
    if (ptr.s_cast_get<Standard::ParsedList>()->getElementCount() == 0) return 0;

    // Print the parsed data.
    Cout << NEW_LINE << STR("-- BUILD COMPLETE --") << NEW_LINE << NEW_LINE <<
            STR("Build Results:") << NEW_LINE << NEW_LINE;
    debugPrintParsedData(ptr);
  } catch (Exception &e) {
    Cout << e.getErrorMessage() << NEW_LINE;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
