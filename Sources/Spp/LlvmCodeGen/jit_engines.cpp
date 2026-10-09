/**
 * @file Spp/LlvmCodeGen/jit_engines.h
 * Contains the implementation for LLVM ORC-based JIT engines.
 */
//==============================================================================

#include "spp.h"

namespace Spp::LlvmCodeGen
{

using namespace llvm;
using namespace llvm::orc;

//==============================================================================
// JitEngineBuilderState Functions

Error JitEngineBuilderState::prepareForConstruction() {
  if (!jtmb) {
    if (auto JTMBOrErr = JITTargetMachineBuilder::detectHost())
      jtmb = std::move(*JTMBOrErr);
    else
      return JTMBOrErr.takeError();
  }

  if (!createObjectLinkingLayer && !jtmb->getCodeModel().has_value() &&
      !jtmb->getRelocationModel().has_value()) {

    auto &tt = jtmb->getTargetTriple();
    if (tt.isOSBinFormatMachO() &&
        (tt.getArch() == Triple::aarch64 || tt.getArch() == Triple::x86_64)) {

      jtmb->setRelocationModel(Reloc::PIC_);
      jtmb->setCodeModel(CodeModel::Small);
      createObjectLinkingLayer =
          [](ExecutionSession &es,
             const Triple &) -> std::unique_ptr<ObjectLayer> {
        uint64_t PageSize = 4096;
        if (auto P = llvm::sys::Process::getPageSize()) {
          PageSize = *P;
        }

        return std::make_unique<ObjectLinkingLayer>(
            es, std::make_unique<jitlink::InProcessMemoryManager>(PageSize));
      };
    }
  }

  return Error::success();
}


//==============================================================================
// JitEngine Functions

JitEngine::~JitEngine() {
  if (compileThreads)
    compileThreads->wait();
}


Error JitEngine::defineAbsolute(StringRef name, JITEvaluatedSymbol sym) {
  auto InternedName = es->intern(name);
  
  SymbolMap Symbols;
  Symbols[InternedName] = llvm::orc::ExecutorSymbolDef(
      llvm::orc::ExecutorAddr(sym.getAddress()), 
      sym.getFlags()
  );

  return main.define(absoluteSymbols(std::move(Symbols)));
}


Error JitEngine::addIRModule(JITDylib &jd, ThreadSafeModule tsm) {
  assert(tsm && "Can not add null module");

  if (auto err = tsm.withModuleDo([&](Module &m) { return applyDataLayout(m); }))
    return err;

  if (optimizeLayer.get() != nullptr) {
    return optimizeLayer->add(jd, std::move(tsm));
  } else {
    return compileLayer->add(jd, std::move(tsm));
  }
}


Error JitEngine::addObjectFile(JITDylib &jd, std::unique_ptr<MemoryBuffer> obj) {
  assert(obj && "Can not add null object");

  return objTransformLayer.add(jd, std::move(obj));
}

Expected<JITEvaluatedSymbol> JitEngine::lookupLinkerMangled(JITDylib &jd, StringRef name) {
  auto Result = es->lookup(
      makeJITDylibSearchOrder(&jd, JITDylibLookupFlags::MatchAllSymbols),
      es->intern(name));

  if (!Result)
    return Result.takeError();

  // Convert ExecutorSymbolDef to JITEvaluatedSymbol
  return JITEvaluatedSymbol(Result->getAddress().getValue(), Result->getFlags());
}


std::unique_ptr<ObjectLayer> JitEngine::createObjectLinkingLayer(JitEngineBuilderState &s, ExecutionSession &es) {
  // If the config state provided an ObjectLinkingLayer factory then use it.
  if (s.createObjectLinkingLayer)
    return s.createObjectLinkingLayer(es, s.jtmb->getTargetTriple());

  // Otherwise default to creating an RTDyldObjectLinkingLayer that constructs
  // a new SectionMemoryManager for each object.
  auto GetMemMgr = []() { return std::make_unique<SectionMemoryManager>(); };
  auto objLinkingLayer =
      std::make_unique<RTDyldObjectLinkingLayer>(es, std::move(GetMemMgr));

  if (s.jtmb->getTargetTriple().isOSBinFormatCOFF()) {
    objLinkingLayer->setOverrideObjectFlagsWithResponsibilityFlags(true);
    objLinkingLayer->setAutoClaimResponsibilityForObjectSymbols(true);
  }

  // FIXME: Explicit conversion to std::unique_ptr<ObjectLayer> added to silence
  //        errors from some GCC / libstdc++ bots. Remove this conversion (i.e.
  //        just return objLinkingLayer) once those bots are upgraded.
  return std::unique_ptr<ObjectLayer>(std::move(objLinkingLayer));
}


Expected<std::unique_ptr<IRCompileLayer::IRCompiler>> JitEngine::createCompileFunction(
  JitEngineBuilderState &s, JITTargetMachineBuilder jtmb
) {
  /// If there is a custom compile function creator set then use it.
  if (s.createCompileFunction)
    return s.createCompileFunction(std::move(jtmb));

  // Otherwise default to creating a SimpleCompiler, or ConcurrentIRCompiler,
  // depending on the number of threads requested.
  if (s.numCompileThreads > 0)
    return std::make_unique<ConcurrentIRCompiler>(std::move(jtmb));

  auto TM = jtmb.createTargetMachine();
  if (!TM)
    return TM.takeError();

  return std::make_unique<TMOwningSimpleCompiler>(std::move(*TM));
}


JitEngine::JitEngine(JitEngineBuilderState &s, Error &err)
    : es(s.es ? std::move(s.es) : []() {
        return std::make_unique<ExecutionSession>(
            cantFail(SelfExecutorProcessControl::Create()));
      }()),
      main(cantFail(this->es->createJITDylib("<main>"))), 
      dl(""),
      objLinkingLayer(createObjectLinkingLayer(s, *es)),
      objTransformLayer(*this->es, *objLinkingLayer), 
      ctorRunner(main),
      dtorRunner(main) {

  ErrorAsOutParameter _(&err);

  if (auto DLOrErr = s.jtmb->getDefaultDataLayoutForTarget()) {
    dl = std::move(*DLOrErr);
  } else {
    err = DLOrErr.takeError();
    return;
  }

  {
    auto compileFunction = createCompileFunction(s, std::move(*s.jtmb));
    if (!compileFunction) {
      err = compileFunction.takeError();
      return;
    }
    compileLayer = std::make_unique<IRCompileLayer>(
        *es, objTransformLayer, std::move(*compileFunction));
  }

  if (s.numCompileThreads > 0) {
    compileLayer->setCloneToNewContextOnEmit(true);

    compileThreads = std::make_unique<ThreadPool>(llvm::optimal_concurrency(s.numCompileThreads));

    es->setDispatchTask([this](std::unique_ptr<llvm::orc::Task> T) {
      auto SharedTask = std::shared_ptr<llvm::orc::Task>(std::move(T));
      compileThreads->async([SharedTask]() {
        SharedTask->run();
      });
    });
  }

  if (s.useOptimizeLayer) {
    optimizeLayer = createOptimizeLayer(*compileLayer);
  }
}

std::unique_ptr<llvm::orc::IRTransformLayer>
JitEngine::createOptimizeLayer(llvm::orc::IRLayer &prevLayer) {
  auto optimizeLayer = std::make_unique<IRTransformLayer>(*es, prevLayer);

  static llvm::Expected<llvm::orc::JITTargetMachineBuilder> tmb =
      llvm::orc::JITTargetMachineBuilder::detectHost();
  static std::unique_ptr<llvm::TargetMachine> targetMachine =
      std::move(tmb.get().createTargetMachine().get());

  optimizeLayer->setTransform(
    [&](llvm::orc::ThreadSafeModule tsm,
        const llvm::orc::MaterializationResponsibility &r) {
      tsm.withModuleDo([&](llvm::Module &module) {
        optimizeLlvmModule(module, targetMachine.get());
      });

      return tsm;
    }
  );

  return optimizeLayer;
}


std::string JitEngine::mangle(StringRef unmangledName) {
  std::string MangledName;
  {
    raw_string_ostream MangledNameStream(MangledName);
    Mangler::getNameWithPrefix(MangledNameStream, unmangledName, dl);
  }
  return MangledName;
}


Error JitEngine::applyDataLayout(Module &m) {
  if (m.getDataLayout().isDefault())
    m.setDataLayout(dl);

  if (m.getDataLayout() != dl)
    return make_error<StringError>(
        "Added modules have incompatible data layouts",
        inconvertibleErrorCode());

  return Error::success();
}


void JitEngine::recordCtorDtors(Module &m) {
  ctorRunner.add(getConstructors(m));
  dtorRunner.add(getDestructors(m));
}


//==============================================================================
// LazyJitEngineBuilderState Functions

Error LazyJitEngineBuilderState::prepareForConstruction() {
  if (auto err = JitEngineBuilderState::prepareForConstruction())
    return err;
  tt = jtmb->getTargetTriple();
  return Error::success();
}


//==============================================================================
// LazyJitEngine Functions

Error LazyJitEngine::addLazyIRModule(JITDylib &jd, ThreadSafeModule tsm) {
  assert(tsm && "Can not add null module");

  if (auto err = tsm.withModuleDo([&](Module &m) -> Error {
        if (auto err = applyDataLayout(m))
          return err;

        recordCtorDtors(m);
        return Error::success();
      }))
    return err;

  return optimizeLayer->add(jd, std::move(tsm));
}

LazyJitEngine::LazyJitEngine(LazyJitEngineBuilderState &s, Error &err) 
    : JitEngine(s, err) {
    
    // If base JitEngine construction failed, bail out.
    if (err)
        return;

    ErrorAsOutParameter _(&err);

    /// 1. Create/Take the lazy-compile callthrough manager.
    if (s.lctMgr) {
        lctMgr = std::move(s.lctMgr);
    } else {
        auto lctMgrOrErr = createLocalLazyCallThroughManager(
            s.tt, 
            *es, 
            llvm::orc::ExecutorAddr(s.lazyCompileFailureAddr)
        );

        if (lctMgrOrErr) {
            lctMgr = std::move(*lctMgrOrErr);
        } else {
            err = lctMgrOrErr.takeError();
            return;
        }
    }

    /// 2. Create the indirect stubs manager builder.
    auto ismBuilder = std::move(s.ismBuilder);

    if (!ismBuilder) {
        ismBuilder = createLocalIndirectStubsManagerBuilder(s.tt);
    }

    if (!ismBuilder) {
        err = make_error<StringError>(
            "Could not construct IndirectStubsManagerBuilder for target " + s.tt.str(),
            inconvertibleErrorCode()
        );
        return;
    }

    /// 3. Setup Layers.
    // transformLayer bridges the base compileLayer to the COD (Compile On Demand) layer.
    transformLayer = std::make_unique<IRTransformLayer>(*es, *compileLayer);

    codLayer = std::make_unique<CompileOnDemandLayer>(
        *es, 
        *transformLayer, 
        *lctMgr, 
        std::move(ismBuilder)
    );

    if (s.numCompileThreads > 0) {
        codLayer->setCloneToNewContextOnEmit(true);
    }

    // Finalize by creating the optimization layer on top of the COD layer.
    optimizeLayer = createOptimizeLayer(*codLayer);
}

} // namespace
