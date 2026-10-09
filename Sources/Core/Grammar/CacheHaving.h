/**
 * @file Core/Grammar/CacheHaving.h
 * Contains the header of interface Core::Grammar::CacheHaving.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_GRAMMAR_CACHEHAVING_H
#define CORE_GRAMMAR_CACHEHAVING_H

namespace Core::Grammar
{

// TODO: DOC

class CacheHaving : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(CacheHaving, TiInterface, "Core.Grammar", "Core", "alusus.org");


  //============================================================================
  // Abstract Functions

  public: virtual void clearCache() = 0;

}; // class

} // namespace

#endif
