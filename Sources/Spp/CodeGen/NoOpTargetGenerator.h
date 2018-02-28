/**
 * @file Spp/CodeGen/NoOpTargetGenerator.h
 * Contains the header of class Spp::CodeGen::NoOpTargetGenerator.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_NOOPTARGETGENERATOR_H
#define SPP_CODEGEN_NOOPTARGETGENERATOR_H

namespace Spp { namespace CodeGen
{

class NoOpTargetGenerator : public TiObject, public virtual DynamicBindings, public virtual DynamicInterfaces
{
  //============================================================================
  // Type Info

  TYPE_INFO(NoOpTargetGenerator, TiObject, "Spp.CodeGen", "Spp", "alusus.net", (
    INHERITANCE_INTERFACES(DynamicBindings, DynamicInterfaces),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACES(interfaceList);


  //============================================================================
  // Member Variables

  private: SharedPtr<ExecutionContext const> executionContext;


  //============================================================================
  // Constructors & Destructor

  public: NoOpTargetGenerator()
  {
    this->addDynamicInterface(std::make_shared<Spp::CodeGen::TargetGeneration>(this));
    this->initBindings();
  }

  public: NoOpTargetGenerator(NoOpTargetGenerator *parent)
  {
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
  }

  public: virtual ~NoOpTargetGenerator()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void initBindings();

  /// @}

  /// @name Property Getters and Setters
  /// @{

  public: void setExecutionContext(SharedPtr<ExecutionContext const> const &ec)
  {
    this->executionContext = ec;
  }

  public: ExecutionContext const* getExecutionContext()
  {
    return this->executionContext.get();
  }

  /// @}

  /// @name Type Generation Functions
  /// @{

  public: Bool generateVoidType(TioSharedPtr &type) { return true; }

  public: Bool generateIntType(Word bitCount, TioSharedPtr &type) { return true; }

  public: Bool generateFloatType(Word bitCount, TioSharedPtr &type) { return true; }

  public: Bool generatePointerType(TiObject *contentType, TioSharedPtr &type) { return true; }

  public: Bool generateArrayType(TiObject *contentType, Word size, TioSharedPtr &type) { return true; }

  public: Bool generateStructTypeDecl(
    Char const *name, TioSharedPtr &type
  ) { return true; }

  public: Bool generateStructTypeBody(
    TiObject *type, Core::Basic::MapContainer<TiObject> *membersTypes,
    Core::Basic::SharedList<TiObject, TiObject> *members
  ) { return true; }

  public: Word getTypeAllocationSize(TiObject *type) { return 0; }

  /// @}

  /// @name Function Generation Functions
  /// @{

  public: Bool generateFunctionDecl(
    Char const *name, Core::Basic::MapContainer<TiObject> *argTypes, TiObject *retType, Bool variadic,
    TioSharedPtr &function
  ) { return true; }

  public: Bool prepareFunctionBody(
    TiObject *function, Core::Basic::MapContainer<TiObject> *argTypes, TiObject *retType,
    Bool variadic, Core::Basic::SharedList<TiObject, TiObject> *args, TioSharedPtr &context
  ) { return true; }

  public: Bool finishFunctionBody(
    TiObject *function, Core::Basic::MapContainer<TiObject> *argTypes, TiObject *retType,
    Bool variadic, Core::Basic::ListContainer<TiObject> *args, TiObject *context
  ) { return true; }

  /// @}

  /// @name Variable Definition Generation Functions
  /// @{

  public: Bool generateGlobalVariable(
    TiObject *type, Char const* name, TiObject *defaultValue, TioSharedPtr &result
  ) { return true; }

  public: Bool generateLocalVariable(
    TiObject *context, TiObject *type, Char const* name, TiObject *defaultValue, TioSharedPtr &result
  ) { return true; }

  /// @}

  /// @name Statements Generation Functions
  /// @{

  public: Bool prepareIfStatement(
    TiObject *context, TioSharedPtr &conditionContext, TioSharedPtr &bodyContext, TioSharedPtr *elseContext
  ) { return true; }
  public: Bool finishIfStatement(
    TiObject *context, TiObject *conditionContext, TiObject *conditionVal, TiObject *bodyContext, TiObject *elseContext
  ) { return true; }

  public: Bool prepareWhileStatement(
    TiObject *context, TioSharedPtr &conditionContext, TioSharedPtr &bodyContext
  ) { return true; }
  public: Bool finishWhileStatement(
    TiObject *context, TiObject *conditionContext, TiObject *conditionVal, TiObject *bodyContext
  ) { return true; }

  /// @}

  /// @name Casting Generation Functions
  /// @{

  public: Bool generateCastIntToInt(
    TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  ) { return true; }

  public: Bool generateCastIntToFloat(
    TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  ) { return true; }

  public: Bool generateCastFloatToInt(
    TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  ) { return true; }

  public: Bool generateCastFloatToFloat(
    TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  ) { return true; }

  public: Bool generateCastIntToPointer(
    TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  ) { return true; }

  public: Bool generateCastPointerToInt(
    TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  ) { return true; }

  public: Bool generateCastPointerToPointer(
    TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
  ) { return true; }

  /// @}

  /// @name Operation Generation Functions
  /// @{

  public: Bool generateVarReference(
    TiObject *context, TiObject *varType, TiObject *varDefinition, TioSharedPtr &result
  ) { return true; }

  public: Bool generateMemberVarReference(
    TiObject *context, TiObject *structType, TiObject *memberType,
    TiObject *memberVarDef, TiObject *structRef, TioSharedPtr &result
  ) { return true; }

  public: Bool generateArrayElementReference(
    TiObject *context, TiObject *arrayType, TiObject *elementType, TiObject *index, TiObject *arrayRef,
    TioSharedPtr &result
  ) { return true; }

  public: Bool generateDereference(
    TiObject *context, TiObject *contentType, TiObject *srcVal, TioSharedPtr &result
  ) { return true; }

  public: Bool generateAssign(
    TiObject *context, TiObject *contentType, TiObject *srcVal, TiObject *destRef, TioSharedPtr &result
  ) { return true; }

  public: Bool generateFunctionCall(
    TiObject *context, TiObject *function,
    Core::Basic::Container<Core::Basic::TiObject>* arguments, TioSharedPtr &result
  ) { return true; }

  public: Bool generateReturn(
    TiObject *context, TiObject *retType, TiObject *retVal
  ) { return true; }

  /// @}

  /// @name Math Ops Generation Functions
  /// @{

  public: Bool generateAdd(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  ) { return true; }

  public: Bool generateSub(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  ) { return true; }

  public: Bool generateMul(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  ) { return true; }

  public: Bool generateDiv(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  ) { return true; }

  public: Bool generateNeg(
    TiObject *context, TiObject *type, TiObject *srcVal, TioSharedPtr &result
  ) { return true; }

  public: Bool generateEarlyInc(
    TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
  ) { return true; }

  public: Bool generateEarlyDec(
    TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
  ) { return true; }

  public: Bool generateLateInc(
    TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
  ) { return true; }

  public: Bool generateLateDec(
    TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
  ) { return true; }

  /// @}

  /// @name Comparison Ops Generation Functions
  /// @{

  public: Bool generateEqual(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  ) { return true; }

  public: Bool generateNotEqual(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  ) { return true; }

  public: Bool generateGreaterThan(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  ) { return true; }

  public: Bool generateGreaterThanOrEqual(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  ) { return true; }

  public: Bool generateLessThan(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  ) { return true; }

  public: Bool generateLessThanOrEqual(
    TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
  ) { return true; }

  /// @}

  /// @name Literal Generation Functions
  /// @{

  public: Bool generateIntLiteral(
    TiObject *context, Word bitCount, Long value, TioSharedPtr &destVal
  ) { return true; }

  public: Bool generateFloatLiteral(
    TiObject *context, Word bitCount, Double value, TioSharedPtr &destVal
  ) { return true; }

  public: Bool generateStringLiteral(
    TiObject *context, Char const* value, TiObject *charType, TiObject *strType, TioSharedPtr &destVal
  ) { return true; }

  public: Bool generateNullPtrLiteral(
    TiObject *context, TiObject *type, TioSharedPtr &destVal
  ) { return true; }

  /// @}

}; // class

} } // namespace

#endif
