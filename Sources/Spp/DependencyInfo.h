/**
 * @file Spp/DependencyInfo.h
 * Contains the header of class Spp::DependencyInfo.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_DEPENDENCYINFO_H
#define SPP_DEPENDENCYINFO_H

namespace Spp
{

struct DependencyInfo
{
  public: DependencyList<Core::Data::Node> globalVarInitializationDeps;
  public: DependencyList<Core::Data::Node> globalVarDestructionDeps;
  public: DependencyList<Ast::Function> funcDeps;
};

} // namespace

#endif
