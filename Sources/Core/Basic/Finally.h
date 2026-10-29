/**
 * @file Core/Basic/Finally.h
 * Contains the header of class Core::Basic::Finally.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_FINALLY_H
#define CORE_BASIC_FINALLY_H

namespace Core { namespace Basic
{

// This is a template on the lambda type rather than holding a std::function to avoid the overhead of type erasure,
// since this class is used in hot paths like signal emitting.
template <class F> class Finally
{
  //============================================================================
  // Member Variables

  private: F lambda;


  //============================================================================
  // Constructor & Destructor

  public: Finally(F l) : lambda(l)
  {
  }

  public: Finally(Finally const&) = delete;

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
