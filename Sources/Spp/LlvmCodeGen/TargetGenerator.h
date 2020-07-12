/**
 * @file Spp/LlvmCodeGen/TargetGenerator.h
 * Contains the header of class Spp::CodeGen::TargetGenerator.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_TARGETGENERATOR_H
#define SPP_LLVMCODEGEN_TARGETGENERATOR_H

namespace Spp::LlvmCodeGen
{

class TargetGenerator : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(TargetGenerator, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(DynamicBinding, DynamicInterfacing),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACING(interfaceList);


  //============================================================================
  // Member Variables

  private: Core::Notices::Store *noticeStore = 0;
  private: BuildTarget *buildTarget = 0;
  private: Bool perFunctionModules = false;
  private: llvm::FunctionType *vaStartEndFnType = 0;

  private: Int blockIndex = 0;
  private: Int anonymousVarIndex = 0;


  //============================================================================
  // Constructors & Destructor

  public: TargetGenerator(BuildTarget *bt, Bool perFnMods) : buildTarget(bt), perFunctionModules(perFnMods)
  {
    this->addDynamicInterface(std::make_shared<Spp::CodeGen::TargetGeneration>(this));
    this->initBindings();
  }

  public: TargetGenerator(TargetGenerator *parent, BuildTarget *bt, Bool perFnMods)
    : buildTarget(bt), perFunctionModules(perFnMods)
  {
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void initBindings();

  public: void setupBuild();

  /// @}

  /// @name Property Getters and Setters
  /// @{

  public: void setNoticeStore(Core::Notices::Store *ns)
  {
    this->noticeStore = ns;
  }

  public: Core::Notices::Store* getNoticeStore() const
  {
    return this->noticeStore;
  }

  /// @}

  /// @name Type Generation Functions
  /// @{

  public: Bool generateVoidType(TioSharedPtr &type);

  public: Bool generateIntType(Word bitCount, Bool withSign, TioSharedPtr &type);

  public: Bool generateFloatType(Word bitCount, TioSharedPtr &type);

  public: Bool generatePointerType(TiObject *contentType, TioSharedPtr &type);

  public: Bool generateArrayType(TiObject *contentType, Word size, TioSharedPtr &type);

  public: Bool generateStructTypeDecl(
    Char const *name, TioSharedPtr &type
  );

  public: Bool generateStructTypeBody(
    TiObject *type, MapContaining<TiObject> *membersTypes,
    SharedList<TiObject> *members
  );

  public: Word getTypeAllocationSize(TiObject *type);

  private: llvm::FunctionType* getVaStartEndFnType();

  /// @}

  /// @name Function Generation Functions
  /// @{

  public: Bool generateFunctionType(
    MapContaining<TiObject>* argTypes, TiObject *retType, Bool variadic, TioSharedPtr &functionType
  );

  public: Bool generateFunctionDecl(
    Char const *name, TiObject *functionType, TioSharedPtr &function
  );

  public: Bool prepareFunctionBody(
    TiObject *function, TiObject *functionType, SharedList<TiObject> *args, TioSharedPtr &context
  );

  public: Bool finishFunctionBody(
    TiObject *function, TiObject *functionType, DynamicContaining<TiObject> *args, TiObject *context
  );

  public: Bool deleteFunction(TiObject *function);

  /// @}

  /// @name Variable Definition Generation Functions
  /// @{

  public: Bool generateGlobalVariable(
    TiObject *type, Char const* name, TiObject *defaultValue, TioSharedPtr &result
  );

  public: Bool generateLocalVariable(
    TiObject *context, TiObject *type, Char const* name, TiObject *defaultValue, TioSharedPtr &result
  );

  /// @}

  /// @name Statements Generation Functions
  /// @{

  public: Bool prepareIfStatement(TiObject *context, Bool withElse, SharedPtr<CodeGen::IfTgContext> &ifTgContext);
  public: Bool finishIfStatement(TiObject *context, CodeGen::IfTgContext *ifTgContext, TiObject *conditionVal);

  public: Bool prepareWhileStatement(TiObject *context, SharedPtr<CodeGen::LoopTgContext> &loopTgContext);
  public: Bool finishWhileStatement(TiObject *context, CodeGen::LoopTgContext *loopTgContext, TiObject *conditionVal);

  public: Bool prepareForStatement(TiObject *context, SharedPtr<CodeGen::LoopTgContext> &loopTgContext);
  public: Bool finishForStatement(TiObject *context, CodeGen::LoopTgContext *loopTgContext, TiObject *conditionVal);

  public: Bool generateContinue(TiObject *context, CodeGen::LoopTgContext *loopTgContext);
  public: Bool generateBreak(TiObject *context, CodeGen::LoopTgContext *loopTgContext);

  /// @}

  /// @name Casting Generation Functions
  /// @{

  public: Bool generateCastIntToInt(
    TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  );

  public: Bool generateCastIntToFloat(
    TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  );

  public: Bool generateCastFloatToInt(
    TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  );

  public: Bool generateCastFloatToFloat(
    TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  );

  public: Bool generateCastIntToPointer(
    TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  );

  public: Bool generateCastPointerToInt(
    TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  );

  public: Bool generateCastPointerToPointer(
    TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  );

  /// @}

  /// @name Operation Generation Functions
  /// @{

  public: Bool generateVarReference(
    TiObject *context, TiObject *varType, TiObject *varDefinition, TioSharedPtr &result
  );

  public: Bool generateMemberVarReference(
    TiObject *context, TiObject *structType, TiObject *memberType,
    TiObject *memberVarDef, TiObject *structRef, TioSharedPtr &result
  );

  public: Bool generateArrayElementReference(
    TiObject *context, TiObject *arrayType, TiObject *elementType, TiObject *index, TiObject *arrayRef,
    TioSharedPtr &result
  );

  public: Bool generateDereference(
    TiObject *context, TiObject *contentType, TiObject *srcVal, TioSharedPtr &result
  );

  public: Bool generateAssign(
    TiObject *context, TiObject *contentType, TiObject *srcVal, TiObject *destRef, TioSharedPtr &result
  );

  public: Bool generateFunctionPointer(
    TiObject *context, TiObject *function, TiObject *functionPtrType, TioSharedPtr &result
  );

  public: Bool generateFunctionCall(
    TiObject *context, TiObject *function,
    Containing<TiObject>* arguments, TioSharedPtr &result
  );

  public: Bool generateFunctionPtrCall(
    TiObject *context, TiObject *functionPtr, TiObject *functionPtrType,
    Containing<TiObject>* arguments, TioSharedPtr &result
  );

  public: Bool generateReturn(
    TiObject *context, TiObject *retType, TiObject *retVal
  );

  /// @}

  /// @name Logical Ops Generation Functions
  /// @{

  public: Bool prepareLogicalOp(TiObject *context, TioSharedPtr &secondContext);

  public: Bool finishLogicalOr(
    TiObject *context, TiObject *secondContext, TiObject *val1, TiObject *val2, TioSharedPtr &result
  );

  public: Bool finishLogicalAnd(
    TiObject *context, TiObject *secondContext, TiObject *val1, TiObject *val2, TioSharedPtr &result
  );

  /// @}

  /// @name Math Ops Generation Functions
  /// @{

  public: Bool generateAdd(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateSub(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateMul(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateDiv(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateRem(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateShr(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateShl(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateAnd(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateOr(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateXor(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateNot(
    TiObject *context, TiObject *type, TiObject *srcVal, TioSharedPtr &result
  );

  public: Bool generateNeg(
    TiObject *context, TiObject *type, TiObject *srcVal, TioSharedPtr &result
  );

  public: Bool generateEarlyInc(
    TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
  );

  public: Bool generateEarlyDec(
    TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
  );

  public: Bool generateLateInc(
    TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
  );

  public: Bool generateLateDec(
    TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
  );

  public: Bool generateShrAssign(
    TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
  );

  public: Bool generateShlAssign(
    TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
  );

  public: Bool generateAndAssign(
    TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
  );

  public: Bool generateOrAssign(
    TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
  );

  public: Bool generateXorAssign(
    TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
  );

  public: Bool generateNextArg(
    TiObject *context, TiObject *type, TiObject *srcVal, TioSharedPtr &result
  );

  /// @}

  /// @name Comparison Ops Generation Functions
  /// @{

  public: Bool generateEqual(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateNotEqual(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateGreaterThan(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateGreaterThanOrEqual(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateLessThan(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  public: Bool generateLessThanOrEqual(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  /// @}

  /// @name Literal Generation Functions
  /// @{

  public: Bool generateIntLiteral(
    TiObject *context, Word bitCount, Bool withSign, LongInt value, TioSharedPtr &destVal
  );

  public: Bool generateFloatLiteral(
    TiObject *context, Word bitCount, Double value, TioSharedPtr &destVal
  );

  public: Bool generateStringLiteral(
    TiObject *context, Char const* value, TiObject *charType, TiObject *strType, TioSharedPtr &destVal
  );

  public: Bool generateNullPtrLiteral(
    TiObject *context, TiObject *type, TioSharedPtr &destVal
  );

  public: Bool generateStructLiteral(
    TiObject *context, TiObject *type, MapContaining<TiObject> *membersTypes, Containing<TiObject> *membersVals,
    TioSharedPtr &destVal
  );

  public: Bool generateArrayLiteral(
    TiObject *context, TiObject *type, Containing<TiObject> *membersVals,
    TioSharedPtr &destVal
  );

  public: Bool generatePointerLiteral(TiObject *context, TiObject *type, void *value, TioSharedPtr &destVal);

  /// @}

  /// @name Helper Functions
  /// @{

  private: Str getNewBlockName();

  private: Str getAnonymouseVarName();

  /// @}

}; // class

} // namespace

#endif
