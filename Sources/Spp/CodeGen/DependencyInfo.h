/**
 * @file Spp/CodeGen/DependencyInfo.h
 * Contains the header of class Spp::CodeGen::DependencyInfo.
 *
 * @copyright Copyright (C) 2025 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_DEPENDENCYINFO_H
#define SPP_CODEGEN_DEPENDENCYINFO_H

namespace Spp::CodeGen
{

struct DependencyInfo
{
  public: DependencyList<Core::Data::Node> globalVarInitializationDeps;
  public: DependencyList<Core::Data::Node> globalVarDestructionDeps;
  public: DependencyList<Ast::Function> funcDeps;
};

} // namespace

#endif
