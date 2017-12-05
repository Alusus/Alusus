/**
 * @file Spp/LlvmCodeGen/ExpressionGenerator.h
 * Contains the header of class Spp::LlvmCodeGen::ExpressionGenerator.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_EXPRESSIONGENERATOR_H
#define SPP_LLVMCODEGEN_EXPRESSIONGENERATOR_H

namespace Spp { namespace LlvmCodeGen
{

class ExpressionGenerator : public TiObject, public virtual DynamicBindings, public virtual DynamicInterfaces
{
  //============================================================================
  // Type Info

  TYPE_INFO(ExpressionGenerator, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.net", (
    INHERITANCE_INTERFACES(DynamicBindings, DynamicInterfaces),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACES(interfaceList);


  //============================================================================
  // Member Variables

  private: Generator *generator;


  //============================================================================
  // Constructors & Destructor

  public: ExpressionGenerator(Generator *g = 0) : generator(g)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: ExpressionGenerator(ExpressionGenerator *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->generator = parent->getGenerator();
  }

  public: virtual ~ExpressionGenerator()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void initBindingCaches();
  private: void initBindings();

  public: void setGenerator(Generator *g)
  {
    this->generator = g;
  }

  public: Generator* getGenerator() const
  {
    return this->generator;
  }

  /// @}

  /// @name Code Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateIdentifierAccess,
    Bool, (
      Core::Data::Ast::Identifier*, llvm::IRBuilder<>*, llvm::Function*,
      Spp::Ast::Type*&, llvm::Value*&, TiObject*&
    )
  );
  public: METHOD_BINDING_CACHE(generateIdentifierReference,
    Bool, (
      Core::Data::Ast::Identifier*, llvm::IRBuilder<>*, llvm::Function*,
      Spp::Ast::Type*&, llvm::Value*&, TiObject*&
    )
  );
  public: METHOD_BINDING_CACHE(generateParamPass,
    Bool, (
      Core::Data::Ast::ParamPass*, llvm::IRBuilder<>*, llvm::Function*,
      Spp::Ast::Type*&, llvm::Value*&, TiObject*&
    )
  );
  public: METHOD_BINDING_CACHE(generateInfixOp,
    Bool, (
      Core::Data::Ast::InfixOperator*, llvm::IRBuilder<>*, llvm::Function*,
      Spp::Ast::Type*&, llvm::Value*&, TiObject*&
    )
  );
  public: METHOD_BINDING_CACHE(generateAssignment,
    Bool, (
      Core::Data::Ast::AssignmentOperator*, llvm::IRBuilder<>*, llvm::Function*,
      Spp::Ast::Type*&, llvm::Value*&, TiObject*&
    )
  );
  public: METHOD_BINDING_CACHE(generateFunctionCall,
    Bool, (
      Spp::Ast::Function*,
      Core::Basic::Container<Core::Basic::TiObject>*, Core::Basic::Container<llvm::Value>*,
      llvm::IRBuilder<>*, llvm::Function*, Spp::Ast::Type*&, llvm::Value*&
    )
  );
  public: METHOD_BINDING_CACHE(generateBuiltInFunctionCall,
    Bool, (
      Spp::Ast::Function*,
      Core::Basic::Container<Core::Basic::TiObject>*, Core::Basic::Container<llvm::Value>*,
      llvm::IRBuilder<>*, llvm::Function*, Spp::Ast::Type*&, llvm::Value*&
    )
  );
  public: METHOD_BINDING_CACHE(generateUserFunctionCall,
    Bool, (
      Spp::Ast::Function*,
      Core::Basic::Container<Core::Basic::TiObject>*, Core::Basic::Container<llvm::Value>*,
      llvm::IRBuilder<>*, llvm::Function*, Spp::Ast::Type*&, llvm::Value*&
    )
  );

  private: static Bool _generateIdentifierAccess(
    TiObject *self, Core::Data::Ast::Identifier *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Spp::Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
  );
  private: static Bool _generateIdentifierReference(
    TiObject *self, Core::Data::Ast::Identifier *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Spp::Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
  );
  private: static Bool _generateParamPass(
    TiObject *self, Core::Data::Ast::ParamPass *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Spp::Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
  );
  private: static Bool _generateInfixOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
  );
  private: static Bool _generateAssignment(
    TiObject *self, Core::Data::Ast::AssignmentOperator *astNode, llvm::IRBuilder<> *llvmIrBuilder,
    llvm::Function *llvmFunc, Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
  );
  private: static Bool _generateFunctionCall(
    TiObject *self, Spp::Ast::Function *callee,
    Core::Basic::Container<Core::Basic::TiObject> *paramTypes, Core::Basic::Container<llvm::Value> *paramCgs,
    llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc, Spp::Ast::Type *&resultType, llvm::Value *&llvmResult
  );
  private: static Bool _generateBuiltInFunctionCall(
    TiObject *self, Spp::Ast::Function *callee,
    Core::Basic::Container<Core::Basic::TiObject> *paramTypes, Core::Basic::Container<llvm::Value> *paramCgs,
    llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc, Spp::Ast::Type *&resultType, llvm::Value *&llvmResult
  );
  private: static Bool _generateUserFunctionCall(
    TiObject *self, Spp::Ast::Function *callee,
    Core::Basic::Container<Core::Basic::TiObject> *paramTypes, Core::Basic::Container<llvm::Value> *paramCgs,
    llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc, Spp::Ast::Type *&resultType, llvm::Value *&llvmResult
  );

  /// @}

  /// @name Helper Functions
  /// @{

  private: Bool generateParamList(
    Core::Basic::TiObject *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Core::Basic::ListContainer<Core::Basic::TiObject> *resultTypes, Core::Basic::ListContainer<llvm::Value> *llvmResults
  );
  private: Bool generateParamList(
    Core::Data::Container *astNodes, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Core::Basic::ListContainer<Core::Basic::TiObject> *resultTypes, Core::Basic::ListContainer<llvm::Value> *llvmResults
  );

  /// @}

}; // class

} } // namespace

#endif
