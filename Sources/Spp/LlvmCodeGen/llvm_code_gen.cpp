/**
 * @file Spp/LlvmCodeGen/llvm_code_gen.cpp
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::LlvmCodeGen
{

void llvmDiagnosticCallback(const llvm::DiagnosticInfo &di, void *context)
{
  std::string msg;
  llvm::raw_string_ostream stream(msg);
  llvm::DiagnosticPrinterRawOStream dpstream(stream);

  di.print(dpstream);
  stream.flush();

  outStream << msg << "\n";
}

}
