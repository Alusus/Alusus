/**
 * @file Spp/CodeGen/Generator.h
 * Contains the header of class Spp::CodeGen::Generator.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_GENERATOR_H
#define SPP_CODEGEN_GENERATOR_H

namespace Spp::CodeGen
{

class Generator : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(Generator, TiObject, "Spp.CodeGen", "Spp", "alusus.org", (
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
  private: GlobalItemRepo *globalItemRepo;
  private: TypeGenerator *typeGenerator;
  private: CommandGenerator *commandGenerator;
  private: ExpressionGenerator *expressionGenerator;

  private: Core::Notices::Store *noticeStore = 0;
  private: Bool offlineExecution = false;
  private: Int tempVarIndex = 0;


  //============================================================================
  // Constructors & Destructor

  public: Generator(
    Core::Main::RootManager *manager,
    Ast::Helper *ah,
    GlobalItemRepo *vr,
    TypeGenerator *tg,
    CommandGenerator *cg,
    ExpressionGenerator *eg
  ) :
    rootManager(manager),
    astHelper(ah),
    globalItemRepo(vr),
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
    this->globalItemRepo = parent->getGlobalItemRepo();
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

  public: GlobalItemRepo* getGlobalItemRepo() const
  {
    return this->globalItemRepo;
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

  public: void prepareBuild(Core::Notices::Store *noticeStore, Bool offlineExecution);

  /// @}

  /// @name Code Generation Functions
  /// @{

  private: static Bool _generateModules(TiObject *self, Core::Data::Ast::Scope *root, GenDeps const &deps);

  private: static Bool _generateModule(TiObject *self, Spp::Ast::Module *astModule, GenDeps const &deps);

  private: static Bool _generateFunction(TiObject *self, Spp::Ast::Function *astFunc, GenDeps const &deps);

  private: static Bool _generateFunctionDecl(TiObject *self, Spp::Ast::Function *astFunc, GenDeps const &deps);

  private: static Bool _generateUserTypeBody(TiObject *self, Spp::Ast::UserType *astType, GenDeps const &deps);

  private: static Bool _generateVarDef(
    TiObject *self, Core::Data::Ast::Definition *definition, GenDeps const &deps
  );

  private: static Bool _generateTempVar(
    TiObject *self, Core::Data::Node *astNode, Spp::Ast::Type *astType, GenDeps const &deps, Bool initialize
  );

  private: static Bool _generateVarInitialization(
    TiObject *self, Spp::Ast::Type *varAstType, TiObject *tgVarRef, Core::Data::Node *paramsAstNode,
    PlainList<TiObject> *paramAstTypes, SharedList<TiObject> *paramTgValues,
    GenDeps const &deps
  );

  private: static Bool _generateMemberVarInitialization(
    TiObject *self, TiObject *astMemberNode, GenDeps const &deps
  );

  private: static Bool _generateVarDestruction(
    TiObject *self, Spp::Ast::Type *varAstType, TiObject *tgVarRef, Core::Data::Node *astNode, GenDeps const &deps
  );

  private: static Bool _generateMemberVarDestruction(
    TiObject *self, TiObject *astMemberNode, GenDeps const &deps
  );

  private: static void _registerDestructor(
    TiObject *self, Core::Data::Node *varAstNode, Ast::Type *astType, ExecutionContext const *ec,
    DestructionStack *destructionStack
  );

  private: static Bool _generateVarGroupDestruction(
    TiObject *self, GenDeps const &deps, Int index
  );

  private: static Bool _generateStatements(
    TiObject *self, Core::Data::Ast::Scope *astBlock, GenDeps const &deps,
    TerminalStatement &terminal
  );

  private: static Bool _generateStatement(
    TiObject *self, TiObject *astNode, GenDeps const &deps, TerminalStatement &terminal
  );

  private: static Bool _generateExpression(
    TiObject *self, TiObject *astNode, GenDeps const &deps, GenResult &result
  );

  private: static Bool _generateCast(
    TiObject *self, GenDeps const &deps, Spp::Ast::Type *srcType, Spp::Ast::Type *destType,
    TiObject *tgValue, TioSharedPtr &tgCastedValue
  );

  private: static Bool _getGeneratedType(
    TiObject *self, TiObject *ref, TargetGeneration *tg, TiObject *&targetTypeResult, Ast::Type **astTypeResult
  );

  private: static Bool _getTypeAllocationSize(
    TiObject *self, Spp::Ast::Type *astType, TargetGeneration *tg, Word &result
  );

  /// @}

  /// @name Helper Functions
  /// @{

  private: Str getTempVarName();

  /// @}

}; // class

} // namespace

#endif
