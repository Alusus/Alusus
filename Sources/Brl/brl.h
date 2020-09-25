/**
 * @file Brl/brl.h
 * The main header file of the Basic Runtime Library.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

// System header files
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#include <wctype.h>
#include <exception>
#include <sstream>
#include <cassert>

namespace Brl
{

/**
 * @defgroup brl Brl (Basic Runtime Library)
 */

//==============================================================================
// Basic Datatypes

/// @ingroup brl
typedef unsigned char Byte;

/// @ingroup brl
typedef char Char;

/// @ingroup brl
typedef wchar_t WChar;

/// @ingroup brl
typedef int Int;

/// @ingroup brl
typedef short ShortInt;

/// @ingroup brl
typedef long int LongInt;

/**
 * @brief An integer of the same size as pointers.
 * This is used for pointer arithmetics.
 * @ingroup brl
 */
typedef long int PtrInt;

/// @ingroup brl
typedef float Float;

/// @ingroup brl
typedef double Double;

/// @ingroup brl
typedef bool Bool;

/// @ingroup brl
typedef unsigned int Word;

/// @ingroup brl
typedef unsigned short ShortWord;

/// @ingroup brl
typedef unsigned long int LongWord;

/// @ingroup basic_datatypes
typedef std::stringstream StrStream;

/**
 * @brief A word of the same size as pointers.
 * This is used for things lik IDs that need to be the same size as pointers.
 * @ingroup brl
 */
typedef unsigned long int PtrWord;

//==============================================================================
// Basic Macros

/**
 * @brief Wrapper for string literals.
 * @ingroup brl
 *
 * This wrapper is needed for future purposes. The main purpose of this is to
 * allow easy conversion of the program from ascii to unicode. Every string
 * literal in the source code should be wrapped by this macro.
 */
#define S(x)	u8##x

/**
 * @brief Wrapper for character literals.
 * @ingroup brl
 *
 * This wrapper is needed for future purposes. The main purpose of this is to
 * allow easy conversion of the program from ascii to unicode. Every character
 * literal in the source code should be wrapped by this macro.
 */
#define C(x)	u8##x

/**
 * @brief Wrapper for wide character literals.
 * @ingroup brl
 */
#define WC(x) U##x

/**
 * @brief Maps to the standard C++ assert function.
 * @ingroup brl
 */
#define ASSERT(x) assert(x)

} // namespace

// Alusus Brl header files

#include "Array.h"
#include "refs.h"
#include "strs.h"
#include "exceptions.h"
#include "ArrayIndex.h"
#include "Map.h"

// Since basic datatypes should be available everywhere, we'll just open up the namespace.
using namespace Brl;
