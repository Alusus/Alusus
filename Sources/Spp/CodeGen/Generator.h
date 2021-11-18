/**
 * @file Spp/CodeGen/Generator.h
 * Contains the header of class Spp::CodeGen::Generator.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
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
  private: Ast::CalleeTracer *calleeTracer;
  private: GlobalItemRepo *globalItemRepo;
  private: TypeGenerator *typeGenerator;
  private: CommandGenerator *commandGenerator;
  private: ExpressionGenerator *expressionGenerator;

  private: AstProcessor *astProcessor = 0;
  private: Int tempVarIndex = 0;


  //============================================================================
  // Constructors & Destructor

  public: Generator(
    Core::Main::RootManager *manager,
    Ast::Helper *ah,
    Ast::CalleeTracer *cTracer,
    GlobalItemRepo *vr,
    TypeGenerator *tg,
    CommandGenerator *cg,
    ExpressionGenerator *eg
  ) :
    rootManager(manager),
    astHelper(ah),
    calleeTracer(cTracer),
    globalItemRepo(vr),
    typeGenerator(tg),
    commandGenerator(cg),
    expressionGenerator(eg)
  {
    this->addDynamicInterface(newSrdObj<Generation>(this));
    this->initBindings();
  }

  public: Generator(Generator *parent)
  {
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->rootManager = parent->getRootManager();
    this->astHelper = parent->getAstHelper();
    this->calleeTracer = parent->getCalleeTracer();
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

  public: Ast::CalleeTracer* getCalleeTracer() const
  {
    return this->calleeTracer;
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

  public: void setAstProcessor(AstProcessor *ap)
  {
    this->astProcessor = ap;
  }

  public: AstProcessor* getAstProcessor() const
  {
    return this->astProcessor;
  }

  /// @}

  /// @name Code Generation Functions
  /// @{

  private: static Bool _generateModules(TiObject *self, Core::Data::Ast::Scope *root, Session *session);

  private: static Bool _generateModule(TiObject *self, Spp::Ast::Module *astModule, Session *session);

  private: static Bool _generateModuleInit(TiObject *self, Spp::Ast::Module *astModule, Session *session);

  private: static Bool _generateFunction(TiObject *self, Spp::Ast::Function *astFunc, Session *session);

  private: static Bool _generateFunctionDecl(TiObject *self, Spp::Ast::Function *astFunc, Session *session);

  private: static Bool _generateUserTypeBody(TiObject *self, Spp::Ast::UserType *astType, Session *session);

  private: static Bool _generateVarDef(
    TiObject *self, Core::Data::Ast::Definition *definition, Session *session
  );

  private: static Bool _generateTempVar(
    TiObject *self, Core::Data::Node *astNode, Spp::Ast::Type *astType, Session *session, Bool initialize
  );

  private: static Bool _generateVarInitialization(
    TiObject *self, Spp::Ast::Type *varAstType, TiObject *tgVarRef, Core::Data::Node *astNode,
    PlainList<TiObject>* paramsAstNode, PlainList<TiObject> *paramAstTypes, SharedList<TiObject> *paramTgValues,
    Session *session
  );

  private: static Bool _generateMemberVarInitialization(
    TiObject *self, TiObject *astMemberNode, Session *session
  );

  private: static Bool _generateVarDestruction(
    TiObject *self, Spp::Ast::Type *varAstType, TiObject *tgVarRef, Core::Data::Node *astNode, Session *session
  );

  private: static Bool _generateMemberVarDestruction(
    TiObject *self, TiObject *astMemberNode, Session *session
  );

  private: static void _registerDestructor(
    TiObject *self, Core::Data::Node *varAstNode, Ast::Type *astType, ExecutionContext const *ec,
    DestructionStack *destructionStack
  );

  private: static Bool _generateVarGroupDestruction(
    TiObject *self, Session *session, Int index
  );

  private: static Bool _generateStatementBlock(
    TiObject *self, TiObject *astBlock, Session *session,
    TerminalStatement &terminal
  );

  private: static Bool _generateStatement(
    TiObject *self, TiObject *astNode, Session *session, TerminalStatement &terminal
  );

  private: static Bool _generateExpression(
    TiObject *self, TiObject *astNode, Session *session, GenResult &result
  );

  private: static Bool _generateCast(
    TiObject *self, Session *session, Spp::Ast::Type *srcType, Spp::Ast::Type *destType,
    Core::Data::Node *astNode, TiObject *tgValue, Bool implicit, GenResult &castedResult
  );

  private: static Bool _generateFunctionCall(
    TiObject *self, Core::Data::Node *astNode, Spp::Ast::Function *callee,
    Containing<TiObject> *paramAstTypes, Containing<TiObject> *paramTgValues,
    Session *session, GenResult &result
  );

  private: static Bool _getGeneratedType(
    TiObject *self, TiObject *ref, Session *session, TiObject *&targetTypeResult, Ast::Type **astTypeResult
  );

  private: static Bool _getTypeAllocationSize(
    TiObject *self, Spp::Ast::Type *astType, Session *session, Word &result
  );

  private: static Int _addThisDefinition(
    TiObject *self, Ast::Block *body, Char const *thisName, Ast::Type *astThisType, TioSharedPtr const &tgThis,
    Session *session
  );

  /// @}

  /// @name Helper Functions
  /// @{

  private: Str getTempVarName();

  /// @}

}; // class

} // namespace

#endif
