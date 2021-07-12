/**
 * @file Spp/LlvmCodeGen/jit_engines.h
 * Contains the definitions for LLVM ORC-based JIT engines.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_JITENGINES_H
#define SPP_LLVMCODEGEN_JITENGINES_H

namespace Spp::LlvmCodeGen
{

class JitEngineBuilderState;
class LazyJitEngineBuilderState;

//==============================================================================
/// A pre-fabricated ORC JIT stack that can serve as an alternative to MCJIT.
class JitEngine
{
  template <typename, typename, typename> friend class JitEngineBuilderSetters;

  //============================================================================
  // Member Variables

  protected: std::unique_ptr<llvm::orc::ExecutionSession> es;
  protected: llvm::orc::JITDylib &main;

  protected: llvm::DataLayout dl;
  protected: std::unique_ptr<llvm::ThreadPool> compileThreads;

  protected: std::unique_ptr<llvm::orc::ObjectLayer> objLinkingLayer;
  protected: llvm::orc::ObjectTransformLayer objTransformLayer;
  protected: std::unique_ptr<llvm::orc::IRCompileLayer> compileLayer;
  protected: std::unique_ptr<llvm::orc::IRTransformLayer> optimizeLayer;

  protected: llvm::orc::CtorDtorRunner ctorRunner, dtorRunner;


  //============================================================================
  // Constructor & Destructor

  /// Create an JitEngine instance with a single compile thread.
  protected: JitEngine(JitEngineBuilderState &s, llvm::Error &err, Bool useOptimizeLayer = true);

  /// Destruct this instance. If a multi-threaded instance, waits for all
  /// compile threads to complete.
  public: virtual ~JitEngine();

  public: static llvm::Expected<std::unique_ptr<JitEngine>> Create(JitEngineBuilderState &s);


  //============================================================================
  // Member Functions

  /// Returns the ExecutionSession for this instance.
  public: llvm::orc::ExecutionSession &getExecutionSession() { return *es; }

  /// Returns a reference to the DataLayout for this instance.
  public: const llvm::DataLayout &getDataLayout() const { return dl; }

  /// Returns a reference to the JITDylib representing the JIT'd main program.
  public: llvm::orc::JITDylib &getMainJITDylib() { return main; }

  /// Returns the JITDylib with the given name, or nullptr if no JITDylib with
  /// that name exists.
  public: llvm::orc::JITDylib *getJITDylibByName(llvm::StringRef name) {
    return es->getJITDylibByName(name);
  }

  /// Create a new JITDylib with the given name and return a reference to it.
  ///
  /// JITDylib names must be unique. If the given name is derived from user
  /// input or elsewhere in the environment then the client should check
  /// (e.g. by calling getJITDylibByName) that the given name is not already in
  /// use.
  public: llvm::orc::JITDylib &createJITDylib(std::string name) {
    return es->createJITDylib(std::move(name));
  }

  /// Convenience method for defining an absolute symbol.
  public: llvm::Error defineAbsolute(llvm::StringRef name, llvm::JITEvaluatedSymbol address);

  /// Adds an IR module to the given JITDylib.
  public: llvm::Error addIRModule(llvm::orc::JITDylib &jd, llvm::orc::ThreadSafeModule tsm);

  /// Adds an IR module to the main JITDylib.
  public: llvm::Error addIRModule(llvm::orc::ThreadSafeModule tsm) {
    return addIRModule(main, std::move(tsm));
  }

  /// Adds an object file to the given JITDylib.
  public: llvm::Error addObjectFile(llvm::orc::JITDylib &jd, std::unique_ptr<llvm::MemoryBuffer> obj);

  /// Adds an object file to the given JITDylib.
  public: llvm::Error addObjectFile(std::unique_ptr<llvm::MemoryBuffer> obj) {
    return addObjectFile(main, std::move(obj));
  }

  /// Look up a symbol in JITDylib jd by the symbol's linker-mangled name (to
  /// look up symbols based on their IR name use the lookup function instead).
  public: llvm::Expected<llvm::JITEvaluatedSymbol> lookupLinkerMangled(
    llvm::orc::JITDylib &jd, llvm::StringRef name
  );

  /// Look up a symbol in the main JITDylib by the symbol's linker-mangled name
  /// (to look up symbols based on their IR name use the lookup function
  /// instead).
  public: llvm::Expected<llvm::JITEvaluatedSymbol> lookupLinkerMangled(llvm::StringRef name) {
    return lookupLinkerMangled(main, name);
  }

  /// Look up a symbol in JITDylib jd based on its IR symbol name.
  public: llvm::Expected<llvm::JITEvaluatedSymbol> lookup(llvm::orc::JITDylib &jd, llvm::StringRef unmangledName) {
    return lookupLinkerMangled(jd, mangle(unmangledName));
  }

  /// Look up a symbol in the main JITDylib based on its IR symbol name.
  public: llvm::Expected<llvm::JITEvaluatedSymbol> lookup(llvm::StringRef unmangledName) {
    return lookup(main, unmangledName);
  }

  /// Runs all not-yet-run static constructors.
  public: llvm::Error runConstructors() {
    return ctorRunner.run();
  }

  /// Runs all not-yet-run static destructors.
  public: llvm::Error runDestructors() {
    return dtorRunner.run();
  }

  /// Returns a reference to the objLinkingLayer
  public: llvm::orc::ObjectLayer &getObjLinkingLayer() {
    return *objLinkingLayer;
  }

  /// Returns a reference to the object transform layer.
  public: llvm::orc::ObjectTransformLayer &getObjTransformLayer() {
    return objTransformLayer;
  }

  protected: static std::unique_ptr<llvm::orc::ObjectLayer> createObjectLinkingLayer(
    JitEngineBuilderState &s, llvm::orc::ExecutionSession &es
  );

  protected: std::unique_ptr<llvm::orc::IRTransformLayer> createOptimizeLayer(llvm::orc::IRLayer &prevLayer);

  protected: static llvm::Expected<std::unique_ptr<llvm::orc::IRCompileLayer::IRCompiler>> createCompileFunction(
    JitEngineBuilderState &s, llvm::orc::JITTargetMachineBuilder jtmb
  );

  protected: std::string mangle(llvm::StringRef unmangledName);

  protected: llvm::Error applyDataLayout(llvm::Module &m);

  protected: void recordCtorDtors(llvm::Module &m);

};


//==============================================================================
/// An extended version of JitEngine that supports lazy function-at-a-time
/// compilation of LLVM IR.
class LazyJitEngine : public JitEngine
{
  template <typename, typename, typename> friend class JitEngineBuilderSetters;

  //============================================================================
  // Member Variables

  private: std::unique_ptr<llvm::orc::LazyCallThroughManager> lctMgr;
  private: std::unique_ptr<llvm::orc::IRTransformLayer> transformLayer;
  private: std::unique_ptr<llvm::orc::CompileOnDemandLayer> codLayer;


  //============================================================================
  // Constructors & Destructor

  // Create a single-threaded LazyJitEngine instance.
  private: LazyJitEngine(LazyJitEngineBuilderState &s, llvm::Error &err);


  //============================================================================
  // Member Functions

  /// Set an IR transform (e.g. pass manager pipeline) to run on each function
  /// when it is compiled.
  public: void setLazyCompileTransform(llvm::orc::IRTransformLayer::TransformFunction transform) {
    transformLayer->setTransform(std::move(transform));
  }

  /// Sets the partition function.
  public: void setPartitionFunction(llvm::orc::CompileOnDemandLayer::PartitionFunction partition) {
    codLayer->setPartitionFunction(std::move(partition));
  }

  /// Add a module to be lazily compiled to JITDylib jd.
  public: llvm::Error addLazyIRModule(llvm::orc::JITDylib &jd, llvm::orc::ThreadSafeModule m);

  /// Add a module to be lazily compiled to the main JITDylib.
  public: llvm::Error addLazyIRModule(llvm::orc::ThreadSafeModule m) {
    return addLazyIRModule(main, std::move(m));
  }

};


//==============================================================================
class JitEngineBuilderState
{
  public: using ObjectLinkingLayerCreator = std::function<std::unique_ptr<llvm::orc::ObjectLayer>(
      llvm::orc::ExecutionSession &, const llvm::Triple &tt)>;

  public: using CompileFunctionCreator =
      std::function<llvm::Expected<std::unique_ptr<llvm::orc::IRCompileLayer::IRCompiler>>(
          llvm::orc::JITTargetMachineBuilder jtmb)>;

  public: std::unique_ptr<llvm::orc::ExecutionSession> es;
  public: llvm::Optional<llvm::orc::JITTargetMachineBuilder> jtmb;
  public: ObjectLinkingLayerCreator createObjectLinkingLayer;
  public: CompileFunctionCreator createCompileFunction;
  public: unsigned numCompileThreads = 0;

  /// Called prior to JIT class construcion to fix up defaults.
  public: llvm::Error prepareForConstruction();
};


//==============================================================================
class GlobalMappingGenerator : public llvm::orc::JITDylib::DefinitionGenerator {
  private: CodeGen::GlobalItemRepo *itemRepo;

  public: GlobalMappingGenerator(CodeGen::GlobalItemRepo *itemRepo) : itemRepo(itemRepo) {}

  llvm::Error tryToGenerate(
    llvm::orc::LookupKind K, llvm::orc::JITDylib &JD, llvm::orc::JITDylibLookupFlags JDLookupFlags,
    const llvm::orc::SymbolLookupSet &Names
  ) {
    llvm::orc::SymbolMap NewDefs;

    for (const auto &KV : Names) {
      const auto &Name = KV.first;
      #if __APPLE__
        // Skip the leading _ that gets auto added in macOS.
        auto index = this->itemRepo->findItem((*Name).data() + 1);
      #else
        auto index = this->itemRepo->findItem((*Name).data());
      #endif
      if (index != -1) NewDefs[Name] = llvm::JITEvaluatedSymbol(
        (llvm::JITTargetAddress)this->itemRepo->getItemPtr(index), llvm::JITSymbolFlags::None
      );
    }

    cantFail(JD.define(absoluteSymbols(std::move(NewDefs))));
    return llvm::Error::success();
  }
};


//==============================================================================
template <typename JIT_TYPE, typename SETTER_IMPL, typename STATE>
class JitEngineBuilderSetters
{
  /// Set the JITTargetMachineBuilder for this instance.
  ///
  /// If this method is not called, JITTargetMachineBuilder::detectHost will be
  /// used to construct a default target machine builder for the host platform.
  public: SETTER_IMPL& setJITTargetMachineBuilder(llvm::orc::JITTargetMachineBuilder jtmb) {
    impl().jtmb = std::move(jtmb);
    return impl();
  }

  /// Return a reference to the JITTargetMachineBuilder.
  ///
  public: llvm::Optional<llvm::orc::JITTargetMachineBuilder>& getJITTargetMachineBuilder() {
    return impl().jtmb;
  }

  /// Set an ObjectLinkingLayer creation function.
  ///
  /// If this method is not called, a default creation function will be used
  /// that will construct an RTDyldObjectLinkingLayer.
  public: SETTER_IMPL& setObjectLinkingLayerCreator(
      JitEngineBuilderState::ObjectLinkingLayerCreator createObjectLinkingLayer) {
    impl().createObjectLinkingLayer = std::move(createObjectLinkingLayer);
    return impl();
  }

  /// Set a CompileFunctionCreator.
  ///
  /// If this method is not called, a default creation function wil be used
  /// that will construct a basic IR compile function that is compatible with
  /// the selected number of threads (SimpleCompiler for '0' compile threads,
  /// ConcurrentIRCompiler otherwise).
  public: SETTER_IMPL& setCompileFunctionCreator(
      JitEngineBuilderState::CompileFunctionCreator createCompileFunction) {
    impl().createCompileFunction = std::move(createCompileFunction);
    return impl();
  }

  /// Set the number of compile threads to use.
  ///
  /// If set to zero, compilation will be performed on the execution thread when
  /// JITing in-process. If set to any other number N, a thread pool of N
  /// threads will be created for compilation.
  ///
  /// If this method is not called, behavior will be as if it were called with
  /// a zero argument.
  public: SETTER_IMPL& setNumCompileThreads(unsigned numCompileThreads) {
    impl().numCompileThreads = numCompileThreads;
    return impl();
  }

  /// Create an instance of the JIT.
  public: llvm::Expected<std::unique_ptr<JIT_TYPE>> create(CodeGen::GlobalItemRepo *itemRepo) {
    if (auto err = impl().prepareForConstruction())
      return std::move(err);

    llvm::Error err = llvm::Error::success();
    std::unique_ptr<JIT_TYPE> j(new JIT_TYPE(impl(), err));
    if (err)
      return std::move(err);

    j->getMainJITDylib().addGenerator(std::make_unique<GlobalMappingGenerator>(itemRepo));
    j->getMainJITDylib().addGenerator(llvm::cantFail(
      llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(j->getDataLayout().getGlobalPrefix())
    ));

    return std::move(j);
  }

  protected: SETTER_IMPL &impl() {
    return static_cast<SETTER_IMPL &>(*this);
  }

};


//==============================================================================
/// Constructs JitEngine instances.
class JitEngineBuilder : public JitEngineBuilderState,
                         public JitEngineBuilderSetters<JitEngine, JitEngineBuilder, JitEngineBuilderState>
{
};


//==============================================================================
class LazyJitEngineBuilderState : public JitEngineBuilderState
{
  friend class LazyJitEngine;

  public: using IndirectStubsManagerBuilderFunction = std::function<std::unique_ptr<llvm::orc::IndirectStubsManager>()>;

  public: llvm::Triple tt;
  public: llvm::JITTargetAddress lazyCompileFailureAddr = 0;
  public: std::unique_ptr<llvm::orc::LazyCallThroughManager> lctMgr;
  public: IndirectStubsManagerBuilderFunction ismBuilder;

  public: llvm::Error prepareForConstruction();

};


//==============================================================================
template <typename JIT_TYPE, typename SETTER_IMPL, typename STATE>
class LazyJitEngineBuilderSetters : public JitEngineBuilderSetters<JIT_TYPE, SETTER_IMPL, STATE>
{
  /// Set the address in the target address to call if a lazy compile fails.
  ///
  /// If this method is not called then the value will default to 0.
  public: SETTER_IMPL& setLazyCompileFailureAddr(llvm::JITTargetAddress addr) {
    this->impl().lazyCompileFailureAddr = addr;
    return this->impl();
  }

  /// Set the lazy-callthrough manager.
  ///
  /// If this method is not called then a default, in-process lazy callthrough
  /// manager for the host platform will be used.
  public: SETTER_IMPL& setLazyCallthroughManager(std::unique_ptr<llvm::orc::LazyCallThroughManager> lctMgr) {
    this->impl().lctMgr = std::move(lctMgr);
    return this->impl();
  }

  /// Set the IndirectStubsManager builder function.
  ///
  /// If this method is not called then a default, in-process
  /// IndirectStubsManager builder for the host platform will be used.
  public: SETTER_IMPL& setIndirectStubsManagerBuilder(
      LazyJitEngineBuilderState::IndirectStubsManagerBuilderFunction ismBuilder) {
    this->impl().ismBuilder = std::move(ismBuilder);
    return this->impl();
  }
};


//==============================================================================
/// Constructs LazyJitEngine instances.
class LazyJitEngineBuilder :
  public LazyJitEngineBuilderState,
  public LazyJitEngineBuilderSetters<LazyJitEngine, LazyJitEngineBuilder, LazyJitEngineBuilderState>
{
};

} // namespace

#endif
