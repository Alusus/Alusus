/**
 * @file Spp/LlvmCodeGen/TargetGenerator.h
 * Contains the header of class Spp::CodeGen::TargetGenerator.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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

  private: Core::Main::RootManager *rootManager;
  private: BuildTarget *buildTarget = 0;
  private: Bool perFunctionModules = false;
  private: TioSharedPtr nullaryProcedureType;
  private: llvm::FunctionType *vaStartEndFnType = 0;

  private: Int blockIndex = 0;
  private: Int anonymousVarIndex = 0;


  //============================================================================
  // Constructors & Destructor

  public: TargetGenerator(Core::Main::RootManager *rm, BuildTarget *bt, Bool perFnMods)
    : rootManager(rm), buildTarget(bt), perFunctionModules(perFnMods)
  {
    this->addDynamicInterface(newSrdObj<Spp::CodeGen::TargetGeneration>(this));
    this->initBindings();
  }

  public: TargetGenerator(TargetGenerator *parent, BuildTarget *bt, Bool perFnMods)
    : buildTarget(bt), perFunctionModules(perFnMods)
  {
    this->rootManager = parent->getRootManager();
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

  public: Core::Main::RootManager* getRootManager() const
  {
    return this->rootManager;
  }

  /// @}

  /// @name Type Generation Functions
  /// @{

  private: static Bool _generateVoidType(TiObject *self, TioSharedPtr &type);

  private: static Bool _generateIntType(TiObject *self, Word bitCount, Bool withSign, TioSharedPtr &type);

  private: static Bool _generateFloatType(TiObject *self, Word bitCount, TioSharedPtr &type);

  private: static Bool _generatePointerType(TiObject *self, TiObject *contentType, TioSharedPtr &type);

  private: static Bool _generateArrayType(TiObject *self, TiObject *contentType, Word size, TioSharedPtr &type);

  private: static Bool _generateStructTypeDecl(
    TiObject *self, Char const *name, TioSharedPtr &type
  );

  private: static Bool _generateStructTypeBody(
    TiObject *self, TiObject *type, MapContaining<TiObject> *membersTypes,
    SharedList<TiObject> *members
  );

  private: static Word _getTypeAllocationSize(TiObject *self, TiObject *type);

  private: static TiObject* _getNullaryProcedureType(TiObject *self);

  private: llvm::FunctionType* getVaStartEndFnType();

  /// @}

  /// @name Function Generation Functions
  /// @{

  private: static Bool _generateFunctionType(
    TiObject *self, MapContaining<TiObject>* argTypes, TiObject *retType, Bool variadic, TioSharedPtr &functionType
  );

  private: static Bool _generateFunctionDecl(
    TiObject *self, Char const *name, TiObject *functionType, TioSharedPtr &function
  );

  private: static Bool _prepareFunctionBody(
    TiObject *self, TiObject *function, TiObject *functionType, SharedList<TiObject> *args, TioSharedPtr &context
  );

  private: static Bool _finishFunctionBody(
    TiObject *self, TiObject *function, TiObject *functionType, DynamicContaining<TiObject> *args, TiObject *context
  );

  private: static Bool _deleteFunction(TiObject *self, TiObject *function);

  /// @}

  /// @name Variable Definition Generation Functions
  /// @{

  private: static Bool _generateGlobalVariable(
    TiObject *self, TiObject *type, Char const* name, TiObject *defaultValue, TioSharedPtr &result
  );

  private: static Bool _generateLocalVariable(
    TiObject *self, TiObject *context, TiObject *type, Char const* name, TiObject *defaultValue, TioSharedPtr &result
  );

  /// @}

  /// @name Statements Generation Functions
  /// @{

  private: static Bool _prepareIfStatement(
    TiObject *self, TiObject *context, Bool withElse, SharedPtr<CodeGen::IfTgContext> &ifTgContext
  );
  private: static Bool _finishIfStatement(
    TiObject *self, TiObject *context, CodeGen::IfTgContext *ifTgContext, TiObject *conditionVal
  );

  private: static Bool _prepareWhileStatement(
    TiObject *self, TiObject *context, SharedPtr<CodeGen::LoopTgContext> &loopTgContext
  );
  private: static Bool _finishWhileStatement(
    TiObject *self, TiObject *context, CodeGen::LoopTgContext *loopTgContext, TiObject *conditionVal
  );

  private: static Bool _prepareForStatement(
    TiObject *self, TiObject *context, SharedPtr<CodeGen::LoopTgContext> &loopTgContext
  );
  private: static Bool _finishForStatement(
    TiObject *self, TiObject *context, CodeGen::LoopTgContext *loopTgContext, TiObject *conditionVal
  );

  private: static Bool _generateContinue(TiObject *self, TiObject *context, CodeGen::LoopTgContext *loopTgContext);
  private: static Bool _generateBreak(TiObject *self, TiObject *context, CodeGen::LoopTgContext *loopTgContext);

  /// @}

  /// @name Casting Generation Functions
  /// @{

  private: static Bool _generateCastIntToInt(
    TiObject *self, TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  );

  private: static Bool _generateCastIntToFloat(
    TiObject *self, TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  );

  private: static Bool _generateCastFloatToInt(
    TiObject *self, TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  );

  private: static Bool _generateCastFloatToFloat(
    TiObject *self, TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  );

  private: static Bool _generateCastIntToPointer(
    TiObject *self, TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  );

  private: static Bool _generateCastPointerToInt(
    TiObject *self, TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  );

  private: static Bool _generateCastPointerToPointer(
    TiObject *self, TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  );

  /// @}

  /// @name Operation Generation Functions
  /// @{

  private: static Bool _generateVarReference(
    TiObject *self, TiObject *context, TiObject *varType, TiObject *varDefinition, TioSharedPtr &result
  );

  private: static Bool _generateMemberVarReference(
    TiObject *self, TiObject *context, TiObject *structType, TiObject *memberType,
    TiObject *memberVarDef, TiObject *structRef, TioSharedPtr &result
  );

  private: static Bool _generateArrayElementReference(
    TiObject *self, TiObject *context, TiObject *arrayType, TiObject *elementType, TiObject *index, TiObject *arrayRef,
    TioSharedPtr &result
  );

  private: static Bool _generateDereference(
    TiObject *self, TiObject *context, TiObject *contentType, TiObject *srcVal, TioSharedPtr &result
  );

  private: static Bool _generateAssign(
    TiObject *self, TiObject *context, TiObject *contentType, TiObject *srcVal, TiObject *destRef, TioSharedPtr &result
  );

  private: static Bool _generateFunctionPointer(
    TiObject *self, TiObject *context, TiObject *function, TiObject *functionPtrType, TioSharedPtr &result
  );

  private: static Bool _generateFunctionCall(
    TiObject *self, TiObject *context, TiObject *function,
    Containing<TiObject>* arguments, TioSharedPtr &result
  );

  private: static Bool _generateFunctionPtrCall(
    TiObject *self, TiObject *context, TiObject *functionPtr, TiObject *functionPtrType,
    Containing<TiObject>* arguments, TioSharedPtr &result
  );

  private: static Bool _generateReturn(
    TiObject *self, TiObject *context, TiObject *retType, TiObject *retVal
  );

  /// @}

  /// @name Logical Ops Generation Functions
  /// @{

  private: static Bool _prepareLogicalOp(TiObject *self, TiObject *context, TioSharedPtr &secondContext);

  private: static Bool _finishLogicalOr(
    TiObject *self, TiObject *context, TiObject *secondContext, TiObject *val1, TiObject *val2, TioSharedPtr &result
  );

  private: static Bool _finishLogicalAnd(
    TiObject *self, TiObject *context, TiObject *secondContext, TiObject *val1, TiObject *val2, TioSharedPtr &result
  );

  /// @}

  /// @name Math Ops Generation Functions
  /// @{

  private: static Bool _generateAdd(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateSub(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateMul(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateDiv(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateRem(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateShr(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateShl(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateAnd(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateOr(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateXor(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateNot(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal, TioSharedPtr &result
  );

  private: static Bool _generateNeg(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal, TioSharedPtr &result
  );

  private: static Bool _generateEarlyInc(
    TiObject *self, TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
  );

  private: static Bool _generateEarlyDec(
    TiObject *self, TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
  );

  private: static Bool _generateLateInc(
    TiObject *self, TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
  );

  private: static Bool _generateLateDec(
    TiObject *self, TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
  );

  private: static Bool _generateShrAssign(
    TiObject *self, TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
  );

  private: static Bool _generateShlAssign(
    TiObject *self, TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
  );

  private: static Bool _generateAndAssign(
    TiObject *self, TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
  );

  private: static Bool _generateOrAssign(
    TiObject *self, TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
  );

  private: static Bool _generateXorAssign(
    TiObject *self, TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
  );

  private: static Bool _generateNextArg(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal, TioSharedPtr &result
  );

  /// @}

  /// @name Comparison Ops Generation Functions
  /// @{

  private: static Bool _generateEqual(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateNotEqual(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateGreaterThan(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateGreaterThanOrEqual(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateLessThan(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  private: static Bool _generateLessThanOrEqual(
    TiObject *self, TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  );

  /// @}

  /// @name Literal Generation Functions
  /// @{

  private: static Bool _generateIntLiteral(
    TiObject *self, TiObject *context, Word bitCount, Bool withSign, LongInt value, TioSharedPtr &destVal
  );

  private: static Bool _generateFloatLiteral(
    TiObject *self, TiObject *context, Word bitCount, Double value, TioSharedPtr &destVal
  );

  private: static Bool _generateStringLiteral(
    TiObject *self, TiObject *context, Char const* value, TiObject *charType, TiObject *strType, TioSharedPtr &destVal
  );

  private: static Bool _generateNullPtrLiteral(
    TiObject *self, TiObject *context, TiObject *type, TioSharedPtr &destVal
  );

  private: static Bool _generateStructLiteral(
    TiObject *self, TiObject *context, TiObject *type, MapContaining<TiObject> *membersTypes,
    Containing<TiObject> *membersVals,
    TioSharedPtr &destVal
  );

  private: static Bool _generateArrayLiteral(
    TiObject *self, TiObject *context, TiObject *type, Containing<TiObject> *membersVals,
    TioSharedPtr &destVal
  );

  private: static Bool _generatePointerLiteral(
    TiObject *self, TiObject *context, TiObject *type, void *value, TioSharedPtr &destVal
  );

  /// @}

  /// @name Helper Functions
  /// @{

  private: std::string getNewBlockName();

  private: std::string getAnonymouseVarName();

  /// @}

  /// @name C ABI Helper Functions
  /// @{

  /// Decide how a struct is passed (as an argument if isArg, otherwise as a return value). On x86-64 System V this
  /// follows the eightbyte classification rules. intRegs/sseRegs hold the number of registers still available.
  private: AbiInfo classifyStruct(llvm::Type *llvmType, Bool isArg, Int &intRegs, Int &sseRegs);

  /// Apply the sret and byval attributes dictated by the ABI info to a function declaration or a call site.
  private: template <class T> void applyAbiAttributes(T *target, FunctionType *funcType);

  /// Convert a struct value to the LLVM arguments needed to pass it according to its ABI info.
  /// If the value is a load instruction with no other users, the load is erased and its pointer reused.
  private: void lowerStructArg(
    llvm::IRBuilder<> *builder, llvm::Value *structValue, AbiInfo const &abi, std::vector<llvm::Value*> &args
  );

  /// Create a struct value from the LLVM parameters it was passed in (the reverse of lowerStructArg).
  private: llvm::Value* raiseStructArg(
    llvm::IRBuilder<> *builder, llvm::Type *structType, AbiInfo const &abi, llvm::Value **params
  );

  /// Convert a struct value to the value returned by a COERCED return type.
  private: llvm::Value* lowerStructRet(llvm::IRBuilder<> *builder, llvm::Value *structValue, AbiInfo const &abi);

  /// Create a struct value from the value returned by a COERCED return type.
  private: llvm::Value* raiseStructRet(
    llvm::IRBuilder<> *builder, llvm::Type *structType, AbiInfo const &abi, llvm::Value *retValue
  );

  /// Create the call instruction for a function, applying the ABI to arguments and result.
  private: llvm::Value* generateAbiCall(
    llvm::IRBuilder<> *builder, FunctionType *funcType, llvm::Value *callee,
    Containing<TiObject>* arguments
  );

  /// @}

}; // class

} // namespace

#endif
