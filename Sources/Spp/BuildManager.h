/**
 * @file Spp/BuildManager.h
 * Contains the header of class Spp::BuildManager.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACING(interfaceList);


  //============================================================================
  // Member Variables

  private: Core::Main::RootManager *rootManager;
  private: Ast::Helper *astHelper;
  private: CodeGen::AstProcessor *astProcessor;
  private: CodeGen::Generator *generator;
  private: LlvmCodeGen::TargetGenerator *targetGenerator;

  private: TioSharedPtr rootExecTgFuncType;
  private: TioSharedPtr rootCtorTgFunc;
  private: TioSharedPtr rootCtorTgContext;
  private: TioSharedPtr rootStmtTgFunc;
  private: TioSharedPtr rootStmtTgContext;


  //============================================================================
  // Constructors & Destructor

  public: BuildManager(
    Core::Main::RootManager *rm,
    Ast::Helper *helper,
    CodeGen::AstProcessor *astP,
    CodeGen::Generator *gen,
    LlvmCodeGen::TargetGenerator *tGen
  ) :
    rootManager(rm),
    astHelper(helper),
    astProcessor(astP),
    generator(gen),
    targetGenerator(tGen)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: BuildManager(BuildManager *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);

    this->rootManager = parent->getRootManager();
    this->astHelper = parent->getAstHelper();
    this->astProcessor = parent->getAstProcessor();
    this->generator = parent->getGenerator();
    this->targetGenerator = parent->getTargetGenerator();
  }

  public: virtual ~BuildManager()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void initBindingCaches();
  private: void initBindings();

  public: Core::Main::RootManager* getRootManager() const
  {
    return this->rootManager;
  }

  public: Ast::Helper* getAstHelper() {
    return this->astHelper;
  }

  public: CodeGen::AstProcessor* getAstProcessor() {
    return this->astProcessor;
  }

  public: CodeGen::Generator* getGenerator() {
    return this->generator;
  }

  public: LlvmCodeGen::TargetGenerator* getTargetGenerator() {
    return this->targetGenerator;
  }

  /// @}

  /// @name Code Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(prepareRootScopeExecution, void, (Core::Notices::Store*));
  private: static void _prepareRootScopeExecution(TiObject *self, Core::Notices::Store *noticeStore);

  public: METHOD_BINDING_CACHE(addRootScopeExecutionElement, Bool, (TioSharedPtr const&));
  private: static Bool _addRootScopeExecutionElement(TiObject *self, TioSharedPtr const &element);

  public: METHOD_BINDING_CACHE(finalizeRootScopeExecution, void, (Core::Notices::Store*, Bool));
  private: static void _finalizeRootScopeExecution(TiObject *self, Core::Notices::Store *noticeStore, Bool execute);

  public: METHOD_BINDING_CACHE(dumpLlvmIrForElement,
    void, (TiObject*, Core::Notices::Store*, Core::Processing::Parser*)
  );
  public: static void _dumpLlvmIrForElement(
    TiObject *self, TiObject *element, Core::Notices::Store *noticeStore, Core::Processing::Parser *parser
  );

  public: METHOD_BINDING_CACHE(buildObjectFileForElement,
    Bool, (TiObject*, Char const*, Core::Notices::Store*, Core::Processing::Parser*)
  );
  public: static Bool _buildObjectFileForElement(
    TiObject *self, TiObject *element, Char const *objectFilename, Core::Notices::Store *noticeStore,
    Core::Processing::Parser *parser
  );

  public: METHOD_BINDING_CACHE(resetBuildData, void, (TiObject*));
  private: static void _resetBuildData(TiObject *self, TiObject *obj);

  /// @}

  /// @name Helper Functions
  /// @{

  private: TioSharedPtr getVoidNoArgsFuncTgType();

  private: void prepareFunction(
    Char const *funcName, TiObject *tgFuncType, TioSharedPtr &context, TioSharedPtr &tgFunc
  );

  /// @}

}; // class

} // namespace

#endif
