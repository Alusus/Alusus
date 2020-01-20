/**
 * @file Core/main.cpp
 * Contains the program's entry point.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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
 * @brief Get the system language code from env vars.
 * @ingroup main
 * This checks the value in LANGUAGE env var if available. If not it checks the
 * value in LANG env var.
 */
Str getSystemLanguage()
{
  Char const *langEnv = getenv(S("LANGUAGE"));
  if (langEnv == 0 || getStrLen(langEnv) == 0) langEnv = getenv(S("LANG"));
  if (langEnv == 0 || getStrLen(langEnv) == 0) langEnv = S("en");

  Str lang;
  lang.assign(langEnv, 2);
  return lang;
}


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
  Char const *sourceFile = 0;
  Bool dump = false;
  auto lang = getSystemLanguage();
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
    else if (strcmp(args[i], S("--log")) == 0 || strcmp(args[i], S("--تدوين")) == 0) {
      if (i < argCount-1) {
        ++i;
        Logger::filter = atoi(args[i]);
      } else {
        Logger::filter = 0;
      }
    }
#endif
    else {
      sourceFile = args[i];
      break;
    }
  }

  if (lang == S("ar")) {
    // TODO: Support other locales.
    Core::Notices::L18nDictionary::getSingleton()->initialize(S("ar"));
  }

  // We'll show help if now source file is given.
  if (sourceFile == 0 && !interactive) help = true;

  if (help) {
    Char alususReleaseYear[5];
    Char alususHijriReleaseYear[5];
    copyStr(ALUSUS_RELEASE_DATE, alususReleaseYear, 4);
    copyStr(ALUSUS_HIJRI_RELEASE_DATE, alususHijriReleaseYear, 4);
    alususReleaseYear[4] = alususHijriReleaseYear[4] = 0;
    // Check if the command line was in English by detecting if the first character is ASCII.
    if (lang == S("ar")) {
      // Write Arabic help.
      outStream << S("قلب الأسُس\n"
                     "الإصدار (" ALUSUS_VERSION ALUSUS_REVISION ")\n(" ALUSUS_RELEASE_DATE " م)\n(" ALUSUS_HIJRI_RELEASE_DATE " هـ)\n"
                     "جميع الحقوق محفوظة لـ سرمد خالد عبدالله (" << alususReleaseYear << " م) \\ (" << alususHijriReleaseYear << " هـ)\n\n");
      outStream << S("نُشر هذا البرنامج برخصة الأسُس العامة، الإصدار 1.0، والمتوفرة على الرابط أدناه.\n"
                     "يرجى قراءة الرخصة قبل استخدام البرنامج. استخدامك لهذا البرنامج أو أي من الملفات\n"
                     "المرفقة معه إقرار منك أنك قرأت هذه الرخصة ووافقت على جميع فقراتها.\n");
      outStream << S("\nAlusus Public License: <https://alusus.org/alusus_license_1_0>\n");
      outStream << S("\nطريقة الاستخدام:\n");
      outStream << S("الأسُس [<خيارات القلب>] <الشفرة المصدرية> [<خيارات البرنامج>]\n");
      outStream << S("الشفرة المصدرية = اسم الملف الحاوي على الشفرة المصدرية\n");
      outStream << S("alusus [<Core options>] <source> [<program options>]\n");
      outStream << S("source = filename.\n");
      outStream << S("\nالخيارات:\n");
      outStream << S("\tتنفيذ بشكل تفاعلي:\n");
      outStream << S("\t\t--تفاعلي\n");
      outStream << S("\t\t-ت\n");
      outStream << S("\t\t--interactive\n");
      outStream << S("\t\t-i\n");
      outStream << S("\tالقاء شجرة AST عند الانتهاء:\n");
      outStream << S("\t\t--شجرة\n");
      outStream << S("\t\t--dump\n");
      #if defined(USE_LOGS)
        outStream << S("\tالتحكم بمستوى التدوين (قيمة من 6 بتات):\n");
        outStream << S("\t\t--تدوين\n");
        outStream << S("\t\t--log\n");
      #endif
    } else {
      // Write English help.
      outStream << S("Alusus Core\n"
                     "Version " ALUSUS_VERSION ALUSUS_REVISION " (" ALUSUS_RELEASE_DATE ")\n"
                     "Copyright (C) " << alususReleaseYear << " Sarmad Khalid Abdullah\n\n");
      outStream << S("This software is released under Alusus Public License, Version 1.0.\n"
                     "For details on usage and copying conditions read the full license at\n"
                     "<https://alusus.org/alusus_license_1_0>. By using this software you acknowledge\n"
                     "that you have read the terms in the license and agree with and accept all such\n"
                     "terms.\n\n");
      outStream << S("Usage: alusus [<Core options>] <source> [<program options>]\n");
      outStream << S("source = filename.\n");
      outStream << S("\nOptions:\n");
      outStream << S("\t--interactive, -i  Run in interactive mode.\n");
      outStream << S("\t--dump  Tells the Core to dump the resulting AST tree.\n");
      #if defined(USE_LOGS)
        outStream << S("\t--log  A 6 bit value to control the level of details of the log.\n");
      #endif
    }
    return EXIT_SUCCESS;
  } else if (interactive) {
    // Run in interactive mode.
    if (lang == S("ar")) {
      outStream << S("تنفيذ بشكل تفاعلي.\n");
      outStream << S("إضغط على CTRL+C للخروج.\n\n");
    } else {
      outStream << S("Running in interactive mode.\n");
      outStream << S("Press CTRL+C to exit.\n\n");
    }

    try {
      // Prepare the root object;
      Main::RootManager root;
      root.setInteractive(true);
      root.setProcessArgInfo(argCount, args);
      root.setLanguage(lang.c_str());
      Slot<void, SharedPtr<Notices::Notice> const&> noticeSlot(
        [](SharedPtr<Notices::Notice> const &notice)->void
        {
          Notices::printNotice(notice.get());
        }
      );
      root.noticeSignal.connect(noticeSlot);

      // Parse the standard input stream.
      Processing::InteractiveCharInStream charStream;
      root.processStream(&charStream, S("user input"));
    } catch (Exception &e) {
      outStream << e.getVerboseErrorMessage() << NEW_LINE;
      return EXIT_FAILURE;
    } catch (Int v) {
      return v;
    }
    return EXIT_SUCCESS;
  } else {
    // Parse the provided source file.
    try {
      // Prepare the root object;
      Main::RootManager root;
      root.setProcessArgInfo(argCount, args);
      root.setLanguage(lang.c_str());
      Slot<void, SharedPtr<Notices::Notice> const&> noticeSlot(
        [](SharedPtr<Notices::Notice> const &notice)->void
        {
          Notices::printNotice(notice.get());
        }
      );
      root.noticeSignal.connect(noticeSlot);

      // Parse the provided filename.
      TioSharedPtr ptr = root.processFile(sourceFile);
      if (ptr == 0) return EXIT_SUCCESS;

      // Print the parsed data.
      if (dump) {
        outStream << NEW_LINE << S("-- BUILD COMPLETE --") << NEW_LINE << NEW_LINE <<
                S("Build Results:") << NEW_LINE << NEW_LINE;
        Data::dumpData(outStream, ptr.get(), 0);
        outStream << NEW_LINE;
      }
    } catch (FileException &e) {
      if (lang == S("ar")) {
        outStream << S("الملف مفقود: ") << e.getFileName() << NEW_LINE;
      } else {
        outStream << S("File not found: ") << e.getFileName() << NEW_LINE;
      }
      return EXIT_FAILURE;
    } catch (Exception &e) {
      outStream << e.getVerboseErrorMessage() << NEW_LINE;
      return EXIT_FAILURE;
    } catch (Int v) {
      return v;
    }
    return EXIT_SUCCESS;
  }
}
