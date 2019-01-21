/**
 * @file Spp/CodeGen/Generator.h
 * Contains the header of class Spp::CodeGen::Generator.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_GENERATOR_H
#define SPP_CODEGEN_GENERATOR_H

namespace Spp { namespace CodeGen
{

class Generator : public TiObject, public virtual DynamicBinding, public virtual DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(Generator, TiObject, "Spp.CodeGen", "Spp", "alusus.net", (
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
  private: Ast::Helper *astHelper;
  private: TypeGenerator *typeGenerator;
  private: CommandGenerator *commandGenerator;
  private: ExpressionGenerator *expressionGenerator;

  private: Core::Notices::Store *noticeStore = 0;


  //============================================================================
  // Constructors & Destructor

  public: Generator(
    Core::Main::RootManager *manager,
    Ast::Helper *ah,
    TypeGenerator *tg,
    CommandGenerator *cg,
    ExpressionGenerator *eg
  ) :
    rootManager(manager),
    astHelper(ah),
    typeGenerator(tg),
    commandGenerator(cg),
    expressionGenerator(eg)
  {
    this->addDynamicInterface(std::make_shared<Generation>(this));
    this->initBindings();
  }

  public: Generator(Generator *parent)
  {
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->rootManager = parent->getRootManager();
    this->astHelper = parent->getAstHelper();
    this->typeGenerator = parent->getTypeGenerator();
    this->commandGenerator = parent->getCommandGenerator();
    this->expressionGenerator = parent->getExpressionGenerator();
  }

  public: virtual ~Generator()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void initBindings();

  /// @}

  /// @name Property Getters
  /// @{

  public: Core::Main::RootManager* getRootManager() const
  {
    return this->rootManager;
  }

  public: Core::Data::Seeker* getSeeker() const
  {
    return this->rootManager->getSeeker();
  }

  public: Ast::Helper* getAstHelper() const
  {
    return this->astHelper;
  }

  public: TypeGenerator* getTypeGenerator() const
  {
    return this->typeGenerator;
  }

  public: CommandGenerator* getCommandGenerator() const
  {
    return this->commandGenerator;
  }

  public: ExpressionGenerator* getExpressionGenerator() const
  {
    return this->expressionGenerator;
  }

  public: Core::Notices::Store* getNoticeStore() const
  {
    return this->noticeStore;
  }

  /// @}

  /// @name Main Operation Functions
  /// @{

  public: Bool generate(Core::Data::Ast::Scope *root, Core::Processing::ParserState *state, TargetGeneration *tg);

  /// @}

  /// @name Code Generation Functions
  /// @{

  private: static Bool _generateModules(TiObject *self, Core::Data::Ast::Scope *root, TargetGeneration *tg);

  private: static Bool _generateModule(TiObject *self, Spp::Ast::Module *astModule, TargetGeneration *tg);

  private: static Bool _generateFunction(TiObject *self, Spp::Ast::Function *astFunc, TargetGeneration *tg);

  private: static Bool _generateFunctionDecl(TiObject *self, Spp::Ast::Function *astFunc, TargetGeneration *tg);

  private: static Bool _generateUserTypeBody(TiObject *self, Spp::Ast::UserType *astType, TargetGeneration *tg);

  private: static Bool _generateVarDef(
    TiObject *self, Core::Data::Ast::Definition *definition, TargetGeneration *tg
  );

  private: static Bool _generateStatements(
    TiObject *self, Spp::Ast::Block *astBlock, TargetGeneration *tg, TiObject *tgContext
  );

  private: static Bool _generateStatement(
    TiObject *self, TiObject *astNode, TargetGeneration *tg, TiObject *tgContext
  );

  private: static Bool _generateExpression(
    TiObject *self, TiObject *astNode, TargetGeneration *tg, TiObject *tgContext, GenResult &result
  );

  private: static Bool _generateCast(
    TiObject *self, TargetGeneration *tg, TiObject *tgContext, Spp::Ast::Type *srcType, Spp::Ast::Type *destType,
    TiObject *tgValue, TioSharedPtr &tgCastedValue
  );

  private: static Bool _getGeneratedType(
    TiObject *self, TiObject *ref, TargetGeneration *tg, TiObject *&targetTypeResult, Ast::Type **astTypeResult
  );

  private: static Bool _getTypeAllocationSize(
    TiObject *self, Spp::Ast::Type *astType, TargetGeneration *tg, Word &result
  );

  private: static Bool _validateUseStatement(
    TiObject *self, Spp::Ast::UseStatement *useStatement
  );

  /// @}

}; // class

} } // namespace

#endif
