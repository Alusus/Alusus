/**
 * @file Core/Basic/Finally.h
 * Contains the header of class Core::Basic::Finally.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_FINALLY_H
#define CORE_BASIC_FINALLY_H

namespace Core { namespace Basic
{

class Finally
{
  //============================================================================
  // Member Variables

  private: std::function<void(void)> lambda;


  //============================================================================
  // Constructor & Destructor

  public: Finally(std::function<void(void)> l) : lambda(l)
  {
  }

  public: ~Finally()
  {
    this->lambda();
  }

}; // class


//==============================================================================
// Macros

#define finally(lambda) Core::Basic::Finally __finally(lambda);

} } // namespace

#endif
