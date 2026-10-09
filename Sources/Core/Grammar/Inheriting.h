/**
 * @file Core/Grammar/Inheriting.h
 * Contains the header of interface Core::Grammar::Inheriting.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_GRAMMAR_INHERITING_H
#define CORE_GRAMMAR_INHERITING_H

namespace Core::Grammar
{

// TODO: DOC

class Inheriting : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Inheriting, TiInterface, "Core.Grammar", "Core", "alusus.org");


  //============================================================================
  // Abstract Functions

  public: virtual Reference* getBaseReference() const = 0;

  public: virtual void setBase(TiObject *base) = 0;

  public: virtual TiObject* getBase() const = 0;

}; // class

} // namespace

#endif
