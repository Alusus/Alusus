/**
 * @file Spp/LlvmCodeGen/Generator.h
 * Contains the header of class Spp::LlvmCodeGen::Generator.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
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

class Generator : public TiObject, public virtual DynamicRtBinding
{
  //============================================================================
  // Type Info

  TYPE_INFO(Generator, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(TiObject, DynamicRtBinding);


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_RT_BINDING;


  //============================================================================
  // Member Variables

  public: static constexpr Char const* META_EXTRA_NAME = STR("llvmCodeGen");

  private: Core::Data::Seeker *seeker;

  private: SharedPtr<NodePathResolver> nodePathResolver;


  //============================================================================
  // Constructors & Destructor

  public: Generator(Core::Data::Seeker *s) : seeker(s)
  {
    this->initialize();
  }

  public: Generator(Generator *parent)
  {
    this->inherit(parent);
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

  private: void initialize();

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

  public: BINDING_INDEX_CACHE(generateModule, this->getBindingMap());
  public: BINDING_INDEX_CACHE(generateType, this->getBindingMap());
  public: BINDING_INDEX_CACHE(generateBuiltInType, this->getBindingMap());
  public: BINDING_INDEX_CACHE(generateUserType, this->getBindingMap());
  public: BINDING_INDEX_CACHE(generateFunction, this->getBindingMap());
  public: BINDING_INDEX_CACHE(generateBlock, this->getBindingMap());
  public: BINDING_INDEX_CACHE(generateStatements, this->getBindingMap());
  public: BINDING_INDEX_CACHE(generateStatement, this->getBindingMap());
  public: BINDING_INDEX_CACHE(generateIfStatement, this->getBindingMap());
  public: BINDING_INDEX_CACHE(generateWhileStatement, this->getBindingMap());
  public: BINDING_INDEX_CACHE(generateExpression, this->getBindingMap());

  private: static void _generateModule(RtBinding *_self, Spp::Ast::Module *astModule, llvm::Module *llvmModule);
  private: static void _generateType(RtBinding *_self, Spp::Ast::Type *astType, llvm::Module *llvmModule);
  private: static void _generateBuiltInType(RtBinding *_self, Char const *typeName, Spp::Ast::Type *astType,
                                            llvm::Module *llvmModule);
  private: static void _generateUserType(RtBinding *_self, Spp::Ast::Type *astType, llvm::Module *llvmModule);
  private: static void _generateFunction(RtBinding *_self, Spp::Ast::Function *astFunc, llvm::Module *llvmModule);
  private: static void _generateBlock(RtBinding *_self, Spp::Ast::Block *astBlock, llvm::Function *llvmFunc);
  private: static void _generateStatements(RtBinding *_self, Spp::Ast::Block *astBlock, llvm::Function *llvmFunc);
  //private: static void _generateStatement(RtBinding *_self, );
  //private: static void _generateIfStatement(RtBinding *_self, );
  //private: static void _generateWhileStatement(RtBinding *_self, );
  //private: static void _generateExpression(RtBinding *_self, );

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
