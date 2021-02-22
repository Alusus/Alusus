/**
 * @file Srl/srl.h
 * The main header file of the Basic Runtime Library.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
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

namespace Srl
{

/**
 * @defgroup srl Srl (Basic Runtime Library)
 */

//==============================================================================
// Basic Datatypes

/// @ingroup srl
typedef unsigned char Byte;

/// @ingroup srl
typedef char Char;

/// @ingroup srl
typedef wchar_t WChar;

/// @ingroup srl
typedef int Int;

/// @ingroup srl
typedef short ShortInt;

/// @ingroup srl
typedef long int LongInt;

/**
 * @brief An integer of the same size as pointers.
 * This is used for pointer arithmetics.
 * @ingroup srl
 */
typedef long int ArchInt;

/// @ingroup srl
typedef float Float;

/// @ingroup srl
typedef double Double;

/// @ingroup srl
typedef bool Bool;

/// @ingroup srl
typedef unsigned int Word;

/// @ingroup srl
typedef unsigned short ShortWord;

/// @ingroup srl
typedef unsigned long int LongWord;

/// @ingroup basic_datatypes
typedef std::stringstream StrStream;

/**
 * @brief A word of the same size as pointers.
 * This is used for things lik IDs that need to be the same size as pointers.
 * @ingroup srl
 */
typedef unsigned long int PtrWord;

//==============================================================================
// Basic Macros

/**
 * @brief Wrapper for string literals.
 * @ingroup srl
 *
 * This wrapper is needed for future purposes. The main purpose of this is to
 * allow easy conversion of the program from ascii to unicode. Every string
 * literal in the source code should be wrapped by this macro.
 */
#define S(x)	u8##x

/**
 * @brief Wrapper for character literals.
 * @ingroup srl
 *
 * This wrapper is needed for future purposes. The main purpose of this is to
 * allow easy conversion of the program from ascii to unicode. Every character
 * literal in the source code should be wrapped by this macro.
 */
#define C(x)	u8##x

/**
 * @brief Wrapper for wide character literals.
 * @ingroup srl
 */
#define WC(x) U##x

/**
 * @brief Maps to the standard C++ assert function.
 * @ingroup srl
 */
#define ASSERT(x) assert(x)

} // namespace

// Alusus Srl header files

#include "Array.h"
#include "refs.h"
#include "strs.h"
#include "exceptions.h"
#include "ArrayIndex.h"
#include "Map.h"

// Since basic datatypes should be available everywhere, we'll just open up the namespace.
using namespace Srl;
