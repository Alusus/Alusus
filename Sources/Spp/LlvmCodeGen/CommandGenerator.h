/**
 * @file Spp/LlvmCodeGen/CommandGenerator.h
 * Contains the header of class Spp::LlvmCodeGen::CommandGenerator.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_COMMANDGENERATOR_H
#define SPP_LLVMCODEGEN_COMMANDGENERATOR_H

namespace Spp { namespace LlvmCodeGen
{

class CommandGenerator : public TiObject, public virtual DynamicBindings, public virtual DynamicInterfaces
{
  //============================================================================
  // Type Info

  TYPE_INFO(CommandGenerator, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.net", (
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

  public: CommandGenerator(Generator *g = 0) : generator(g)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: CommandGenerator(CommandGenerator *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->generator = parent->getGenerator();
  }

  public: virtual ~CommandGenerator()
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

  public: METHOD_BINDING_CACHE(generateReturn,
    Bool, (
      Spp::Ast::ReturnStatement*, llvm::IRBuilder<>*, llvm::Function*,
      Spp::Ast::Type*&, llvm::Value*&, TiObject*&
    )
  );
  public: METHOD_BINDING_CACHE(generateIfStatement,
    Bool, (
      Spp::Ast::IfStatement*, llvm::IRBuilder<>*, llvm::Function*
    )
  );
  public: METHOD_BINDING_CACHE(generateWhileStatement,
    Bool, (
      Spp::Ast::WhileStatement*, llvm::IRBuilder<>*, llvm::Function*
    )
  );

  private: static Bool _generateReturn(
    TiObject *self, Spp::Ast::ReturnStatement *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Spp::Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
  );
  private: static Bool _generateIfStatement(
    TiObject *self, Spp::Ast::IfStatement *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc
  );
  private: static Bool _generateWhileStatement(
    TiObject *self, Spp::Ast::WhileStatement *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc
  );

  /// @}

}; // class

} } // namespace

#endif
