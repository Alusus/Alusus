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
  // We will only print the error message if we have a source location for it.
  if (msg->getSourceLocation().filename == 0) return;

  // Print severity.
  switch (msg->getSeverity()) {
    case 0: outStream << STR("\033[0;31mBLOCKER "); break;
    case 1: outStream << STR("\033[0;31mERROR "); break;
    case 2: case 3: outStream << STR("\033[1;33mWARNING "); break;
    case 4: outStream << STR("\033[0;34mATTN "); break;
  }
  // Print msg code.
  outStream << msg->getCode() << " @ ";
  // Print location.
  outStream << msg->getSourceLocation().filename->c_str()
            << " (" << msg->getSourceLocation().line << "," << msg->getSourceLocation().column << "): ";
  // Print description.
  outStream << msg->getDescription() << STR("\033[0m") << NEW_LINE;
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
  Bool dump = false;
  if (argCount < 2) help = true;
  for (Int i = 1; i < argCount; ++i) {
    if (strcmp(args[i], STR("--help")) == 0) help = true;
    else if (strcmp(args[i], STR("--مساعدة")) == 0) help = true;
    else if (strcmp(args[i], STR("--dump")) == 0) dump = true;
    else if (strcmp(args[i], STR("--إلقاء")) == 0) dump = true;
#ifdef USE_LOGS
    // Parse the log option.
    if (i < argCount-1) {
      if (strcmp(args[i], STR("--log")) == 0 || strcmp(args[i], STR("--تدوين")) == 0) {
        Logger::filter = atoi(args[i+1]);
        break;
      }
    }
#endif
  }

  if (help) {
    Char alususReleaseYear[5];
    Char alususHijriReleaseYear[5];
    copyStr(ALUSUS_RELEASE_DATE, alususReleaseYear, 4);
    copyStr(ALUSUS_HIJRI_RELEASE_DATE, alususHijriReleaseYear, 4);
    alususReleaseYear[4] = alususHijriReleaseYear[4] = 0;
    // Check if the command line was in English by detecting if the first character is ASCII.
    if (args[0][0] > 0) {
      // Write English help.
      outStream << STR("Alusus Core\n"
                  "Version " ALUSUS_VERSION ALUSUS_REVISION " (" ALUSUS_RELEASE_DATE ")\n"
                  "Copyright (C) " << alususReleaseYear << " Sarmad Khalid Abdullah\n\n");
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
      outStream << STR("\nOptions:\n");
      outStream << STR("\t--dump  Tells the Core to dump the resulting AST tree.\n");
      #if defined(USE_LOGS)
        outStream << STR("\t--log  A 6 bit value to control the level of details of the log.\n");
      #endif
    } else {
      // Write Arabic help.
      outStream << STR("قلب الأسُس\n"
                       "الإصدار " ALUSUS_VERSION ALUSUS_REVISION "\n (" ALUSUS_RELEASE_DATE "م \\ " ALUSUS_HIJRI_RELEASE_DATE "هـ)\n"
                       "جميع الحقوق محفوظة لـ سرمد خالد عبدالله " << alususReleaseYear << "م\\" << alususHijriReleaseYear << "هـ\n\n");
      outStream << STR("نُشر هذا البرنامج برخصة الأسُس العامة (Alusus Public License)، الإصدار 1.0\n"
                       "والمتوفرة على الرابط <http://alusus.net/alusus_license_1_0>. يرجى قراءة\n"
                       "الرخصة قبل استخدام البرنامج. استخدامك لهذا البرنامج أو أي من الملفات\n"
                       "المرفقة معه إقرار منك أنك قرأت هذه الرخصة ووافقت على جميع فقراتها.\n\n");
      #if defined(USE_LOGS)
        outStream << STR("طريقة الاستخدام: الأسُس < الشفرة المصدرية > [< خيارات >]\n");
      #else
        outStream << STR("طريقة الاستخدام: الأسُس < الشفرة المصدرية >\n");
      #endif
      outStream << STR("الشفرة المصدرية = اسم الملف الحاوي على الشفرة المصدرية\n");
      outStream << STR("\nالخيارات:\n");
      outStream << STR("\t--إلقاء  تبلغ القلب بإلقاء شجرة AST عند الانتهاء.\n");
      #if defined(USE_LOGS)
        outStream << STR("\t--تدوين  قيمة من 6 بتّات للتحكم بمستوى التدوين.\n");
      #endif
    }
    return EXIT_SUCCESS;
  }

  // Parse the provided source file.
  try {
    // Prepare the root object;
    Standard::RootManager root;
    root.buildMsgNotifier.connect(&printBuildMsg);

    // Parse the provided filename.
    TioSharedPtr ptr = root.processFile(args[1]);
    if (ptr == 0) return EXIT_SUCCESS;

    // Print the parsed data.
    if (dump) {
      outStream << NEW_LINE << STR("-- BUILD COMPLETE --") << NEW_LINE << NEW_LINE <<
              STR("Build Results:") << NEW_LINE << NEW_LINE;
      Data::dumpData(outStream, ptr.get(), 0);
      outStream << NEW_LINE;
    }
  } catch (Exception &e) {
    outStream << e.getVerboseErrorMessage() << NEW_LINE;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
