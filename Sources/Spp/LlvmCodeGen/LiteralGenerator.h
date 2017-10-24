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

  private: Core::Standard::RootManager *rootManager;
  private: TypeGenerator *typeGenerator;
  private: llvm::DataLayout *llvmDataLayout;

  private: Core::Processing::ParserState *parserState;
  private: SharedPtr<llvm::Module> llvmModule;
  private: std::vector<Core::Data::SourceLocation> *sourceLocationStack;
  private: SharedPtr<Core::Data::Ast::ParamPass> constStringTypeRef;

  private: Spp::Ast::Type *astCharPtrType = 0;
  private: llvm::Type *llvmCharType = 0;
  private: Int anonymousVarIndex = 0;
  

  //============================================================================
  // Constructors & Destructor

  public: LiteralGenerator(Core::Standard::RootManager *m, TypeGenerator *tg) : rootManager(m), typeGenerator(tg)
  {
    this->initBindingCaches();
    this->initBindings();
    this->llvmDataLayout = new llvm::DataLayout("");
  }

  public: LiteralGenerator(LiteralGenerator *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->rootManager = parent->getRootManager();
    this->typeGenerator = parent->getTypeGenerator();
    this->llvmDataLayout = new llvm::DataLayout("");
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

  public: Core::Standard::RootManager* getRootManager() const
  {
    return this->rootManager;
  }

  public: Core::Data::Seeker* getSeeker() const
  {
    return this->rootManager->getSeeker();
  }

  public: TypeGenerator* getTypeGenerator() const
  {
    return this->typeGenerator;
  }

  public: void prepareForGeneration(
    Core::Processing::ParserState *state,
    SharedPtr<llvm::Module> const &module,
    std::vector<Core::Data::SourceLocation> *stack)
  {
    this->parserState = state;
    this->llvmModule = module;
    this->sourceLocationStack = stack;
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

  private: static Bool _generateStringLiteral(
    TiObject *self, Core::Data::Ast::StringLiteral *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Spp::Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedRef
  );

  /// @}

  /// @name Helper Functions
  /// @{

  private: Str getAnonymouseVarName();
  private: void getConstStringType(Word size, Ast::Type *&astType, llvm::Type *&llvmType);

  /// @}

}; // class

} } // namespace

#endif
