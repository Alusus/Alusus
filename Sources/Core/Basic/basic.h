/**
 * @file Core/Basic/basic.h
 * Contains the definitions and include statements of all types in the Basic
 * namespace.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef BASIC_BASIC_H
#define BASIC_BASIC_H

namespace Core { namespace Basic
{

/**
 * @defgroup basic Basic
 * @ingroup core
 * @brief Basic types and definitions used throughout the core.
 * This namespace contains definitions for basic datatypes and other definitions
 * that are generic, lower level, and not part of the core's design.
 */


//==============================================================================
// Basic Datatypes

/**
 * @defgroup basic_datatypes Basic Datatypes
 * @ingroup basic
 * @brief Wrapper definitions for basic datatypes.
 */

/// @ingroup basic_datatypes
typedef unsigned char Byte;

/// @ingroup basic_datatypes
typedef char Char;

/// @ingroup basic_datatypes
typedef int Int;

/// @ingroup basic_datatypes
typedef short ShortInt;

/**
 * @brief An integer of the same size as pointers.
 * This is used for pointer arithmetics.
 * @ingroup basic_datatypes
 */
typedef long long int PtrInt;

/// @ingroup basic_datatypes
typedef float Float;

/// @ingroup basic_datatypes
typedef bool Bool;

/// @ingroup basic_datatypes
typedef unsigned int Word;

/// @ingroup basic_datatypes
typedef unsigned short ShortWord;

/**
 * @brief A word of the same size as pointers.
 * This is used for things lik IDs that need to be the same size as pointers.
 * @ingroup basic_datatypes
 */
typedef unsigned long long int PtrWord;

/// @ingroup basic_datatypes
typedef std::stringstream StrStream;


//==============================================================================
// Basic Constants

/**
 * @defgroup basic_constants Basic Constants
 * @ingroup basic
 * @brief Wrapper definitions for basic constants.
 */

/// @ingroup basic_constants
#define STRING_TERMINATOR   '\0'

/**
 * @brief Marks the end of a file.
 * @ingroup basic_constants
 *
 * This value is passed to the lexer at the end of a file to inform the lexer
 * that no more characters are to be expected from this file.
 */
#define FILE_TERMINATOR     1

/**
 * @brief Defines a new line character.
 * @ingroup basic_constants
 *
 * This constant is needed for future purposes. The main purpose of this is to
 * allow easy conversion of the program from ascii to unicode.
 */
#define NEW_LINE            '\n'

/**
 * @brief Defines a generic unknown identifier.
 * @ingroup basic_constants
 *
 * This defines an unknown ID and can be used with all kinds of objects,
 * whether it's a token definition, a production definition, or any similar
 * use.
 */
#define UNKNOWN_ID          0x0

/**
 * @brief The bit index at which flags bits start.
 * @ingroup basic_constants
 *
 * Flags bits are a group of bits within an integer (an id or an index) that are
 * not part of the number itself, instead they are used as flags for special
 * purposes.
 * This is the index of the least significant bit in the flags group of bits.
 * The flags group starts at this index and ends at the most significant bit
 * of the integer.
 */
#define FLAGS_BITS_INDEX    (sizeof(Int)*8-4)

/**
 * @brief A mask for all reserved flags in IDs or indexes.
 * @ingroup basic_constants
 *
 * Flags bits are a group of bits within an integer (an id or an index) that are
 * not part of the number itself, instead they are used as flags for special
 * purposes.
 * Any ID or index should avoid using these flags because they are reserved for
 * other purposes.
 */
#define FLAGS_BITS_MASK     (0xF << FLAGS_BITS_INDEX)


//==============================================================================
// Basic Macros

/**
 * @defgroup basic_macros Basic Macros
 * @ingroup basic
 * @brief Wrapper definitions for basic macros.
 */

/**
 * @brief Wrapper for string literals.
 * @ingroup basic_macros
 *
 * This wrapper is needed for future purposes. The main purpose of this is to
 * allow easy conversion of the program from ascii to unicode. Every string
 * literal in the source code should be wrapped by this macro.
 */
#define STR(x)	x

/**
 * @brief Wrapper for character literals.
 * @ingroup basic_macros
 *
 * This wrapper is needed for future purposes. The main purpose of this is to
 * allow easy conversion of the program from ascii to unicode. Every character
 * literal in the source code should be wrapped by this macro.
 */
#define CHR(x)	x

/**
 * @brief Maps to the standard C++ assert function.
 * @ingroup basic_macros
 */
#define ASSERT(x) assert(x)

/**
 * @brief Defines an enumeration.
 * @ingroup basic_macros
 *
 * Unlike built-in enumerations, this enumeration encapsulates the enum values
 * rather than keeping them available in the parent scope. This is implemented
 * by wrapping the enum inside a structure.<br>
 * <br>
 * Usage: this macro takes the type name in the first argument, followed by
 * a variable list of arguments defining the enum values. Ex:<br>
 * enumeration(My_Type, VAL1, VAL2=8, V3);
 */
#define enumeration(x,...) \
  struct x \
  { \
    enum _##x {__VA_ARGS__} val; \
    x() : val((_##x)0) {} \
    x(_##x v) : val(v) {} \
    const x& operator=(_##x v) { this->val = v; return *this; } \
    bool operator ==(x v) const { return this->val == v.val; } \
    bool operator !=(x v) const { return this->val != v.val; } \
    bool operator ==(_##x v) const { return this->val == v; } \
    bool operator !=(_##x v) const { return this->val != v; } \
  }

/**
 * @def DL_EXPORTED
 * @brief Macro for marking dynamic library's exported symbols.
 * @ingroup basic_macros
 */
#ifdef _MSC_VER
#define DL_EXPORTED extern "C" __declspec(dllexport)
#else
#define DL_EXPORTED extern "C" __attribute__((__visibility__("default")))
#endif


//==============================================================================
// Logging Definitions

#ifdef USE_LOGS

/**
 * @def LOG(l,x)
 * @brief Sends a log message to the screen.
 * @ingroup basic_macros
 *
 * To enable this macro, the USE_LOGS flag must be set, otherwise this macro
 * will do nothing. To set the flag do:<br>
 * make &lt;target&gt; "CFLAGS=-DUSE_LOGS"
 * <br><br>
 * The generated log message has this format:<br>
 * &lt;source_file&gt;(&lt;line_number&gt;): &lt;log_msg&gt;
 *
 * @param l The level of this log message. This will be bitwise filtered in with
 *          Logger::filter to enable controlling the level of details in the
 *          log output.
 * @param x The value to be logged. This can be a combination of values using the
 *          &lt;&lt; operator. Ex: LOG(1, "error: " &lt;&lt; code);
 */
#define LOG(l,x)  Logger(l) << __FILE__ << "(" << __LINE__ << "): " << x << NEW_LINE

/**
 * @brief Constant definitions for different levels of log messages.
 * @ingroup basic_macros
 *
 * Each log message should have a level value associated with it. This level
 * value is used to filter in/out log messages depending on how much details
 * are needed by the user. It's up to the programmar what level to assign to
 * his log message. Note that these levels doesn't cover errors or warnings
 * because those should be caught by assertions and exceptions. Possible values
 * are LEXER_MINOR, LEXER_MID, LEXER_MAJOR, PARSER_MINOR, PARSER_MID, and
 * PARSER_MAJOR.
 *
 * &lt;x&gt;_MINOR: Refers to a minor event. This should be used for messages
 *                 that can give the reader intricate details of the process.
 * <br>
 * &lt;x&gt;_MID: Refers to mid level events.<br>
 * &lt;x&gt;_MAJOR: Refers to major events. This should be used for messages
 *                 that can give the reader simple high level info.
 */
enumeration(LogLevel,
    LEXER_MINOR = 1,
    LEXER_MID = 2,
    LEXER_MAJOR = 4,
    PARSER_MINOR = 8,
    PARSER_MID = 16,
    PARSER_MAJOR = 32
);

#else

#define LOG(l,x)

#endif


//==============================================================================
// Function Definitions

/**
 * @defgroup basic_functions Basic Functions
 * @ingroup basic
 * @brief Wrapper definitions for basic functions.
 */

/**
 * @brief Wrapper for string comparison function.
 * @ingroup basic_functions
 *
 * This wrapper is needed for future purposes. The main purpose of this is to
 * allow easy conversion of the program from ascii to unicode.
 */
inline Int compareStr(Char const *str1, Char const *str2)
{
    return strcmp(str1, str2);
}

/**
 * @brief Wrapper for string comparison function.
 * @ingroup basic_functions
 *
 * This wrapper is needed for future purposes. The main purpose of this is to
 * allow easy conversion of the program from ascii to unicode.
 */
inline Int compareStr(Char const *str1, Char const *str2, Int size)
{
    return strncmp(str1, str2, size);
}

/**
 * @brief Wrapper for string copy function.
 * @ingroup basic_functions
 *
 * This wrapper is needed for future purposes. The main purpose of this is to
 * allow easy conversion of the program from ascii to unicode.
 */
inline void copyStr(Char const *src, Char *dest, Word size)
{
    strncpy(dest, src, size);
}

/**
 * @brief Wrapper for string length function.
 * @ingroup basic_functions
 *
 * This wrapper is needed for future purposes. The main purpose of this is to
 * allow easy conversion of the program from ascii to unicode.
 */
inline Word getStrLen(Char const *str)
{
    return strlen(str);
}


//==============================================================================
// Other Group Definitions

/**
 * @defgroup basic_utils Basic Utilities
 * @ingroup basic
 * @brief Wrapper definitions for basic utility classes.
 */

/**
 * @defgroup basic_exceptions Basic Exceptions
 * @ingroup basic
 * @brief Wrapper definitions for basic exception classes.
 */

} } // namespace


//==============================================================================
// Headers

#include "SbStr.h"
#include "Str.h"
#include "Logger.h"
#include "exceptions.h"
#include "SortedIndex.h"
#include "default_sorted_indices.h"
#include "GlobalStorage.h"
#include "TypeInfo.h"
#include "IdentifiableObject.h"
#include "IdentifiableInterface.h"
#include "SharedPtr.h"
#include "WeakPtr.h"
#include "SignalReverseConnector.h"
#include "SignalReceiver.h"
#include "signals.h"
#include "arguments.h"

#endif
