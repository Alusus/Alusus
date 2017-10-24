/**
 * @file Spp/LlvmCodeGen/Generator.h
 * Contains the header of class Spp::LlvmCodeGen::Generator.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_GENERATOR_H
#define SPP_LLVMCODEGEN_GENERATOR_H

namespace Spp { namespace LlvmCodeGen
{

class Generator : public TiObject, public virtual DynamicBindings, public virtual DynamicInterfaces
{
  //============================================================================
  // Type Info

  TYPE_INFO(Generator, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.net", (
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
  private: NodePathResolver *nodePathResolver;
  private: TypeGenerator *typeGenerator;
  private: LiteralGenerator *literalGenerator;

  private: Core::Processing::ParserState *parserState;
  private: SharedPtr<llvm::Module> llvmModule;
  private: SharedPtr<ExecutionContext> executionContext;
  private: std::vector<Core::Data::SourceLocation> sourceLocationStack;

  private: Int blockIndex = 0;


  //============================================================================
  // Constructors & Destructor

  public: Generator(
    Core::Standard::RootManager *manager, NodePathResolver *r, TypeGenerator *tg, LiteralGenerator *lg
  ) : rootManager(manager), nodePathResolver(r), typeGenerator(tg), literalGenerator(lg)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: Generator(Generator *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->rootManager = parent->getRootManager();
    this->nodePathResolver = parent->getNodePathResolver();
    this->typeGenerator = parent->getTypeGenerator();
    this->literalGenerator = parent->getLiteralGenerator();
  }

  public: virtual ~Generator()
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

  public: NodePathResolver* getNodePathResolver() const
  {
    return this->nodePathResolver;
  }

  public: TypeGenerator* getTypeGenerator() const
  {
    return this->typeGenerator;
  }

  public: LiteralGenerator* getLiteralGenerator() const
  {
    return this->literalGenerator;
  }

  /// @}

  /// @name Main Operation Functions
  /// @{

  public: Bool build(Core::Data::Ast::Scope *root, Core::Processing::ParserState *state, Core::Basic::OutStream &out);
  public: Bool execute(Core::Data::Ast::Scope *root, Core::Processing::ParserState *state, TiObject *entryRef);
  public: Bool generateIr(Core::Data::Ast::Scope *root, Core::Processing::ParserState *state);

  /// @}

  /// @name Code Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateModule, Bool, (Spp::Ast::Module*));

  public: METHOD_BINDING_CACHE(generateFunction, Bool, (Spp::Ast::Function*));
  public: METHOD_BINDING_CACHE(generateFunctionDecl, Bool, (Spp::Ast::Function*));

  public: METHOD_BINDING_CACHE(generateBlock, Bool, (Spp::Ast::Block*, llvm::Function*));
  public: METHOD_BINDING_CACHE(prepareBlock, void, (Spp::Ast::Block*, llvm::Function*));

  public: METHOD_BINDING_CACHE(generateStatements,
    Bool, (Spp::Ast::Block*, llvm::IRBuilder<>*, llvm::Function*)
  );
  public: METHOD_BINDING_CACHE(generateStatement,
    Bool, (TiObject*, llvm::IRBuilder<>*, llvm::Function*, Spp::Ast::Type*&, llvm::Value*&, TiObject*&)
  );
  public: METHOD_BINDING_CACHE(generateParamPass,
    Bool, (
      Core::Data::Ast::ParamPass*, llvm::IRBuilder<>*, llvm::Function*,
      Spp::Ast::Type*&, llvm::Value*&, TiObject*&
    )
  );
  public: METHOD_BINDING_CACHE(generateIdentifier,
    Bool, (
      Core::Data::Ast::Identifier*, llvm::IRBuilder<>*, llvm::Function*,
      Spp::Ast::Type*&, llvm::Value*&, TiObject*&
    )
  );
  // public: METHOD_BINDING_CACHE(generateIfStatement, this->getBindingMap());
  // public: METHOD_BINDING_CACHE(generateWhileStatement, this->getBindingMap());
  // public: METHOD_BINDING_CACHE(generateExpression, this->getBindingMap());

  private: static Bool _generateModule(TiObject *self, Spp::Ast::Module *astModule);

  private: static Bool _generateFunction(TiObject *self, Spp::Ast::Function *astFunc);
  private: static Bool _generateFunctionDecl(TiObject *self, Spp::Ast::Function *astFunc);

  private: static Bool _generateBlock(TiObject *self, Spp::Ast::Block *astBlock, llvm::Function *llvmFunc);
  private: static void _prepareBlock(TiObject *self, Spp::Ast::Block *astBlock, llvm::Function *llvmFunc);

  private: static Bool _generateStatements(
    TiObject *self, Spp::Ast::Block *astBlock, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc
  );
  private: static Bool _generateStatement(
    TiObject *self, TiObject *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Spp::Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedRef
  );
  private: static Bool _generateParamPass(
    TiObject *self, Core::Data::Ast::ParamPass *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Spp::Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedRef
  );
  private: static Bool _generateIdentifier(
    TiObject *self, Core::Data::Ast::Identifier *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
    Spp::Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedRef
  );

  //private: static Bool _generateIfStatement(Bindings *_self, );
  //private: static Bool _generateWhileStatement(Bindings *_self, );
  //private: static Bool _generateExpression(Bindings *_self, );

  /// @}

  /// @name Helper Functions
  /// @{

  private: Str const& getFunctionName(Spp::Ast::Function *astFunc);
  private: Str getNewBlockName();

  /// @}

}; // class

} } // namespace

#endif
