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

namespace Spp::CodeGen
{

//==============================================================================
// Initialization Functions

void NoOpTargetGenerator::initBindings()
{
  auto targetGeneration = ti_cast<TargetGeneration>(this);

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
  targetGeneration->prepareForStatement = &NoOpTargetGenerator::prepareForStatement;
  targetGeneration->finishForStatement = &NoOpTargetGenerator::finishForStatement;
  targetGeneration->generateContinue = &NoOpTargetGenerator::generateContinue;
  targetGeneration->generateBreak = &NoOpTargetGenerator::generateBreak;

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

  // Logical Ops Generation Functions
  targetGeneration->prepareLogicalOp = &NoOpTargetGenerator::prepareLogicalOp;
  targetGeneration->finishLogicalOr = &NoOpTargetGenerator::finishLogicalOr;
  targetGeneration->finishLogicalAnd = &NoOpTargetGenerator::finishLogicalAnd;

  // Math Ops Generation Functions
  targetGeneration->generateAdd = &NoOpTargetGenerator::generateAdd;
  targetGeneration->generateSub = &NoOpTargetGenerator::generateSub;
  targetGeneration->generateMul = &NoOpTargetGenerator::generateMul;
  targetGeneration->generateDiv = &NoOpTargetGenerator::generateDiv;
  targetGeneration->generateRem = &NoOpTargetGenerator::generateRem;
  targetGeneration->generateShr = &NoOpTargetGenerator::generateShr;
  targetGeneration->generateShl = &NoOpTargetGenerator::generateShl;
  targetGeneration->generateAnd = &NoOpTargetGenerator::generateAnd;
  targetGeneration->generateOr = &NoOpTargetGenerator::generateOr;
  targetGeneration->generateXor = &NoOpTargetGenerator::generateXor;
  targetGeneration->generateNot = &NoOpTargetGenerator::generateNot;
  targetGeneration->generateNeg = &NoOpTargetGenerator::generateNeg;
  targetGeneration->generateEarlyInc = &NoOpTargetGenerator::generateEarlyInc;
  targetGeneration->generateEarlyDec = &NoOpTargetGenerator::generateEarlyDec;
  targetGeneration->generateLateInc = &NoOpTargetGenerator::generateLateInc;
  targetGeneration->generateLateDec = &NoOpTargetGenerator::generateLateDec;
  targetGeneration->generateAddAssign = &NoOpTargetGenerator::generateAddAssign;
  targetGeneration->generateSubAssign = &NoOpTargetGenerator::generateSubAssign;
  targetGeneration->generateMulAssign = &NoOpTargetGenerator::generateMulAssign;
  targetGeneration->generateDivAssign = &NoOpTargetGenerator::generateDivAssign;
  targetGeneration->generateRemAssign = &NoOpTargetGenerator::generateRemAssign;
  targetGeneration->generateShrAssign = &NoOpTargetGenerator::generateShrAssign;
  targetGeneration->generateShlAssign = &NoOpTargetGenerator::generateShlAssign;
  targetGeneration->generateAndAssign = &NoOpTargetGenerator::generateAndAssign;
  targetGeneration->generateOrAssign = &NoOpTargetGenerator::generateOrAssign;
  targetGeneration->generateXorAssign = &NoOpTargetGenerator::generateXorAssign;

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

} // namespace
