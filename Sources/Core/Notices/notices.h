/**
 * @file Core/Notices/notices.h
 * Contains the definitions and include statements of all user notices.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_NOTICES_NOTICES_H
#define CORE_NOTICES_NOTICES_H

/**
 * @defgroup core_notices Notices
 * @ingroup core
 * @brief Classes that contains notices.
 *
 * This namespace contains definitions for classes and types for build notices.
 */


//==============================================================================
// Forward Class Definitions

namespace Core::Notices
{
  class Notice;
}


//==============================================================================
// Macros

#define DEFINE_NOTICE(name, typeNamespace, moduleName, url, code, severity, msg) \
  class name : public Core::Notices::Notice \
  { \
    TYPE_INFO(name, Core::Notices::Notice, typeNamespace, moduleName, url); \
    public: using Core::Notices::Notice::Notice; \
    public: virtual Str const& getCode() const \
    { \
      static Str _code(code); \
      return _code; \
    } \
    public: virtual Int getSeverity() const \
    { \
      return severity; \
    } \
    public: virtual void buildDescription(Str &str) const \
    { \
      str = msg; \
    } \
  }


//==============================================================================
// Global Functions

namespace Core::Notices
{

/**
 * @brief Print the provided notices to the console.
 * @ingroup core_notices
 *
 * Printed notice includes severity, msg code, location, as well as
 * description.
 */
void printNotice(Notice const *msg);

} // namespace


//==============================================================================
// Main Classes

#include "Notice.h"
#include "Store.h"

#include "CustomNotice.h"


//==============================================================================
// Lexer Notices

#include "UnrecognizedCharNotice.h"

namespace Core::Notices
{

DEFINE_NOTICE(BufferFullNotice, "Core.Notices", "Core", "alusus.net", "CL1002", 1,
  STR("Lexer Error: Input buffer is full. A single token is too long to fit in the input buffer. "
      "The token may have been broken into more than one token.")
);
DEFINE_NOTICE(TokenClampedNotice, "Core.Notices", "Core", "alusus.net", "CL2003", 2,
  STR("Warning: Input buffer is full. A single token is too long to fit in the input buffer. "
      "Some characters that are part of the token has been ignored.")
);

} /// namespace


//==============================================================================
// Parser Notices

namespace Core::Notices
{

DEFINE_NOTICE(SyntaxErrorNotice, "Core.Notices", "Core", "alusus.net", "CP1001", 1,
  STR("Parser syntax error.")
);
DEFINE_NOTICE(UnexpectedEofNotice, "Core.Notices", "Core", "alusus.net", "CP1002", 1,
  STR("Parsing exited while needing more tokens.")
);
DEFINE_NOTICE(UnexpectedTokenNotice, "Core.Notices", "Core", "alusus.net", "CP1003", 1,
  STR("Parsing has already folded out to completion.")
);
DEFINE_NOTICE(AmbiguityNotice, "Core.Notices", "Core", "alusus.net", "CP2004", 2,
  STR("Ambiguity is causing state branching.")
);
DEFINE_NOTICE(UnexpectedModifierNotice, "Core.Notices", "Core", "alusus.net", "CP1005", 1,
  STR("Unexpected modifier encountered.")
);

} // namespace


//==============================================================================
// Parser Notices

namespace Core::Notices
{

DEFINE_NOTICE(MissingDefLinkNotice, "Core.Notices", "Core", "alusus.net", "CH1001", 1,
  STR("Missing link expression in 'def' command.")
);
DEFINE_NOTICE(MissingDefNameNotice, "Core.Notices", "Core", "alusus.net", "CH1002", 1,
  STR("Missing definition name in 'def' command.")
);
DEFINE_NOTICE(InvalidDefCommandNotice, "Core.Notices", "Core", "alusus.net", "CH1003", 1,
  STR("Invalid 'def' command.")
);

} // namespace


//==============================================================================
// Generic Notices

namespace Core::Notices
{

DEFINE_NOTICE(UnrecognizedErrorNotice, "Core.Notices", "Core", "alusus.net", "CG1001", 1,
  STR("Unrecognized error.")
);
DEFINE_NOTICE(InvalidDumpArgNotice, "Core.Standard", "Core", "alusus.net", "CG1002", 1,
  STR("Invalid argument for 'dump' command.")
);

} // namespace

#include "ImportLoadFailedNotice.h"
#include "RedefinitionNotice.h"

#endif
