/**
 * @file Core/Basic/basic.h
 * Contains the definitions and include statements of all types in the Basic
 * namespace.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_BASIC_H
#define CORE_BASIC_BASIC_H

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
#define STR(x)	u8##x

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
 * @brief Wrapper for wide character literals.
 * @ingroup basic_macros
 */
#define WCHR(x) U##x

/**
 * @brief Maps to the standard C++ assert function.
 * @ingroup basic_macros
 */
#define ASSERT(x) assert(x)

/**
 * @brief Select a macro based on number of arguments.
 * @ingroup basic_macros
 *
 * This helper macro selects one of ten provided macros based on the number of
 * arguments provided in a variable-args macro.
 *
 * Usage: SELECT_MACRO(__VA_ARGS__, macro10, macro9, macro8 ...)
 */
#define SELECT_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, macro, ...) macro

/**
 * @def SPACE_EXPAND_ARGS(...)
 * @brief Expands a variadic macro into sequential elements separated by spaces.
 * @ingroup basic_macros
 */
#define SPACE_EXPAND_ARGS1(arg1)                               arg1
#define SPACE_EXPAND_ARGS2(arg1, arg2)                         arg1 arg2
#define SPACE_EXPAND_ARGS3(arg1, arg2, arg3)                   arg1 arg2 arg3
#define SPACE_EXPAND_ARGS4(arg1, arg2, arg3, arg4)             arg1 arg2 arg3 arg4
#define SPACE_EXPAND_ARGS5(arg1, arg2, arg3, arg4, arg5)       arg1 arg2 arg3 arg4 arg5
#define SPACE_EXPAND_ARGS6(arg1, arg2, arg3, arg4, arg5, arg6) arg1 arg2 arg3 arg4 arg5 arg6
#define SPACE_EXPAND_ARGS(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               SPACE_EXPAND_ARGS6, \
               SPACE_EXPAND_ARGS5, \
               SPACE_EXPAND_ARGS4, \
               SPACE_EXPAND_ARGS3, \
               SPACE_EXPAND_ARGS2, \
               SPACE_EXPAND_ARGS1)(__VA_ARGS__)

/**
 * @def SEMICOLON_EXPAND_ARGS(...)
 * @brief Expands a variadic macro into sequential elements separated by spaces.
 * @ingroup basic_macros
 */
#define SEMICOLON_EXPAND_ARGS0()
#define SEMICOLON_EXPAND_ARGS1(arg1)                               arg1
#define SEMICOLON_EXPAND_ARGS2(arg1, arg2)                         arg1; arg2
#define SEMICOLON_EXPAND_ARGS3(arg1, arg2, arg3)                   arg1; arg2; arg3
#define SEMICOLON_EXPAND_ARGS4(arg1, arg2, arg3, arg4)             arg1; arg2; arg3; arg4
#define SEMICOLON_EXPAND_ARGS5(arg1, arg2, arg3, arg4, arg5)       arg1; arg2; arg3; arg4; arg5
#define SEMICOLON_EXPAND_ARGS6(arg1, arg2, arg3, arg4, arg5, arg6) arg1; arg2; arg3; arg4; arg5; arg6
#define SEMICOLON_EXPAND_ARGS(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               SEMICOLON_EXPAND_ARGS6, \
               SEMICOLON_EXPAND_ARGS5, \
               SEMICOLON_EXPAND_ARGS4, \
               SEMICOLON_EXPAND_ARGS3, \
               SEMICOLON_EXPAND_ARGS2, \
               SEMICOLON_EXPAND_ARGS1, \
               SEMICOLON_EXPAND_ARGS0)(__VA_ARGS__)

/**
 * @def COMMA_EXPAND_ARGS(...)
 * @brief Expands a variadic macro into sequential elements separated by commas.
 * @ingroup basic_macros
 */
#define COMMA_EXPAND_ARGS(...) __VA_ARGS__

/**
 * @brief Selects the variable name from a var tuple.
 * The tuple consists of: type, name, setter, getter, hold method.
 * @ingroup basic_macros
 */
#define VARNAME_FROM_TUPLE(name, ...) name

/**
 * @brief Selects the variable name as a string from a type/name pair.
 * The tuple consists of: type, name, setter, getter, hold type.
 * @ingroup basic_macros
 */
#define VARNAMESTR_FROM_TUPLE(name, ...) #name

/**
 * @brief Selects the variable type from a var tuple.
 * The tuple consists of: type, name, setter, getter, hold method.
 * @ingroup basic_macros
 */
#define VARTYPE_FROM_TUPLE(name, type, ...) type

/**
 * @brief Selects the variable hold type from a var tuple.
 * The tuple consists of: type, name, setter, getter, hold method.
 * @ingroup basic_macros
 */
#define VARHOLDMODE_FROM_TUPLE(name, type, holdMode, ...) holdMode

/**
 * @brief Selects the variable's setter from a var tuple.
 * The tuple consists of: type, name, setter, getter, hold method.
 * @ingroup basic_macros
 */
#define VARSETTER_FROM_TUPLE(name, type, holdMode, setter, ...) setter

/**
 * @brief Selects the variable's getter from a var tuple.
 * The tuple consists of: type, name, setter, getter, hold method.
 * @ingroup basic_macros
 */
#define VARGETTER_FROM_TUPLE(name, type, holdMode, setter, getter) getter

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
 * s_enum(My_Type, VAL1, VAL2=8, V3);
 */
#define s_enum(x,...) \
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
    bool operator >(x v) const { return this->val > v.val; } \
    bool operator >=(x v) const { return this->val >= v.val; } \
    bool operator >(_##x v) const { return this->val > v; } \
    bool operator >=(_##x v) const { return this->val >= v; } \
    bool operator <(x v) const { return this->val < v.val; } \
    bool operator <=(x v) const { return this->val <= v.val; } \
    bool operator <(_##x v) const { return this->val < v; } \
    bool operator <=(_##x v) const { return this->val <= v; } \
  }

/**
 * @brief Defines an RttiObject based enumeration.
 * @ingroup basic_macros
 *
 * This is similar to s_enum, but it derives the new type from RttiObject or one
 * of its children.
 *
 * @sa s_enum
 * @sa RttiObject
 */
#define ti_s_enum(x, p, typeNamespace, moduleName, url, ...) \
  class x : public p \
  { \
    TYPE_INFO(x, p, typeNamespace, moduleName, url); \
    public: \
      enum _##x {__VA_ARGS__}; \
      x() : p(0) {} \
      x(_##x v) : p(v) {} \
      const x& operator=(_##x v) { this->set(v); return *this; } \
      const x& operator=(x const &v) { this->set(v.get()); return *this; } \
      bool operator ==(x const &v) const { return this->get() == v.get(); } \
      bool operator !=(x const &v) const { return this->get() != v.get(); } \
      bool operator ==(_##x v) const { return this->get() == v; } \
      bool operator !=(_##x v) const { return this->get() != v; } \
      bool operator >(x const &v) const { return this->get() > v.get(); } \
      bool operator >=(x const &v) const { return this->get() >= v.get(); } \
      bool operator >(_##x v) const { return this->get() > v; } \
      bool operator >=(_##x v) const { return this->get() >= v; } \
      bool operator <(x const &v) const { return this->get() < v.get(); } \
      bool operator <=(x const &v) const { return this->get() <= v.get(); } \
      bool operator <(_##x v) const { return this->get() < v; } \
      bool operator <=(_##x v) const { return this->get() <= v; } \
      _##x get() const { return static_cast<_##x>(p::get()); } \
  }

/**
 * @def SALLOC
 * @brief Allocate memory on the stack.
 * @ingroup basic_macros
 * @param s Size of memory to allocate, in bytes.
 *
 * Allocation and deallocation on the stack is faster than on the heap, but
 * should only be used in cases where the allocated memory is relatively
 * small.
 */
#ifdef WINDOWS
  #define SALLOC(s) alloca(s)
#else
  #define SALLOC(s) alloca(s)
#endif

/**
 * @def SFREE
 * @brief Free memory allocated on stack.
 * @ingroup basic_macros
 * @sa SALLOC
 */
#ifdef WINDOWS
  #define SFREE(p) _freea(p);
#else
  #define SFREE(p)
#endif

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
s_enum(LogLevel,
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
typedef wchar_t WChar;

/// @ingroup basic_datatypes
typedef int Int;

/// @ingroup basic_datatypes
typedef long long int Long;

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
typedef double Double;

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

/// @ingroup basic_datatypes
typedef std::ostream OutStream;

/// @ingroup basic_datatypes
typedef std::istream InStream;

/// @ingroup basic_datatypes
s_enum(ContentChangeOp, ADDED, WILL_UPDATE, UPDATED, WILL_REMOVE, REMOVED);


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
 * The main purpose of this is to support both regular and wide character types.
 */
inline Int compareStr(Char const *str1, Char const *str2)
{
    return strcmp(str1, str2);
}

/**
 * @brief Wrapper for wide string comparison function.
 * @ingroup basic_functions.
 *
 * The main purpose of this is to support both regular and wide character types.
 */
inline Int compareStr(WChar const *str1, WChar const *str2)
{
  return wcscmp(str1, str2);
}

/**
 * @brief Wrapper for string comparison function.
 * @ingroup basic_functions
 *
 * The main purpose of this is to support both regular and wide character types.
 */
inline Int compareStr(Char const *str1, Char const *str2, Int size)
{
    return strncmp(str1, str2, size);
}

/**
 * @brief Wrapper for wide string comparison function.
 * @ingroup basic_functions
 *
 * The main purpose of this is to support both regular and wide character types.
 */
inline Int compareStr(WChar const *str1, WChar const *str2, Int size)
{
  return wcsncmp(str1, str2, size);
}

/**
 * @brief Wrapper for string copy function.
 * @ingroup basic_functions
 *
 * The main purpose of this is to support both regular and wide character types.
 */
inline void copyStr(Char const *src, Char *dest)
{
    strcpy(dest, src);
}

/**
 * @brief Wrapper for string copy function.
 * @ingroup basic_functions
 *
 * The main purpose of this is to support both regular and wide character types.
 */
inline void copyStr(Char const *src, Char *dest, Word size)
{
    strncpy(dest, src, size);
}

/**
 * @brief Wrapper for wide string copy function.
 * @ingroup basic_functions
 *
 * The main purpose of this is to support both regular and wide character types.
 */
inline void copyStr(WChar const *src, WChar *dest)
{
    wcscpy(dest, src);
}

/**
 * @brief Wrapper for wide string copy function.
 * @ingroup basic_functions
 *
 * The main purpose of this is to support both regular and wide character types.
 */
inline void copyStr(WChar const *src, WChar *dest, Word size)
{
    wcsncpy(dest, src, size);
}

/**
 * @brief Wrapper for string length function.
 * @ingroup basic_functions
 *
 * The main purpose of this is to support both regular and wide character types.
 */
inline Word getStrLen(Char const *str)
{
    return strlen(str);
}

/**
 * @brief Wrapper for wide string length function.
 * @ingroup basic_functions
 *
 * The main purpose of this is to support both regular and wide character types.
 */
inline Word getStrLen(WChar const *str)
{
    return wcslen(str);
}

/**
 * @brief Convert string from utf8 to wide characters.
 * @ingroup basic_functions
 *
 * @param input Input utf8 string.
 * @param inputLength Length of input string.
 * @param output Pointer to array to hold the output.
 * @param outputSize Size of output array.
 * @param processedInputLength Number of input characters processed.
 * @param resultedOutputLength Number of resulting output characters.
 */
void convertStr(Char const *input, int inputLength, WChar *output, int outputSize, int &processedInputLength, int &resultedOutputLength);

/**
 * @brief Convert string from wide characters to utf8.
 * @ingroup basic_functions
 *
 * @param input Input wide characters string.
 * @param inputLength Length of input string.
 * @param output Pointer to array to hold the output utf8 results.
 * @param outputSize Size of output array.
 * @param processedInputLength Number of input characters processed.
 * @param resultedOutputLength Number of resulting output characters.
 */
void convertStr(WChar const *input, int inputLength, Char *output, int outputSize, int &processedInputLength, int &resultedOutputLength);

/**
 * @brief Get the wide character for a given UTF8 sequence.
 * @ingroup basic_functions
 */
WChar getWideCharFromUtf8(Char const *s);

/// Print 'indents' number of spaces.
void printIndents(OutStream &stream, int indents);


//==============================================================================
// Global Variable Definitions

/**
 * @brief A reference to std::cout to use to output to the console.
 * @ingroup basic
 *
 * This should be used to output to the console instead of directly using
 * std::cout, in order to minimize the needed changes in case we needed to
 * support wide characters in the output stream.
 */
extern std::ostream &outStream;


/**
 * @brief A reference to std::cin to use to input from the console.
 * @ingroup basic
 *
 * This should be used to input from the console instead of directly using
 * std::cin, in order to minimize the needed changes in case we needed to
 * support wide characters in the input stream.
 */
extern std::istream &inStream;


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
#include "SbWStr.h"
#include "Str.h"
#include "WStr.h"

#include "Logger.h"
#include "exceptions.h"
#include "validation.h"

#include "SortedIndex.h"
#include "default_sorted_indices.h"

#include "GlobalStorage.h"

#include "type_names.h"
#include "type_info.h"
#include "ti_declarers.h"
#include "TiObject.h"
#include "TiInterface.h"
#include "ObjTiInterface.h"
#include "ti_casting.h"

#include "SharedPtr.h"
#include "WeakPtr.h"

#include "ti_object_factories.h"

#include "Finally.h"
#include "signals.h"

#include "Argument.h"

#include "Container.h"
#include "ListContainer.h"
#include "MapContainer.h"

#include "SharedList.h"
#include "PlainList.h"
#include "SharedMap.h"
#include "PlainMap.h"
#include "Box.h"

#include "TiNumber.h"
#include "TiStr.h"
#include "TiBool.h"
#include "ti_functions.h"

#include "Bindings.h"
#include "BindingMap.h"
#include "bindings_helpers.h"
#include "DynamicBindings.h"
#include "binding_caches.h"

#include "ObjTiInterfaceList.h"
#include "DynamicInterfaces.h"

#endif
