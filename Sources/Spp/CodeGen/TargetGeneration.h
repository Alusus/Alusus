/**
 * @file Spp/CodeGen/TargetGeneration.h
 * Contains the header of class Spp::CodeGen::TargetGeneration.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_TARGETGENERATION_H
#define SPP_CODEGEN_TARGETGENERATION_H

namespace Spp { namespace CodeGen
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
      &this->generateFunctionDecl,
      &this->prepareFunctionBody,
      &this->finishFunctionBody,
      &this->generateGlobalVariable,
      &this->generateLocalVariable,
      &this->prepareIfStatement,
      &this->finishIfStatement,
      &this->prepareWhileStatement,
      &this->finishWhileStatement,
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
      &this->generateFunctionCall,
      &this->generateReturn,
      &this->generateAddInt,
      &this->generateAddFloat,
      &this->generateSubInt,
      &this->generateSubFloat,
      &this->generateMulInt,
      &this->generateMulFloat,
      &this->generateDivInt,
      &this->generateDivFloat,
      &this->generateNegInt,
      &this->generateNegFloat,
      &this->generateEqualInt,
      &this->generateEqualFloat,
      &this->generateNotEqualInt,
      &this->generateNotEqualFloat,
      &this->generateGreaterThanInt,
      &this->generateGreaterThanFloat,
      &this->generateGreaterThanOrEqualInt,
      &this->generateGreaterThanOrEqualFloat,
      &this->generateLessThanInt,
      &this->generateLessThanFloat,
      &this->generateLessThanOrEqualInt,
      &this->generateLessThanOrEqualFloat,
      &this->generateIntLiteral,
      &this->generateFloatLiteral,
      &this->generateStringLiteral,
      &this->generateNullPtrLiteral
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
    Bool, (Word /* bitCount */, TioSharedPtr& /* type */)
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
      TiObject* /* type */, Core::Basic::MapContainer<Core::Basic::TiObject>* /* membersTypes */,
      Core::Basic::SharedList<TiObject, TiObject>* /* members */
    )
  );

  /// @}

  /// @name Function Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateFunctionDecl,
    Bool, (
      Char const* /* name */, Core::Basic::MapContainer<TiObject>* /* argTypes */, TiObject* /* retType */,
      Bool /* variadic */, TioSharedPtr& /* function */
    )
  );

  public: METHOD_BINDING_CACHE(prepareFunctionBody,
    Bool, (
      TiObject* /* function */, Core::Basic::MapContainer<TiObject>* /* argTypes */, TiObject* /* retType */,
      Bool /* variadic */, Core::Basic::SharedList<TiObject, TiObject>* /* args */, TioSharedPtr& /* context */
    )
  );

  public: METHOD_BINDING_CACHE(finishFunctionBody,
    Bool, (
      TiObject* /* function */, Core::Basic::MapContainer<TiObject>* /* argTypes */, TiObject* /* retType */,
      Bool /* variadic */, Core::Basic::ListContainer<TiObject>* /* args */, TiObject* /* context */
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
    Bool, (
      TiObject* /* context */,
      TioSharedPtr& /* conditionContext */,
      TioSharedPtr& /* bodyContext */,
      TioSharedPtr* /* elseContext */
    )
  );
  public: METHOD_BINDING_CACHE(finishIfStatement,
    Bool, (
      TiObject* /* context */,
      TiObject* /* conditionContext */,
      TiObject* /* conditionVal */,
      TiObject* /* bodyContext */,
      TiObject* /* elseContext */
    )
  );

  public: METHOD_BINDING_CACHE(prepareWhileStatement,
    Bool, (
      TiObject* /* context */,
      TioSharedPtr& /* conditionContext */,
      TioSharedPtr& /* bodyContext */
    )
  );
  public: METHOD_BINDING_CACHE(finishWhileStatement,
    Bool, (
      TiObject* /* context */,
      TiObject* /* conditionContext */,
      TiObject* /* conditionVal */,
      TiObject* /* bodyContext */
    )
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

  public: METHOD_BINDING_CACHE(generateFunctionCall,
    Bool, (
      TiObject* /* context */, TiObject* /* function */,
      Core::Basic::Container<Core::Basic::TiObject>* /* arguments */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateReturn,
    Bool, (
      TiObject* /* context */, TiObject* /* retType */, TiObject* /* retVal */
    )
  );

  /// @}

  /// @name Math Ops Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateAddInt,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );
  public: METHOD_BINDING_CACHE(generateAddFloat,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateSubInt,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );
  public: METHOD_BINDING_CACHE(generateSubFloat,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateMulInt,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );
  public: METHOD_BINDING_CACHE(generateMulFloat,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateDivInt,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );
  public: METHOD_BINDING_CACHE(generateDivFloat,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateNegInt,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );
  public: METHOD_BINDING_CACHE(generateNegFloat,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal */, TioSharedPtr& /* result */
    )
  );

  /// @}

  /// @name Comparison Ops Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateEqualInt,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );
  public: METHOD_BINDING_CACHE(generateEqualFloat,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateNotEqualInt,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );
  public: METHOD_BINDING_CACHE(generateNotEqualFloat,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateGreaterThanInt,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );
  public: METHOD_BINDING_CACHE(generateGreaterThanFloat,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateGreaterThanOrEqualInt,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );
  public: METHOD_BINDING_CACHE(generateGreaterThanOrEqualFloat,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateLessThanInt,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );
  public: METHOD_BINDING_CACHE(generateLessThanFloat,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(generateLessThanOrEqualInt,
    Bool, (
      TiObject* /* context */, TiObject* /* type */,
      TiObject* /* srcVal1 */, TiObject* /* srcVal2 */, TioSharedPtr& /* result */
    )
  );
  public: METHOD_BINDING_CACHE(generateLessThanOrEqualFloat,
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
      TiObject* /* context */, Word /* bitCount */, Long /* value */, TioSharedPtr& /* destVal */
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

  /// @}

}; // class

} } // namespace

#endif
