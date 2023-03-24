/**
 * @file Spp/CodeGen/DestructionNode.h
 * Contains the header of class Spp::CodeGen::DestructionNode.
 *
 * @copyright Copyright (C) 2023 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_DESTRUCTIONNODE_h
#define SPP_CODEGEN_DESTRUCTIONNODE_h

namespace Spp::CodeGen
{

class DestructionNode : public TiObject
{
  TYPE_INFO(DestructionNode, TiObject, "Spp.CodeGen", "Spp", "alusus.org");

  Core::Data::Node *astNode;
  Ast::Type *astType;
  TioSharedPtr tgVar;
};

} // namespace

#endif
