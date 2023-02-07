/**
 * @file Core/Notices/notices.cpp
 * Contains the global implementations of Notices namespace's declarations.
 *
 * @copyright Copyright (C) 2023 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Notices
{

Bool *sourceLocationPathSkipping = 0;

Bool* getSourceLocationPathSkippingPtr()
{
  if (sourceLocationPathSkipping == 0) {
    sourceLocationPathSkipping =
      reinterpret_cast<Bool*>(GLOBAL_STORAGE->getObject(S("Core::Notices::sourceLocationPathSkipping")));
    if (sourceLocationPathSkipping == 0) {
      sourceLocationPathSkipping = new Bool(false);
      GLOBAL_STORAGE->setObject(
        S("Core::Notices::sourceLocationPathSkipping"),
        reinterpret_cast<void*>(sourceLocationPathSkipping)
      );
    }
  }
  return sourceLocationPathSkipping;
}


void setSourceLocationPathSkipping(Bool skipping)
{
  *(getSourceLocationPathSkippingPtr()) = skipping;
}


Bool getSourceLocationPathSkipping()
{
  return *(getSourceLocationPathSkippingPtr());
}


void printNotice(Notice const *msg)
{
  // We will only print the error message if we have a source location for it.
  if (msg->getSourceLocation() == 0) return;

  // Print severity.
  switch (msg->getSeverity()) {
    case 0:
      outStream << S("\033[0;31m") << L18nDictionary::getSingleton()->get(S("BLOCKER"), S("BLOCKER")) << S(" ");
      break;
    case 1:
      outStream << S("\033[0;31m") << L18nDictionary::getSingleton()->get(S("ERROR"), S("ERROR")) << S(" ");
      break;
    case 2:
    case 3:
      outStream << S("\033[1;33m") << L18nDictionary::getSingleton()->get(S("WARNING"), S("WARNING")) << S(" ");
      break;
    case 4:
      outStream << S("\033[0;34m") << L18nDictionary::getSingleton()->get(S("ATTN"), S("ATTN")) << S(" ");
      break;
  }
  // Print msg code.
  outStream << msg->getCode() << ": ";
  // Print description.
  outStream << msg->getDescription() << S("\033[0m") << NEW_LINE << S("  ");
  // Print location.
  outStream << getSourceLocationString(msg->getSourceLocation().get(), 2) << NEW_LINE;
}


Str getSourceLocationString(Data::SourceLocation *sl, Int indentation) {
  StrStream stream;
  if (sl->isDerivedFrom<Data::SourceLocationRecord>()) {
    auto slRecord = static_cast<Data::SourceLocationRecord*>(sl);
    auto filename = getSourceLocationPathSkipping() ?
      strrchr(slRecord->filename.getBuf(), C('/')) + 1 :
      slRecord->filename.getBuf();
    stream << filename << " (" << slRecord->line << "," << slRecord->column << ")";
  } else {
    auto stack = static_cast<Data::SourceLocationStack*>(sl);
    stream << S("- ") << getSourceLocationString(stack->get(stack->getCount() - 1).get(), indentation + 2);
    for (Int i = stack->getCount() - 2; i >= 0; --i) {
      stream << NEW_LINE;
      for (Int j = 0; j < indentation; ++j) stream << S(" ");
      stream << S("  ") << getSourceLocationString(stack->get(i).get(), indentation + 2);
    }
  }
  return stream.str().c_str();
}

} // namespace
