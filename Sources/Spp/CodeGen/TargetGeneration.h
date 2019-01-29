/**
 * @file Spp/CodeGen/TargetGeneration.h
 * Contains the header of class Spp::CodeGen::TargetGeneration.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_TARGETGENERATION_H
#define SPP_CODEGEN_TARGETGENERATION_H

namespace Spp::CodeGen
{

class TargetGeneration : public ObjTiInterface
{
  //============================================================================
  // Type Info

  OBJ_INTERFACE_INFO(TargetGeneration, ObjTiInterface, "Spp", "CodeGen", "alusus.net");


  //============================================================================
  // Member Variables

  private: TiObject *owner;


  //============================================================================
  // Constructor

  public: TargetGeneration(TiObject *o) : owner(o)
  {
    initBindingCaches(this->owner, {
      &this->getExecutionContext,
      &this->generateVoidType,
      &this->generateIntType,
      &this->generateFloatType,
      &this->generatePointerType,
      &this->generateArrayType,
      &this->generateStructTypeDecl,
      &this->generateStructTypeBody,
      &this->getTypeAllocationSize,
      &this->generateFunctionType,
      &this->generateFunctionDecl,
      &this->prepareFunctionBody,
      &this->finishFunctionBody,
      &this->generateGlobalVariable,
      &this->generateLocalVariable,
      &this->prepareIfStatement,
      &this->finishIfStatement,
      &this->prepareWhileStatement,
      &this->finishWhileStatement,
      &this->prepareForStatement,
      &this->finishForStatement,
      &this->generateContinue,
      &this->generateBreak,
      &this->generateCastIntToInt,
      &this->generateCastIntToFloat,
      &this->generateCastFloatToInt,
      &this->generateCastFloatToFloat,
      &this->generateCastIntToPointer,
      &this->generateCastPointerToInt,
      &this->generateCastPointerToPointer,
      &this->generateVarReference,
      &this->generateMemberVarReference,
      &this->generateArrayElementReference,
      &this->generateDereference,
      &this->generateAssign,
      &this->generateFunctionPointer,
      &this->generateFunctionCall,
      &this->generateFunctionPtrCall,
      &this->generateReturn,
      &this->prepareLogicalOp,
      &this->finishLogicalOr,
      &this->finishLogicalAnd,
      &this->generateAdd,
      &this->generateSub,
      &this->generateMul,
      &this->generateDiv,
      &this->generateRem,
      &this->generateShr,
      &this->generateShl,
      &this->generateAnd,
      &this->generateOr,
      &this->generateXor,
      &this->generateNot,
      &this->generateNeg,
      &this->generateEarlyInc,
      &this->generateEarlyDec,
      &this->generateLateInc,
      &this->generateLateDec,
      &this->generateAddAssign,
      &this->generateSubAssign,
      &this->generateMulAssign,
      &this->generateDivAssign,
      &this->generateRemAssign,
      &this->generateShrAssign,
      &this->generateShlAssign,
      &this->generateAndAssign,
      &this->generateOrAssign,
      &this->generateXorAssign,
      &this->generateEqual,
      &this->generateNotEqual,
      &this->generateGreaterThan,
      &this->generateGreaterThanOrEqual,
      &this->generateLessThan,
      &this->generateLessThanOrEqual,
      &this->generateIntLiteral,
      &this->generateFloatLiteral,
      &this->generateStringLiteral,
      &this->generateNullPtrLiteral,
      &this->generateStructLiteral,
      &this->generateArrayLiteral
    });
  }


  //============================================================================
  // Member Functions

  /// @name ObjTiInterface Implementation
  /// @{

  public: virtual TiObject* getTiObject()
  {
    return this->owner;
  }

  public: virtual TiObject const* getTiObject() const
  {
    return this->owner;
  }

  /// @}

  /// @name Property Getters
  /// @{

  public: METHOD_BINDING_CACHE(getExecutionContext, ExecutionContext const*);

  /// @}

  /// @name Type Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateVoidType,
    Bool, (TioSharedPtr& /* type */)
  );

  public: METHOD_BINDING_CACHE(generateIntType,
    Bool, (Word /* bitCount */, Bool /* withSign */, TioSharedPtr& /* type */)
  );

  public: METHOD_BINDING_CACHE(generateFloatType,
    Bool, (Word /* bitCount */, TioSharedPtr& /* type */)
  );

  public: METHOD_BINDING_CACHE(generatePointerType,
    Bool, (TiObject* /* contentType */, TioSharedPtr& /* type */)
  );

  public: METHOD_BINDING_CACHE(generateArrayType,
    Bool, (TiObject* /* contentType */, Word /* size */, TioSharedPtr& /* type */)
  );

  public: METHOD_BINDING_CACHE(generateStructTypeDecl,
    Bool, (
      Char const* /* name */, TioSharedPtr& /* type */
    )
  );

  public: METHOD_BINDING_CACHE(generateStructTypeBody,
    Bool, (
      TiObject* /* type */, MapContaining<TiObject>* /* membersTypes */,
      SharedList<TiObject>* /* members */
    )
  );

  public: METHOD_BINDING_CACHE(getTypeAllocationSize, Word, (TiObject* /* type */));

  /// @}

  /// @name Function Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateFunctionType,
    Bool, (
      MapContaining<TiObject>* /* argTypes */, TiObject* /* retType */, Bool /* variadic */,
      TioSharedPtr& /* functionType */
    )
  );

  public: METHOD_BINDING_CACHE(generateFunctionDecl,
    Bool, (
      Char const* /* name */, TiObject* /* functionType */, TioSharedPtr& /* function */
    )
  );

  public: METHOD_BINDING_CACHE(prepareFunctionBody,
    Bool, (
      TiObject* /* function */, TiObject* /* functionType */,
      SharedList<TiObject>* /* args */, TioSharedPtr& /* context */
    )
  );

  public: METHOD_BINDING_CACHE(finishFunctionBody,
    Bool, (
      TiObject* /* function */, TiObject* /* functionType */,
      DynamicContaining<TiObject>* /* args */, TiObject* /* context */
    )
  );

  /// @}

  /// @name Variable Definition Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateGlobalVariable,
    Bool, (
      TiObject* /* type */, Char const* /* name */, TiObject* /* defaultValue */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateLocalVariable,
    Bool, (
      TiObject* /* context */, TiObject* /* type */, Char const* /* name */,
      TiObject* /* defaultValue */, TioSharedPtr& /* result */
    )
  );

  /// @}

  /// @name Statements Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(prepareIfStatement,
    Bool, (TiObject* /* context */, Bool /* withElse */, SharedPtr<IfTgContext>& /* ifTgContext */)
  );
  public: METHOD_BINDING_CACHE(finishIfStatement,
    Bool, (TiObject* /* context */, IfTgContext* /* ifTgContext */, TiObject* /* conditionVal */)
  );

  public: METHOD_BINDING_CACHE(prepareWhileStatement,
    Bool, (TiObject* /* context */, SharedPtr<LoopTgContext>& /* loopTgContext */)
  );
  public: METHOD_BINDING_CACHE(finishWhileStatement,
    Bool, (TiObject* /* context */, LoopTgContext* /* loopTgContext */, TiObject* /* conditionVal */)
  );

  public: METHOD_BINDING_CACHE(prepareForStatement,
    Bool, (TiObject* /* context */, SharedPtr<LoopTgContext>& /* loopTgContext */)
  );
  public: METHOD_BINDING_CACHE(finishForStatement,
    Bool, (TiObject* /* context */, LoopTgContext* /* loopTgContext */, TiObject* /* conditionVal */)
  );

  public: METHOD_BINDING_CACHE(generateContinue,
    Bool, (TiObject* /* context */, LoopTgContext* /* loopTgContext */)
  );
  public: METHOD_BINDING_CACHE(generateBreak,
    Bool, (TiObject* /* context */, LoopTgContext* /* loopTgContext */)
  );

  /// @}

  /// @name Casting Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateCastIntToInt,
    Bool, (
      TiObject* /* context */, TiObject* /* srcType */, TiObject* /* destType */,
      TiObject* /* srcVal */, TioSharedPtr& /* destVal */
    )
  );

  public: METHOD_BINDING_CACHE(generateCastIntToFloat,
    Bool, (
      TiObject* /* context */, TiObject* /* srcType */, TiObject* /* destType */,
      TiObject* /* srcVal */, TioSharedPtr& /* destVal */
    )
  );

  public: METHOD_BINDING_CACHE(generateCastFloatToInt,
    Bool, (
      TiObject* /* context */, TiObject* /* srcType */, TiObject* /* destType */,
      TiObject* /* srcVal */, TioSharedPtr& /* destVal */
    )
  );

  public: METHOD_BINDING_CACHE(generateCastFloatToFloat,
    Bool, (
      TiObject* /* context */, TiObject* /* srcType */, TiObject* /* destType */,
      TiObject* /* srcVal */, TioSharedPtr& /* destVal */
    )
  );

  public: METHOD_BINDING_CACHE(generateCastIntToPointer,
    Bool, (
      TiObject* /* context */, TiObject* /* srcType */, TiObject* /* destType */,
      TiObject* /* srcVal */, TioSharedPtr& /* destVal */
    )
  );

  public: METHOD_BINDING_CACHE(generateCastPointerToInt,
    Bool, (
      TiObject* /* context */, TiObject* /* srcType */, TiObject* /* destType */,
      TiObject* /* srcVal */, TioSharedPtr& /* destVal */
    )
  );

  public: METHOD_BINDING_CACHE(generateCastPointerToPointer,
    Bool, (
      TiObject* /* context */, TiObject* /* srcType */, TiObject* /* destType */,
      TiObject* /* srcVal */, TioSharedPtr& /* destVal */
    )
  );

  /// @}

  /// @name Operation Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateVarReference,
    Bool, (
      TiObject* /* context */, TiObject* /* varType */,
      TiObject* /* varDefinition */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateMemberVarReference,
    Bool, (
      TiObject* /* context */, TiObject* /* structType */, TiObject* /* memberType */,
      TiObject* /* memberVarDef */, TiObject* /* structRef */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateArrayElementReference,
    Bool, (
      TiObject* /* context */, TiObject* /* arrayType */, TiObject* /* elementType */,
      TiObject* /* index */, TiObject* /* arrayRef */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateDereference,
    Bool, (
      TiObject* /* context */, TiObject* /* contentType */,
      TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateAssign,
    Bool, (
      TiObject* /* context */, TiObject* /* contentType */,
      TiObject* /* srcVal */, TiObject* /* destRef */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateFunctionPointer,
    Bool, (
      TiObject* /* context */, TiObject* /* function */, TiObject* /* functionPtrType */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateFunctionCall,
    Bool, (
      TiObject* /* context */, TiObject* /* function */,
      Containing<TiObject>* /* arguments */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateFunctionPtrCall,
    Bool, (
      TiObject* /* context */, TiObject* /* functionPtr */, TiObject* /* functionPtrType */,
      Containing<TiObject>* /* arguments */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateReturn,
    Bool, (
      TiObject* /* context */, TiObject* /* retType */, TiObject* /* retVal */
    )
  );

  /// @}

  /// @name Logical Ops Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(prepareLogicalOp,
    Bool, (TiObject* /* context */, TioSharedPtr& /* secondContext */)
  );

  public: METHOD_BINDING_CACHE(finishLogicalOr,
    Bool, (
      TiObject* /* context */, TiObject* /* secondContext */, TiObject* /* val1 */, TiObject* /* val2 */,
      TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(finishLogicalAnd,
    Bool, (
      TiObject* /* context */, TiObject* /* secondContext */, TiObject* /* val1 */, TiObject* /* val2 */,
      TioSharedPtr& /* result */
    )
  );

  /// @}

  /// @name Math Ops Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateAdd,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateSub,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateMul,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateDiv,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateRem,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateShr,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateShl,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateAnd,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateOr,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateXor,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateNot,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateNeg,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateEarlyInc,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* destVar */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateEarlyDec,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* destVar */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateLateInc,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* destVar */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateLateDec,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* destVar */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateAddAssign,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* destVar */, TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateSubAssign,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* destVar */, TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateMulAssign,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* destVar */, TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateDivAssign,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* destVar */, TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateRemAssign,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* destVar */, TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateShrAssign,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* destVar */, TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateShlAssign,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* destVar */, TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateAndAssign,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* destVar */, TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateOrAssign,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* destVar */, TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateXorAssign,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* destVar */, TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );

  /// @}

  /// @name Comparison Ops Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateEqual,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateNotEqual,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateGreaterThan,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateGreaterThanOrEqual,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateLessThan,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateLessThanOrEqual,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  /// @}

  /// @name Literal Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateIntLiteral,
    Bool, (
      TiObject* /* context */, Word /* bitCount */, Bool /* withSign */, LongInt /* value */, TioSharedPtr& /* destVal */
    )
  );

  public: METHOD_BINDING_CACHE(generateFloatLiteral,
    Bool, (
      TiObject* /* context */, Word /* bitCount */, Double /* value */, TioSharedPtr& /* destVal */
    )
  );

  public: METHOD_BINDING_CACHE(generateStringLiteral,
    Bool, (
      TiObject* /* context */, Char const* /* value */, TiObject* /* charType */, TiObject* /* strType */,
      TioSharedPtr& /* destVal */
    )
  );

  public: METHOD_BINDING_CACHE(generateNullPtrLiteral,
    Bool, (
      TiObject* /* context */, TiObject* /* type */, TioSharedPtr& /* destVal */
    )
  );

  public: METHOD_BINDING_CACHE(generateStructLiteral,
    Bool, (
      TiObject* /* context */, TiObject* /* type */, MapContaining<TiObject>* /* membersTypes */,
      Containing<TiObject>* /* membersVals */, TioSharedPtr& /* destVal */
    )
  );

  public: METHOD_BINDING_CACHE(generateArrayLiteral,
    Bool, (
      TiObject* /* context */, TiObject* /* type */, Containing<TiObject>* /* membersVals */,
      TioSharedPtr& /* destVal */
    )
  );

  /// @}

}; // class

} // namespace

#endif
