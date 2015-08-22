/**
 * @file Scg/Operators/Size.cpp
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>
// LLVM header files
#include <llvm/IR/IRBuilder.h>
// Scg files
#include <Operators/IdentifierReference.h>
#include <Containers/Block.h>
#include <Operators/Content.h>
#include <Operators/Size.h>
#include <Types/PointerType.h>
#include <Types/IntegerType.h>

namespace Scg
{

Size::Size(Expression *exp) : expression(exp), llvmValue(nullptr)
{
  // If this given expression is IdentifierReference we won't add it to the children, instead
  // we will manually lookup the referenced type by name. Otherwise, we will add it and use
  // its Expression::getValueTypeSpec function to determine the type.
  IdentifierReference *ref = dynamic_cast<IdentifierReference*>(this->expression);

  if (!ref) children.push_back(this->expression);
}


const ValueTypeSpec *Size::getValueTypeSpec() const
{
  return IntegerType::get()->getValueTypeSpec();
}


Expression::CodeGenerationStage Size::generateCode()
{
  IdentifierReference *ref = dynamic_cast<IdentifierReference*>(this->expression);
  ValueType const *type = 0;

  // If the provided expression is just an identifier, then we will lookup the type in the list of
  // varialbes of the current block and the list of types in the current module.
  if (ref != 0) {
    Variable const *var;

    if ((var = getBlock()->getVariable(ref->getName())) != 0) {
      type = PointerType::get(var->getValueTypeSpec()->toValueType(*getModule()));
      // For variables the returned type is actually the pointer to that variable, so we need to get
      // the actual type of the variable, not the type of its pointer.
      auto ptype = dynamic_cast<PointerType const*>(type);

      if (ptype != 0) type = ptype->getContentType();
    } else if ((type = getModule()->tryGetValueTypeByName(ref->getName())) == 0) {
      throw EXCEPTION(UndefinedVariableException,
                      ("Referencing undefined identifier: " + ref->getName()).c_str());
    }
  } else {
    type = this->expression->getValueTypeSpec()->toValueType(*getModule());
  }

  this->generatedLlvmValue = this->llvmValue = IntegerType::get()->getLlvmConstant(type->getAllocationSize());
  return Expression::generateCode();
}


Expression::CodeGenerationStage Size::postGenerateCode()
{
  if (this->llvmValue == nullptr)
    // Nothing to delete
    return CodeGenerationStage::None;

  if (!this->llvmValue->hasNUses(0))
    // The value is still in use; stay in PostCodeGeneration stage.
    return CodeGenerationStage::PostCodeGeneration;

  // We don't need to delete the constant, it is handled by LLVM.
  this->llvmValue = nullptr;
  return CodeGenerationStage::None;
}


std::string Size::toString()
{
  // TODO: Implement this function.
  return "";
}

} // namespace
