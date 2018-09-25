/**
 * @file Core/main.cpp
 * Contains the program's entry point.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
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
  Bool interactive = false;
  Bool dump = false;
  Bool english = args[0][0] > 0;
  if (argCount < 2) help = true;
  for (Int i = 1; i < argCount; ++i) {
    if (strcmp(args[i], S("--help")) == 0) help = true;
    else if (strcmp(args[i], S("--مساعدة")) == 0) help = true;
    else if (strcmp(args[i], S("--interactive")) == 0) interactive = true;
    else if (strcmp(args[i], S("--تفاعلي")) == 0) interactive = true;
    else if (strcmp(args[i], S("-i")) == 0) interactive = true;
    else if (strcmp(args[i], S("-ت")) == 0) interactive = true;
    else if (strcmp(args[i], S("--dump")) == 0) dump = true;
    else if (strcmp(args[i], S("--إلقاء")) == 0) dump = true;
#ifdef USE_LOGS
    // Parse the log option.
    if (i < argCount-1) {
      if (strcmp(args[i], S("--log")) == 0 || strcmp(args[i], S("--تدوين")) == 0) {
        Logger::filter = atoi(args[i+1]);
        break;
      }
    }
#endif
  }

  if (!english) {
    // TODO: Support other locales.
    Core::Notices::L18nDictionary::getSingleton()->initialize(S("ar"));
  }

  if (help) {
    Char alususReleaseYear[5];
    Char alususHijriReleaseYear[5];
    copyStr(ALUSUS_RELEASE_DATE, alususReleaseYear, 4);
    copyStr(ALUSUS_HIJRI_RELEASE_DATE, alususHijriReleaseYear, 4);
    alususReleaseYear[4] = alususHijriReleaseYear[4] = 0;
    // Check if the command line was in English by detecting if the first character is ASCII.
    if (english) {
      // Write English help.
      outStream << S("Alusus Core\n"
                     "Version " ALUSUS_VERSION ALUSUS_REVISION " (" ALUSUS_RELEASE_DATE ")\n"
                     "Copyright (C) " << alususReleaseYear << " Sarmad Khalid Abdullah\n\n");
      outStream << S("This software is released under Alusus Public License, Version 1.0.\n"
                     "For details on usage and copying conditions read the full license at\n"
                     "<http://alusus.net/alusus_license_1_0>. By using this software you acknowledge\n"
                     "that you have read the terms in the license and agree with and accept all such\n"
                     "terms.\n\n");
      #if defined(USE_LOGS)
        outStream << S("Usage: alusus <source> [<options>]\n");
      #else
        outStream << S("Usage: alusus <source>\n");
      #endif
      outStream << S("source = filename.\n");
      outStream << S("\nOptions:\n");
      outStream << S("\t--interactive, -i  Run in interactive mode.\n");
      outStream << S("\t--dump  Tells the Core to dump the resulting AST tree.\n");
      #if defined(USE_LOGS)
        outStream << S("\t--log  A 6 bit value to control the level of details of the log.\n");
      #endif
    } else {
      // Write Arabic help.
      outStream << S("قلب الأسُس\n"
                     "الإصدار " ALUSUS_VERSION ALUSUS_REVISION "\n (" ALUSUS_RELEASE_DATE "م \\ " ALUSUS_HIJRI_RELEASE_DATE "هـ)\n"
                     "جميع الحقوق محفوظة لـ سرمد خالد عبدالله " << alususReleaseYear << "م\\" << alususHijriReleaseYear << "هـ\n\n");
      outStream << S("نُشر هذا البرنامج برخصة الأسُس العامة (Alusus Public License)، الإصدار 1.0\n"
                     "والمتوفرة على الرابط <http://alusus.net/alusus_license_1_0>. يرجى قراءة\n"
                     "الرخصة قبل استخدام البرنامج. استخدامك لهذا البرنامج أو أي من الملفات\n"
                     "المرفقة معه إقرار منك أنك قرأت هذه الرخصة ووافقت على جميع فقراتها.\n\n");
      #if defined(USE_LOGS)
        outStream << S("طريقة الاستخدام: الأسُس < الشفرة المصدرية > [< خيارات >]\n");
      #else
        outStream << S("طريقة الاستخدام: الأسُس < الشفرة المصدرية >\n");
      #endif
      outStream << S("الشفرة المصدرية = اسم الملف الحاوي على الشفرة المصدرية\n");
      outStream << S("\nالخيارات:\n");
      outStream << S("\t--تفاعلي، -ت  تنفيذ بشكل تفاعلي.\n");
      outStream << S("\t--إلقاء  تبلغ القلب بإلقاء شجرة AST عند الانتهاء.\n");
      #if defined(USE_LOGS)
        outStream << S("\t--تدوين  قيمة من 6 بتّات للتحكم بمستوى التدوين.\n");
      #endif
    }
    return EXIT_SUCCESS;
  } else if (interactive) {
    // Run in interactive mode.
    if (english) {
      outStream << S("Running in interactive mode.\n");
      outStream << S("Press CTRL+C to exit.\n\n");
    } else {
      outStream << S("تنفيذ بشكل تفاعلي.\n");
      outStream << S("إضغط على CTRL+C للخروج.\n\n");
    }

    try {
      // Prepare the root object;
      Main::RootManager root;
      Slot<void, SharedPtr<Notices::Notice> const&> noticeSlot(
        [](SharedPtr<Notices::Notice> const &notice)->void
        {
          Notices::printNotice(notice.get());
        }
      );
      root.noticeSignal.connect(noticeSlot);

      // Parse the standard input stream.
      root.processStream(&inStream, S("user input"));
    } catch (Exception &e) {
      outStream << e.getVerboseErrorMessage() << NEW_LINE;
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  } else {
    // Parse the provided source file.
    try {
      // Prepare the root object;
      Main::RootManager root;
      Slot<void, SharedPtr<Notices::Notice> const&> noticeSlot(
        [](SharedPtr<Notices::Notice> const &notice)->void
        {
          Notices::printNotice(notice.get());
        }
      );
      root.noticeSignal.connect(noticeSlot);

      // Parse the provided filename.
      TioSharedPtr ptr = root.processFile(args[1]);
      if (ptr == 0) return EXIT_SUCCESS;

      // Print the parsed data.
      if (dump) {
        outStream << NEW_LINE << S("-- BUILD COMPLETE --") << NEW_LINE << NEW_LINE <<
                S("Build Results:") << NEW_LINE << NEW_LINE;
        Data::dumpData(outStream, ptr.get(), 0);
        outStream << NEW_LINE;
      }
    } catch (FileException &e) {
      if (english) {
        outStream << S("File not found: ") << e.getFileName() << NEW_LINE;
      } else {
        outStream << S("الملف مفقود: ") << e.getFileName() << NEW_LINE;
      }
      return EXIT_FAILURE;
    } catch (Exception &e) {
      outStream << e.getVerboseErrorMessage() << NEW_LINE;
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }
}
