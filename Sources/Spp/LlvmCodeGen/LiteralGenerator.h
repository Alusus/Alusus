/**
 * @file Spp/LlvmCodeGen/LiteralGenerator.h
 * Contains the header of class Spp::LlvmCodeGen::LiteralGenerator.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_LITERALGENERATOR_H
#define SPP_LLVMCODEGEN_LITERALGENERATOR_H

namespace Spp { namespace LlvmCodeGen
{

class LiteralGenerator : public TiObject, public virtual DynamicBindings, public virtual DynamicInterfaces
{
  //============================================================================
  // Type Info

  TYPE_INFO(LiteralGenerator, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.net", (
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

  private: SharedPtr<Core::Data::Ast::ParamPass> constStringTypeRef;
  private: SharedPtr<Core::Data::Ast::ParamPass> integerTypeRef;
  private: SharedPtr<Core::Data::Ast::ParamPass> floatTypeRef;

  private: Spp::Ast::Type *astCharPtrType = 0;
  private: llvm::Type *llvmCharType = 0;
  private: Int anonymousVarIndex = 0;


  //============================================================================
  // Constructors & Destructor

  public: LiteralGenerator(Generator *g = 0) : generator(g)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: LiteralGenerator(LiteralGenerator *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->generator = parent->getGenerator();
  }

  public: virtual ~LiteralGenerator()
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

  public: METHOD_BINDING_CACHE(generateStringLiteral,
    Bool, (
      Core::Data::Ast::StringLiteral*, llvm::IRBuilder<>*, llvm::Function*,
      Spp::Ast::Type*&, llvm::Value*&, TiObject*&
    )
  );
  public: METHOD_BINDING_CACHE(generateIntegerLiteral,
    Bool, (
      Core::Data::Ast::IntegerLiteral*, llvm::IRBuilder<>*, llvm::Function*,
      Spp::Ast::Type*&, llvm::Value*&, TiObject*&
    )
  );
  public: METHOD_BINDING_CACHE(generateFloatLiteral,
    Bool, (
      Core::Data::Ast::FloatLiteral*, llvm::IRBuilder<>*, llvm::Function*,
      Spp::Ast::Type*&, llvm::Value*&, TiObject*&
    )
  );

  private: static Bool _generateStringLiteral(
    TiObject *self, Core::Data::Ast::StringLiteral *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Spp::Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
  );
  private: static Bool _generateIntegerLiteral(
    TiObject *self, Core::Data::Ast::IntegerLiteral *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Spp::Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
  );
  private: static Bool _generateFloatLiteral(
    TiObject *self, Core::Data::Ast::FloatLiteral *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Spp::Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
  );

  /// @}

  /// @name Helper Functions
  /// @{

  private: Str getAnonymouseVarName();
  private: void getConstStringType(Word size, Ast::Type *&astType, llvm::Type *&llvmType);
  private: Bool getIntegerType(Word size, Ast::Type *&astType, llvm::Type *&llvmType);
  private: Bool getFloatType(Word size, Ast::Type *&astType, llvm::Type *&llvmType);

  /// @}

}; // class

} } // namespace

#endif
