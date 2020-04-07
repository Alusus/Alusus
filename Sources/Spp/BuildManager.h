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

  private: CodeGen::ExtraDataAccessor extraDataAccessor;
  private: Core::Main::RootManager *rootManager;
  private: Ast::Helper *astHelper;
  private: CodeGen::Generator *generator;
  private: LlvmCodeGen::TargetGenerator *targetGenerator;
  private: CodeGen::AstProcessor *astProcessor = 0;

  private: TioSharedPtr globalTgFuncType;
  private: TioSharedPtr globalCtorTgFunc;
  private: TioSharedPtr globalCtorTgContext;
  private: TioSharedPtr globalProcTgFunc;
  private: TioSharedPtr globalProcTgContext;


  //============================================================================
  // Constructors & Destructor

  public: BuildManager(
    Char const *extraDataPrefix,
    Core::Main::RootManager *rm,
    Ast::Helper *helper,
    CodeGen::Generator *gen,
    LlvmCodeGen::TargetGenerator *tGen
  ) :
    extraDataAccessor(extraDataPrefix),
    rootManager(rm),
    astHelper(helper),
    generator(gen),
    targetGenerator(tGen)
  {
    this->addDynamicInterface(std::make_shared<Building>(this));
    this->initBindingCaches();
    this->initBindings();
  }

  public: BuildManager(
    BuildManager *parent, Char const *extraDataPrefix, LlvmCodeGen::TargetGenerator *tGen
  ) : extraDataAccessor(extraDataPrefix)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);

    this->rootManager = parent->getRootManager();
    this->astHelper = parent->getAstHelper();
    this->generator = parent->getGenerator();
    this->targetGenerator = tGen;
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

  public: CodeGen::ExtraDataAccessor const* getExtraDataAccessor() const
  {
    return &this->extraDataAccessor;
  }

  public: Core::Main::RootManager* getRootManager() const
  {
    return this->rootManager;
  }

  public: Ast::Helper* getAstHelper() {
    return this->astHelper;
  }

  public: void setAstProcessor(CodeGen::AstProcessor *astP) {
    this->astProcessor = astP;
  }

  public: CodeGen::AstProcessor* getAstProcessor() const
  {
    return this->astProcessor;
  }

  public: CodeGen::Generator* getGenerator() const
  {
    return this->generator;
  }

  public: LlvmCodeGen::TargetGenerator* getTargetGenerator() const
  {
    return this->targetGenerator;
  }

  /// @}

  /// @name Code Generation Functions
  /// @{

  private: static void _prepareExecution(
    TiObject *self, Core::Notices::Store *noticeStore, TiObject *globalFuncElement, Char const *globalFuncName
  );

  private: static void _prepareBuild(
    TiObject *self, Core::Notices::Store *noticeStore, TiObject *globalFuncElement, Char const *globalFuncName,
    Bool offlineExecution
  );

  private: static Bool _addElementToBuild(TiObject *self, TiObject *element);

  private: static void _finalizeBuild(TiObject *self, Core::Notices::Store *noticeStore, TiObject *globalFuncElement);

  private: static Bool _execute(
    TiObject *self, Core::Notices::Store *noticeStore, Char const *funcName
  );

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

  public: METHOD_BINDING_CACHE(resetBuild, void);
  private: static void _resetBuild(TiObject *self);

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
