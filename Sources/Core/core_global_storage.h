/**
 * @file Core/core_global_storage.h
 * Contains functions headers for setting and getting global storage.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

extern "C" void* getCoreGlobalStorage();

extern "C" void setCoreGlobalStorage(void *strg);
