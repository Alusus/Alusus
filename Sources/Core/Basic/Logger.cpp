/**
 * @file Core/Basic/Logger.cpp
 * Contains the implementation of class Core::Basic::Logger.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

#ifdef USE_LOGS

namespace Core { namespace Basic
{

//==============================================================================
// Static Variables

Word *Logger::filter = 0;


//==============================================================================
// Static Functions

Word* Logger::getFilterPtr()
{
  if (Logger::filter == 0) {
    Logger::filter = reinterpret_cast<Word*>(GLOBAL_STORAGE->getObject(S("Core::Basic::Logger::filter")));
    if (Logger::filter == 0) {
      Logger::filter = new Word;
      GLOBAL_STORAGE->setObject(S("Core::Basic::Logger::filter"), reinterpret_cast<void*>(Logger::filter));
    }
  }
  return Logger::filter;
}

} } // namespace

#endif
