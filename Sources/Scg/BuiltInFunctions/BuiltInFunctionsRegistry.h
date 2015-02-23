/**
* @file Scg/BuiltInFunctions/Callable.h
*
* @copyright Copyright (C) 2015 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef __BuiltInFunctionsRegistry_h__
#define __BuiltInFunctionsRegistry_h__

// SCG header files
#include <Functions/FunctionStore.h>

namespace Scg
{
// /**
// * A singleton class containing all registered built-in functions which are
// * available to any Alusus program.
// */
// class BuiltInFunctionsRegistry
// {
// private:
//   //! A function store containing all registered functions.
//   FunctionStore registeredFunctions;
//
// private:
//   /**
//   * Default class constructor. This cannot be instantiated by the user because
//   * it is a singleton.
//   */
//   BuiltInFunctionsRegistry();
//
//   /**
//   * Default class destructor.
//   */
//   ~BuiltInFunctionsRegistry()
//   {
//   }
//
// private:
//   static BuiltInFunctionsRegistry _theRegistry;
//   static bool _theRegistryInitialised;
//
// public:
//   /**
//   * Retrieves the single instance of this class.
//   * @return A reference to the single instance of this class.
//   */
//   BuiltInFunctionsRegistry &getSingleton()
//   {
//     if (_theRegistry) {
//       _theRegistry = new
//     }
//
//     return _theRegistry;
//   }
// };
}

#endif // __Callable_h__
