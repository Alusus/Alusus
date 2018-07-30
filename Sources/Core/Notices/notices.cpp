/**
 * @file Core/Notices/notices.cpp
 * Contains the global implementations of Notices namespace's declarations.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Notices
{

void printNotice(Notice const *msg)
{
  // We will only print the error message if we have a source location for it.
  if (Data::getSourceLocationRecordCount(msg->getSourceLocation().get()) == 0) return;

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
  auto sl = msg->getSourceLocation().get();
  if (sl->isDerivedFrom<Data::SourceLocationRecord>()) {
    auto slRecord = static_cast<Data::SourceLocationRecord*>(sl);
    outStream << slRecord->filename->c_str() << " (" << slRecord->line << "," << slRecord->column << ")";
  } else {
    auto stack = static_cast<Data::SourceLocationStack*>(sl);
    for (Int i = stack->getCount() - 1; i >= 0; --i) {
      if (i < stack->getCount() -1) outStream << NEW_LINE << STR("from ");
      outStream << stack->get(i)->filename->c_str()
        << " (" << stack->get(i)->line << "," << stack->get(i)->column << ")";
    }
  }
  outStream << STR(": ");
  // Print description.
  outStream << msg->getDescription() << STR("\033[0m") << NEW_LINE;
}

} // namespace
