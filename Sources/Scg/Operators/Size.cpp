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
  // its Expression::GetValueTypeSpec function to determine the type.
  IdentifierReference *ref = dynamic_cast<IdentifierReference*>(this->expression);
  if (!ref) children.push_back(this->expression);
}


const ValueTypeSpec *Size::GetValueTypeSpec() const
{
  return IntegerType::Get()->GetValueTypeSpec();
}


Expression::CodeGenerationStage Size::GenerateCode()
{
  IdentifierReference *ref = dynamic_cast<IdentifierReference*>(this->expression);
  ValueType const *type = 0;
  // If the provided expression is just an identifier, then we will lookup the type in the list of
  // varialbes of the current block and the list of types in the current module.
  if (ref != 0) {
    Variable const *var;
    if ((var = GetBlock()->GetVariable(ref->GetName())) != 0) {
      type = PointerType::Get(var->GetValueTypeSpec()->ToValueType(*GetModule()));
      // For variables the returned type is actually the pointer to that variable, so we need to get
      // the actual type of the variable, not the type of its pointer.
      auto ptype = dynamic_cast<PointerType const*>(type);
      if (ptype != 0) type = ptype->GetContentType();
    } else if ((type = GetModule()->tryGetValueTypeByName(ref->GetName())) == 0) {
      throw EXCEPTION(UndefinedVariableException,
        ("Referencing undefined identifier: " + ref->GetName()).c_str());
    }
  } else {
    type = this->expression->GetValueTypeSpec()->ToValueType(*GetModule());
  }
  this->generatedLlvmValue = this->llvmValue = IntegerType::Get()->GetLlvmConstant(type->getAllocationSize());
  return Expression::GenerateCode();
}


Expression::CodeGenerationStage Size::PostGenerateCode()
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


std::string Size::ToString()
{
  // TODO: Implement this function.
  return "";
}

} // namespace
