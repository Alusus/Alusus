/**
 * @file Spp/CodeGen/GlobalCtorDtorInfo.h
 * Contains the header of class Spp::CodeGen::GlobalCtorDtorInfo.
 *
 * @copyright Copyright (C) 2025 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_GLOBALCTORDTORINFO_H
#define SPP_CODEGEN_GLOBALCTORDTORINFO_H

namespace Spp { namespace CodeGen
{

struct GlobalCtorDtorInfo
{
  public: Str name;
  public: Array<Str> varNames;
  GlobalCtorDtorInfo() {}
  GlobalCtorDtorInfo(Str const &n): name(n) {}
};

} } // namespace

#endif
