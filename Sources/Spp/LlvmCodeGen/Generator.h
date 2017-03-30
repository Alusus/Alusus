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

  public: static constexpr Char const* META_EXTRA_NAME = STR("llvmCodeGen");

  private: Core::Data::Seeker *seeker;

  private: SharedPtr<NodePathResolver> nodePathResolver;


  //============================================================================
  // Constructors & Destructor

  public: Generator(Core::Data::Seeker *s) : seeker(s)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: Generator(Generator *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->seeker = parent->getSeeker();
    this->nodePathResolver = parent->getNodePathResolver();
  }

  public: virtual ~Generator()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Main Operation Functions
  /// @{

  private: void initBindingCaches();
  private: void initBindings();

  public: Str generateIr(Core::Data::Ast::Scope *root);
  public: void execute(Core::Data::Ast::Scope *root, Char const *functionName);

  public: Core::Data::Seeker* getSeeker() const
  {
    return this->seeker;
  }

  public: SharedPtr<NodePathResolver> const& getNodePathResolver() const
  {
    return this->nodePathResolver;
  }

  /// @}

  /// @name Code Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateModule, void, (Spp::Ast::Module*, llvm::Module*));
  public: METHOD_BINDING_CACHE(generateType, void, (Spp::Ast::Type*, llvm::Module*));
  public: METHOD_BINDING_CACHE(generateBuiltInType, void, (Char const*, Spp::Ast::Type*, llvm::Module*));
  public: METHOD_BINDING_CACHE(generateUserType, void, (Spp::Ast::Type*, llvm::Module*));
  public: METHOD_BINDING_CACHE(generateFunction, void, (Spp::Ast::Function*, llvm::Module*));
  public: METHOD_BINDING_CACHE(generateBlock, void, (Spp::Ast::Block*, llvm::Function*));
  public: METHOD_BINDING_CACHE(generateStatements, void, (Spp::Ast::Block*, llvm::Function*));
  // public: METHOD_BINDING_CACHE(generateStatement, this->getBindingMap());
  // public: METHOD_BINDING_CACHE(generateIfStatement, this->getBindingMap());
  // public: METHOD_BINDING_CACHE(generateWhileStatement, this->getBindingMap());
  // public: METHOD_BINDING_CACHE(generateExpression, this->getBindingMap());

  private: static void _generateModule(TiObject *self, Spp::Ast::Module *astModule, llvm::Module *llvmModule);
  private: static void _generateType(TiObject *self, Spp::Ast::Type *astType, llvm::Module *llvmModule);
  private: static void _generateBuiltInType(TiObject *self, Char const *typeName, Spp::Ast::Type *astType,
                                            llvm::Module *llvmModule);
  private: static void _generateUserType(TiObject *self, Spp::Ast::Type *astType, llvm::Module *llvmModule);
  private: static void _generateFunction(TiObject *self, Spp::Ast::Function *astFunc, llvm::Module *llvmModule);
  private: static void _generateBlock(TiObject *self, Spp::Ast::Block *astBlock, llvm::Function *llvmFunc);
  private: static void _generateStatements(TiObject *self, Spp::Ast::Block *astBlock, llvm::Function *llvmFunc);
  //private: static void _generateStatement(Bindings *_self, );
  //private: static void _generateIfStatement(Bindings *_self, );
  //private: static void _generateWhileStatement(Bindings *_self, );
  //private: static void _generateExpression(Bindings *_self, );

  /// @}

  /// @name Helper Functions
  /// @{

  public: Spp::Ast::Type* getGeneratedType(TiObject *ref, llvm::Module *llvmModule);
  public: llvm::Type* getGeneratedLlvmType(TiObject *ref, llvm::Module *llvmModule);
  public: Str const& getFunctionName(Spp::Ast::Function *astFunc);
  public: Str getNewBlockName();
  public: Bool isRootTemplateInstance(Ast::Type *obj, Str &name);

  /// @}

}; // class

} } // namespace

#endif
