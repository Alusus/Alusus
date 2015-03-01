/**
 * @file Tests/EndToEndTests/main.cpp
 * Contains the end-to-end test's main program.
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <stdlib.h>
// Alusus header files
#include <core.h>

// Direct Entries library
#include <dirent.h>

// Boost header files
#include <boost/algorithm/string/predicate.hpp>

using Core::Processing::BuildMsg;
using Core::Data::ParsedList;
using Core::Standard::RootManager;

namespace Tests { namespace EndToEndTests
{

const char *resultFilename = "EndToEndTest.txt";

/**
 * @brief Print the provided build message to the console.
 *
 * Printed message includes severity, msg code, location, as well as
 * description.
 */
void printBuildMsg(const SharedPtr<BuildMsg> &msg)
{
  // Print severity.
  switch (msg->getSeverity()) {
  case 0:
    std::cout << "BLOCKER "; break;
  case 1:
    std::cout << "ERROR "; break;
  case 2:
  case 3:
    std::cout << "WARNING "; break;
  case 4:
    std::cout << "ATTN "; break;
  }
  // Print msg code.
  std::cout << msg->getCode() << " @ ";
  // Print location.
  std::cout << msg->getFileName() << " (" << msg->getLine() << "," << msg->getColumn() << "): ";
  // Print description.
  std::cout << msg->getDescription() << NEW_LINE;
}

/**
 * Executes the given Alusus source code and compare the result against
 * an output file (having the same name with .output at the end).
 *
 * @param[in] fileName  The name of Alusus source file name.
 *
 * @return Returns @c true if the function succeeds, otherwise @c false.
 */
bool RunSourceFile(const std::string &fileName)
{
  try
  {
    // Redirect stdout to a file.
    fpos_t pos;
    fgetpos(stdout, &pos);
    int fd = dup(fileno(stdout));
    freopen(resultFilename,"w", stdout);

    // Prepare the root object;
    RootManager root;
    root.buildMsgNotifier.connect(&printBuildMsg);

    // Parse the provided filename.
    auto ptr = root.processFile(fileName.c_str());

    // Restore stdout.
    fflush(stdout);
    dup2(fd, fileno(stdout));
    close(fd);
    clearerr(stdout);
    fsetpos(stdout, &pos);

    if (ptr == 0)
      return false;

    return true;
  }
  catch (Core::Basic::Exception &e)
  {
    std::cout << "Failed to run source file " << fileName << "." << std::endl;
    std::cout << "The following error were thrown: " << std::endl;
    std::cout << e.getVerboseErrorMessage() << std::endl;
    return false;
  }
  catch (...)
  {
    std::cout << "Failed to run source file " << fileName << "." << std::endl;
    throw;
  }
}

/**
 * Checks whether the run result of a source file matches the expected result.
 * The expected result is stored in a file having the same name followed by
 * ".output" post-fix.
 * @param[in] fileName  The name of Alusus source file name.
 *
 * @return Returns @c true if the run result matches the expected result,
 * otherwise @c false.
 */
bool CheckRunResult(const std::string &fileName)
{
  std::ifstream runResult("EndToEndTest.txt");
  std::string runResultContent((std::istreambuf_iterator<char>(runResult)),
      std::istreambuf_iterator<char>());

  std::ifstream expectedResult(fileName + ".output");
  std::string expectedResultContent((std::istreambuf_iterator<char>(expectedResult)),
      std::istreambuf_iterator<char>());

  // Remove one character from the expectedResultContent because, for some
  // reason, editors seem to append 0A at the end of the file!
  auto ret =  runResultContent.compare(expectedResultContent) == 0;
  if (ret == true)
    std::cout << ">>> Test successful: " << fileName << std::endl;
  else
  {
    std::cout << ">>> Test failed: " << fileName << std::endl;
    std::cout << "Expected Result (Length = " << expectedResultContent.size() << "): " << std::endl;
    std::cout << expectedResultContent << std::endl;
    std::cout << "Received Result (Length = " << runResultContent.size() << "): " << std::endl;
    std::cout << runResultContent << std::endl;
  }
  return ret;
}

/**
 * Runs the given Alusus source file and checks the result against an expected
 * content file. The expected result is stored in a file having the same name
 * of the source file followed by ".output" post-fix.
 *
 * @param[in] fileName  The name of Alusus source file name.
 *
 * @return Returns @c true if the run result matches the expected result,
 * otherwise @c false.
 */
bool RunAndCheckSourceFile(const std::string &fileName)
{
  if (!RunSourceFile(fileName))
    return false;
  return CheckRunResult(fileName);
}

/**
 * Runs all the end-to-end tests under the folder EndToEndTests.
 *
 * @return Returns @c true if all test succeeds, otherwise @c false.
 */
bool RunEndToEndTests(const std::string &dirPath)
{
  DIR *dir;
  dirent *ent;
  if ((dir = opendir (dirPath.c_str())) != nullptr)
  {
    auto ret = true;
    while ((ent = readdir (dir)) != nullptr)
    {
      std::string fileName(ent->d_name);
      if (fileName.compare("common.alusus") != 0 &&
          (boost::algorithm::ends_with(fileName, ".alusus") || boost::algorithm::ends_with(fileName, ".أسس")))
      {
        if (!RunAndCheckSourceFile(dirPath + "/" + fileName))
          ret = false;
      }
    }
    closedir (dir);
    return ret;
  }
  else
  {
    std::cout << "Could not open end-to-end tests directory: " << dirPath << " !" << std::endl;
    return false;
  }
}

} } // namespace

using namespace Tests::EndToEndTests;

int main(int argc, char **argv)
{
  std::cout << "Alusus End-to-End Tests\n"
               "Version " << ALUSUS_VERSION << "\n"
               "Copyright (C) " << ALUSUS_RELEASE_YEAR << " Rafid Khalid Abdullah\n\n";

  auto ret = EXIT_SUCCESS;
  if (!RunEndToEndTests("./Tests/General"))
    ret = EXIT_FAILURE;
  if (!RunEndToEndTests("./Tests/Arabic"))
    ret = EXIT_FAILURE;
  if (!RunEndToEndTests("./Tests/TheCProgrammingLanguage"))
    ret = EXIT_FAILURE;
  return ret;
}
