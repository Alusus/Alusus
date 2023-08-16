/**
 * @file Spp/BuildManager.h
 * Contains the header of class Spp::BuildManager.
 *
 * @copyright Copyright (C) 2023 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_BUILDMANAGER_H
#define SPP_BUILDMANAGER_H

namespace Spp
{

class BuildManager : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(BuildManager, TiObject, "Spp", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(DynamicBinding, DynamicInterfacing),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Types

  public: s_enum(BuildType, OFFLINE = 0, JIT = 1, PREPROCESS = 2);


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACING(interfaceList);


  //============================================================================
  // Member Variables

  private: Core::Main::RootManager *rootManager;
  private: Ast::Helper *astHelper;
  private: CodeGen::Generator *generator;
  private: CodeGen::GlobalItemRepo *globalItemRepo;

  private: SharedPtr<BuildSession> jitBuildSession;
  private: SharedPtr<BuildSession> preprocessBuildSession;

  private: Int funcNameIndex = 0;


  //============================================================================
  // Constructors & Destructor

  public: BuildManager(
    Core::Main::RootManager *rm,
    Ast::Helper *helper,
    CodeGen::Generator *gen,
    CodeGen::GlobalItemRepo *globalItemRepo
  ) :
    rootManager(rm),
    astHelper(helper),
    generator(gen),
    globalItemRepo(globalItemRepo)
  {
    this->addDynamicInterface(newSrdObj<Executing>(this));
    this->addDynamicInterface(newSrdObj<ExpressionComputation>(this));
    this->initBindingCaches();
    this->initBindings();

    this->initNonOfflineBuildSessions();
  }

  public: BuildManager(BuildManager *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);

    this->rootManager = parent->getRootManager();
    this->astHelper = parent->getAstHelper();
    this->generator = parent->getGenerator();

    this->initNonOfflineBuildSessions();
  }

  public: virtual ~BuildManager();


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void initBindingCaches();
  private: void initBindings();

  private: void initNonOfflineBuildSessions();
  private: SharedPtr<BuildSession> createOfflineBuildSession(Char const *targetTriple);

  public: Core::Main::RootManager* getRootManager() const
  {
    return this->rootManager;
  }

  public: Ast::Helper* getAstHelper() {
    return this->astHelper;
  }

  public: CodeGen::Generator* getGenerator() const
  {
    return this->generator;
  }

  /// @}

  /// @name Code Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(prepareBuild,
    SharedPtr<BuildSession>, (Int /* buildType */, Char const* /* targetTriple */, TiObject* /* globalFuncElement */)
  );
  private: static SharedPtr<BuildSession> _prepareBuild(
    TiObject *self, Int buildType, Char const *targetTriple, TiObject *globalFuncElement
  );

  public: METHOD_BINDING_CACHE(addElementToBuild, Bool, (TiObject* /* element */, BuildSession* /* buildSession */));
  private: static Bool _addElementToBuild(TiObject *self, TiObject *element, BuildSession *buildSession);

  public: METHOD_BINDING_CACHE(finalizeBuild,
    Bool, (TiObject* /* globalFuncElement */, BuildSession* /* buildSession */)
  );
  private: static Bool _finalizeBuild(
    TiObject *self, TiObject *globalFuncElement, BuildSession *buildSession
  );

  public: METHOD_BINDING_CACHE(execute, Bool, (BuildSession* /* buildSession */));
  private: static Bool _execute(TiObject *self, BuildSession *buildSession);

  public: METHOD_BINDING_CACHE(buildDependencies, Bool, (BuildSession* /* buildSession */));
  private: static Bool _buildDependencies(TiObject *self, BuildSession *buildSession);

  private: Bool buildGlobalCtorOrDtor(
    BuildSession *buildSession, DependencyList<Core::Data::Node> *deps, Char const *funcName,
    std::function<Bool(
      Spp::Ast::Type *varAstType, TiObject *tgVarRef, Core::Data::Node *astNode, TiObject *astParams,
      CodeGen::Session *session
    )> varOpCallback
  );

  public: METHOD_BINDING_CACHE(dumpLlvmIrForElement, void, (TiObject*));
  public: static void _dumpLlvmIrForElement(TiObject *self, TiObject *element);

  public: METHOD_BINDING_CACHE(buildObjectFileForElement, Bool, (TiObject*, Char const*, Char const*));
  public: static Bool _buildObjectFileForElement(
    TiObject *self, TiObject *element, Char const *objectFilename, Char const *targetTriple
  );

  public: METHOD_BINDING_CACHE(resetBuild, void, (BuildSession*));
  private: static void _resetBuild(TiObject *self, BuildSession *buildSession);

  public: METHOD_BINDING_CACHE(resetBuildData, void, (TiObject*, CodeGen::ExtraDataAccessor*));
  private: static void _resetBuildData(TiObject *self, TiObject *obj, CodeGen::ExtraDataAccessor *eda);

  public: METHOD_BINDING_CACHE(computeResultType,
    Bool, (TiObject* /* astNode */, TiObject*& /* result */, Bool& /* resultIsValue */)
  );
  private: static Bool _computeResultType(TiObject *self, TiObject *astNode, TiObject *&result, Bool &resultIsValue);

  /// @}

  /// @name Helper Functions
  /// @{

  private: TiObject* getVoidNoArgsFuncTgType(BuildSession *buildSession);

  private: TioSharedPtr createVoidNoArgsFuncTgType(LlvmCodeGen::TargetGenerator *targetGen);

  private: void prepareFunction(
    CodeGen::TargetGeneration *targetGen, Char const *funcName, TiObject *tgFuncType, TioSharedPtr &context,
    TioSharedPtr &tgFunc
  );

  private: void markGlobalItemsAsInitialized(Array<Str> const &initializedVarNames);

  private: static Array<Str> getGlobalCtorNames(DependencyInfo *info);

  private: static Array<Str> getGlobalDtorNames(DependencyInfo *info);

  /// @}

}; // class

} // namespace

#endif
