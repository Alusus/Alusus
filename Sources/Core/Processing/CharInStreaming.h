/**
 * @file Core/Processing/CharInStreaming.h
 * Contains the header of class Core::Processing::CharInStreaming.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_CHARINSTREAMING_H
#define CORE_PROCESSING_CHARINSTREAMING_H

namespace Core::Processing
{

class CharInStreaming : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(CharInStreaming, TiInterface, "Core.Processing", "Core", "alusus.org");


  //============================================================================
  // Member Functions

  public: virtual Char get() = 0;
  public: virtual Bool isEof() = 0;

};

} // namespace

#endif
