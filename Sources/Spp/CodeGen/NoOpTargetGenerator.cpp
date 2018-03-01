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
  targetGeneration->generateAdd = &NoOpTargetGenerator::generateAdd;
  targetGeneration->generateSub = &NoOpTargetGenerator::generateSub;
  targetGeneration->generateMul = &NoOpTargetGenerator::generateMul;
  targetGeneration->generateDiv = &NoOpTargetGenerator::generateDiv;
  targetGeneration->generateNeg = &NoOpTargetGenerator::generateNeg;
  targetGeneration->generateEarlyInc = &NoOpTargetGenerator::generateEarlyInc;
  targetGeneration->generateEarlyDec = &NoOpTargetGenerator::generateEarlyDec;
  targetGeneration->generateLateInc = &NoOpTargetGenerator::generateLateInc;
  targetGeneration->generateLateDec = &NoOpTargetGenerator::generateLateDec;
  targetGeneration->generateAddAssign = &NoOpTargetGenerator::generateAddAssign;
  targetGeneration->generateSubAssign = &NoOpTargetGenerator::generateSubAssign;
  targetGeneration->generateMulAssign = &NoOpTargetGenerator::generateMulAssign;
  targetGeneration->generateDivAssign = &NoOpTargetGenerator::generateDivAssign;

  // Comparison Ops Generation Functions
  targetGeneration->generateEqual = &NoOpTargetGenerator::generateEqual;
  targetGeneration->generateNotEqual = &NoOpTargetGenerator::generateNotEqual;
  targetGeneration->generateGreaterThan = &NoOpTargetGenerator::generateGreaterThan;
  targetGeneration->generateGreaterThanOrEqual = &NoOpTargetGenerator::generateGreaterThanOrEqual;
  targetGeneration->generateLessThan = &NoOpTargetGenerator::generateLessThan;
  targetGeneration->generateLessThanOrEqual = &NoOpTargetGenerator::generateLessThanOrEqual;

  // Literal Generation Functions
  targetGeneration->generateIntLiteral = &NoOpTargetGenerator::generateIntLiteral;
  targetGeneration->generateFloatLiteral = &NoOpTargetGenerator::generateFloatLiteral;
  targetGeneration->generateStringLiteral = &NoOpTargetGenerator::generateStringLiteral;
  targetGeneration->generateNullPtrLiteral = &NoOpTargetGenerator::generateNullPtrLiteral;
}

} } // namespace
