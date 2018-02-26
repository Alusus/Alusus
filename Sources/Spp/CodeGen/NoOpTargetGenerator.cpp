/**
 * @file Spp/CodeGen/NoOpTargetGenerator.cpp
 * Contains the implementation of class Spp::CodeGen::NoOpTargetGenerator.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace CodeGen
{

//==============================================================================
// Initialization Functions

void NoOpTargetGenerator::initBindings()
{
  auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(this);

  // Property Getters
  targetGeneration->getExecutionContext = &NoOpTargetGenerator::getExecutionContext;

  // Type Generation Functions
  targetGeneration->generateVoidType = &NoOpTargetGenerator::generateVoidType;
  targetGeneration->generateIntType = &NoOpTargetGenerator::generateIntType;
  targetGeneration->generateFloatType = &NoOpTargetGenerator::generateFloatType;
  targetGeneration->generatePointerType = &NoOpTargetGenerator::generatePointerType;
  targetGeneration->generateArrayType = &NoOpTargetGenerator::generateArrayType;
  targetGeneration->generateStructTypeDecl = &NoOpTargetGenerator::generateStructTypeDecl;
  targetGeneration->generateStructTypeBody = &NoOpTargetGenerator::generateStructTypeBody;
  targetGeneration->getTypeAllocationSize = &NoOpTargetGenerator::getTypeAllocationSize;

  // Function Generation Functions
  targetGeneration->generateFunctionDecl = &NoOpTargetGenerator::generateFunctionDecl;
  targetGeneration->prepareFunctionBody = &NoOpTargetGenerator::prepareFunctionBody;
  targetGeneration->finishFunctionBody = &NoOpTargetGenerator::finishFunctionBody;

  // Variable Definition Generation Functions
  targetGeneration->generateGlobalVariable = &NoOpTargetGenerator::generateGlobalVariable;
  targetGeneration->generateLocalVariable = &NoOpTargetGenerator::generateLocalVariable;

  // Statements Generation Functions
  targetGeneration->prepareIfStatement = &NoOpTargetGenerator::prepareIfStatement;
  targetGeneration->finishIfStatement = &NoOpTargetGenerator::finishIfStatement;
  targetGeneration->prepareWhileStatement = &NoOpTargetGenerator::prepareWhileStatement;
  targetGeneration->finishWhileStatement = &NoOpTargetGenerator::finishWhileStatement;

  // Casting Generation Functions
  targetGeneration->generateCastIntToInt = &NoOpTargetGenerator::generateCastIntToInt;
  targetGeneration->generateCastIntToFloat = &NoOpTargetGenerator::generateCastIntToFloat;
  targetGeneration->generateCastFloatToInt = &NoOpTargetGenerator::generateCastFloatToInt;
  targetGeneration->generateCastFloatToFloat = &NoOpTargetGenerator::generateCastFloatToFloat;
  targetGeneration->generateCastIntToPointer = &NoOpTargetGenerator::generateCastIntToPointer;
  targetGeneration->generateCastPointerToInt = &NoOpTargetGenerator::generateCastPointerToInt;
  targetGeneration->generateCastPointerToPointer = &NoOpTargetGenerator::generateCastPointerToPointer;

  // Operation Generation Functions
  targetGeneration->generateVarReference = &NoOpTargetGenerator::generateVarReference;
  targetGeneration->generateMemberVarReference = &NoOpTargetGenerator::generateMemberVarReference;
  targetGeneration->generateArrayElementReference = &NoOpTargetGenerator::generateArrayElementReference;
  targetGeneration->generateDereference = &NoOpTargetGenerator::generateDereference;
  targetGeneration->generateAssign = &NoOpTargetGenerator::generateAssign;
  targetGeneration->generateFunctionCall = &NoOpTargetGenerator::generateFunctionCall;
  targetGeneration->generateReturn = &NoOpTargetGenerator::generateReturn;

  // Math Ops Generation Functions
  targetGeneration->generateAddInt = &NoOpTargetGenerator::generateAddInt;
  targetGeneration->generateAddFloat = &NoOpTargetGenerator::generateAddFloat;
  targetGeneration->generateSubInt = &NoOpTargetGenerator::generateSubInt;
  targetGeneration->generateSubFloat = &NoOpTargetGenerator::generateSubFloat;
  targetGeneration->generateMulInt = &NoOpTargetGenerator::generateMulInt;
  targetGeneration->generateMulFloat = &NoOpTargetGenerator::generateMulFloat;
  targetGeneration->generateDivInt = &NoOpTargetGenerator::generateDivInt;
  targetGeneration->generateDivFloat = &NoOpTargetGenerator::generateDivFloat;
  targetGeneration->generateNegInt = &NoOpTargetGenerator::generateNegInt;
  targetGeneration->generateNegFloat = &NoOpTargetGenerator::generateNegFloat;

  // Comparison Ops Generation Functions
  targetGeneration->generateEqualInt = &NoOpTargetGenerator::generateEqualInt;
  targetGeneration->generateEqualFloat = &NoOpTargetGenerator::generateEqualFloat;
  targetGeneration->generateNotEqualInt = &NoOpTargetGenerator::generateNotEqualInt;
  targetGeneration->generateNotEqualFloat = &NoOpTargetGenerator::generateNotEqualFloat;
  targetGeneration->generateGreaterThanInt = &NoOpTargetGenerator::generateGreaterThanInt;
  targetGeneration->generateGreaterThanFloat = &NoOpTargetGenerator::generateGreaterThanFloat;
  targetGeneration->generateGreaterThanOrEqualInt = &NoOpTargetGenerator::generateGreaterThanOrEqualInt;
  targetGeneration->generateGreaterThanOrEqualFloat = &NoOpTargetGenerator::generateGreaterThanOrEqualFloat;
  targetGeneration->generateLessThanInt = &NoOpTargetGenerator::generateLessThanInt;
  targetGeneration->generateLessThanFloat = &NoOpTargetGenerator::generateLessThanFloat;
  targetGeneration->generateLessThanOrEqualInt = &NoOpTargetGenerator::generateLessThanOrEqualInt;
  targetGeneration->generateLessThanOrEqualFloat = &NoOpTargetGenerator::generateLessThanOrEqualFloat;

  // Literal Generation Functions
  targetGeneration->generateIntLiteral = &NoOpTargetGenerator::generateIntLiteral;
  targetGeneration->generateFloatLiteral = &NoOpTargetGenerator::generateFloatLiteral;
  targetGeneration->generateStringLiteral = &NoOpTargetGenerator::generateStringLiteral;
  targetGeneration->generateNullPtrLiteral = &NoOpTargetGenerator::generateNullPtrLiteral;
}

} } // namespace
