/**
 * @file Scg/DebuggingHelpers/llvm_helper.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// STL header files
#include <iostream>

// LLVM header files
#include <llvm/Support/raw_ostream.h>

namespace Scg
{
void PrintLlvmValue(llvm::Value *value)
{
  std::string out;
  llvm::raw_string_ostream ostream(out);
  value->print(ostream);
  ostream << '\n';
  ostream.flush();
  std::cout << out;
}

//------------------------------------------------------------------------------

void PrintLlvmValueType(llvm::Value *value)
{
  std::string out;
  llvm::raw_string_ostream ostream(out);
  value->getType()->print(ostream);
  ostream << '\n';
  ostream.flush();
  std::cout << out;
}
}
