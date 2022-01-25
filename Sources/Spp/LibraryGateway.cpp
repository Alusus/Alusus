/**
 * @file Spp/LibraryGateway.cpp
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp
{

using namespace Core;
using namespace Core::Data;
using namespace Core::Processing;
using namespace Core::Processing::Handlers;

//==============================================================================
// Overloaded Abstract Functions

void LibraryGateway::initialize(Main::RootManager *manager)
{
  // Initialize type info for types that aren't initialized at the C++ side prior to being used on Alusus side.
  TiPtr::getTypeInfo();
  Ast::PreGenTransformStatement::getTypeInfo();

  // Create AST helpers.
  this->nodePathResolver = newSrdObj<Ast::NodePathResolver>();
  this->astHelper = newSrdObj<Ast::Helper>(manager, this->nodePathResolver.get());
  this->calleeTracer = newSrdObj<Ast::CalleeTracer>(this->astHelper.get());

  // Create global repos.
  this->astNodeRepo = newSrdObj<SharedList<TiObject>>();
  this->globalItemRepo = newSrdObj<CodeGen::GlobalItemRepo>();

  // Create the generator.
  this->typeGenerator = newSrdObj<CodeGen::TypeGenerator>(this->astHelper.get());
  this->expressionGenerator = newSrdObj<CodeGen::ExpressionGenerator>(
    this->astHelper.get(),
    this->calleeTracer.get(),
    this->astNodeRepo.get()
  );
  this->commandGenerator = newSrdObj<CodeGen::CommandGenerator>(this->astHelper.get());
  this->generator = newSrdObj<CodeGen::Generator>(
    manager,
    this->astHelper.get(),
    this->calleeTracer.get(),
    this->globalItemRepo.get(),
    this->typeGenerator.get(),
    this->commandGenerator.get(),
    this->expressionGenerator.get()
  );

  // Initialize LLVM.
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();

  this->buildManager = newSrdObj<BuildManager>(
    manager,
    this->astHelper.get(),
    this->generator.get(),
    this->globalItemRepo.get()
  );

  this->astProcessor = newSrdObj<CodeGen::AstProcessor>(
    this->astHelper.get(), this->buildManager.ti_cast_get<Executing>(), this->astNodeRepo.get()
  );
  this->generator->setAstProcessor(this->astProcessor.get());
  this->typeGenerator->setAstProcessor(this->astProcessor.get());

  // Add the grammar.
  this->grammarFactory = newSrdObj<Grammar::SppFactory>();
  this->grammarFactory->setRootManager(manager);
  this->grammarFactory->createGrammar();

  // Prepare run-time objects.
  this->rtGrammarMgr = newSrdObj<Rt::GrammarMgr>(manager, this->grammarFactory.get());
  this->rtAstMgr = newSrdObj<Rt::AstMgr>();
  this->rtAstMgr->setAstHelper(this->astHelper.get());
  this->rtAstMgr->setRootManager(manager);
  this->rtAstMgr->setAstProcessor(this->astProcessor.get());
  this->rtBuildMgr = newSrdObj<Rt::BuildMgr>(manager, this->buildManager.get());

  // Extend Core singletons.
  this->seekerExtensionOverrides = SeekerExtension::extend(manager->getSeeker(), this->astHelper);
  this->rootScopeHandlerExtensionOverrides = RootScopeHandlerExtension::extend(manager->getRootScopeHandler(), manager);
  this->rootManagerExtensionOverrides = RootManagerExtension::extend(
    manager, this->buildManager, this->astProcessor, this->rtGrammarMgr, this->rtAstMgr, this->rtBuildMgr
  );

  this->createBuiltInTypes(manager);
  this->createGlobalDefs(manager);
  this->initializeGlobalItemRepo(manager);
}


void LibraryGateway::uninitialize(Main::RootManager *manager)
{
  // Unextend Seeker.
  SeekerExtension::unextend(manager->getSeeker(), this->seekerExtensionOverrides);
  this->seekerExtensionOverrides = 0;
  RootScopeHandlerExtension::unextend(manager->getRootScopeHandler(), this->rootScopeHandlerExtensionOverrides);
  this->rootScopeHandlerExtensionOverrides = 0;
  RootManagerExtension::unextend(manager, this->rootManagerExtensionOverrides);
  this->rootManagerExtensionOverrides = 0;

  // Reset run-time objects.
  this->rtGrammarMgr.reset();
  this->rtAstMgr.reset();
  this->rtBuildMgr.reset();

  // Reset generators.
  this->buildManager.reset();
  this->astProcessor.reset();
  this->generator.reset();
  this->commandGenerator.reset();
  this->expressionGenerator.reset();
  this->typeGenerator.reset();
  this->globalItemRepo.reset();
  this->nodePathResolver.reset();
  this->astHelper.reset();

  this->grammarFactory->cleanGrammar();

  this->removeGlobalDefs(manager);
  this->removeBuiltInTypes(manager);
}


void LibraryGateway::createBuiltInTypes(Core::Main::RootManager *manager)
{
  Core::Data::Ast::Identifier identifier;
  auto root = manager->getRootScope().get();
  SharedPtr<Ast::Template> tmplt;

  // Void
  identifier.setValue(S("Void"));
  manager->getSeeker()->doSet(&identifier, root, Ast::VoidType::create().get());

  // Null
  identifier.setValue(S("Null"));
  manager->getSeeker()->doSet(
    &identifier, root,
    Ast::IntegerType::create({
      { S("withSign"), TiBool(false) },
      { S("nullLiteral"), TiBool(true) }
    }).get()
  );

  // Int
  auto defaultIntBitCount = Core::Data::Ast::IntegerLiteral::create({{ S("value"), TiStr(S("32")) }});
  tmplt = Ast::Template::create();
  tmplt->setVarDefs(Core::Data::Ast::List::create({}, {
    newSrdObj<Ast::TemplateVarDef>(S("bitCount"), Ast::TemplateVarType::INTEGER, defaultIntBitCount)
  }));
  tmplt->setBody(Ast::IntegerType::create({ { S("withSign"), TiBool(true) } }));
  identifier.setValue(S("Int"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // Word
  auto defaultWordBitCount = Core::Data::Ast::IntegerLiteral::create({{ S("value"), TiStr(S("32")) }});
  tmplt = Ast::Template::create();
  tmplt->setVarDefs(Core::Data::Ast::List::create({}, {
    newSrdObj<Ast::TemplateVarDef>(S("bitCount"), Ast::TemplateVarType::INTEGER, defaultWordBitCount)
  }));
  tmplt->setBody(Ast::IntegerType::create({ { S("withSign"), TiBool(false) } }));
  identifier.setValue(S("Word"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // Float
  auto defaultFloatBitCount = Core::Data::Ast::IntegerLiteral::create({{ S("value"), TiStr(S("32")) }});
  tmplt = Ast::Template::create();
  tmplt->setVarDefs(Core::Data::Ast::List::create({}, {
    newSrdObj<Ast::TemplateVarDef>(S("bitCount"), Ast::TemplateVarType::INTEGER, defaultFloatBitCount)
  }));
  tmplt->setBody(Ast::FloatType::create());
  identifier.setValue(S("Float"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // ptr
  tmplt = Ast::Template::create();
  auto defaultPtrType = Core::Data::Ast::Identifier::create({{ S("value"), TiStr(S("Void")) }});
  tmplt->setVarDefs(Core::Data::Ast::List::create({}, {
    newSrdObj<Ast::TemplateVarDef>(S("type"), Ast::TemplateVarType::TYPE, defaultPtrType)
  }));
  tmplt->setBody(Ast::PointerType::create());
  identifier.setValue(S("ptr"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // ref
  tmplt = Ast::Template::create();
  tmplt->setVarDefs(Core::Data::Ast::List::create({}, {
    newSrdObj<Ast::TemplateVarDef>(S("type"), Ast::TemplateVarType::TYPE)
  }));
  tmplt->setBody(Ast::ReferenceType::create({ { S("mode"), Ast::ReferenceMode(Ast::ReferenceMode::EXPLICIT) } }));
  identifier.setValue(S("ref"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // temp_ref
  tmplt = Ast::Template::create();
  tmplt->setVarDefs(Core::Data::Ast::List::create({}, {
    newSrdObj<Ast::TemplateVarDef>(S("type"), Ast::TemplateVarType::TYPE)
  }));
  tmplt->setBody(Ast::ReferenceType::create({ { S("mode"), Ast::ReferenceMode(Ast::ReferenceMode::TEMP_EXPLICIT) } }));
  identifier.setValue(S("temp_ref"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // iref
  tmplt = Ast::Template::create();
  tmplt->setVarDefs(Core::Data::Ast::List::create({}, {
    newSrdObj<Ast::TemplateVarDef>(S("type"), Ast::TemplateVarType::TYPE)
  }));
  tmplt->setBody(Ast::ReferenceType::create({ { S("mode"), Ast::ReferenceMode(Ast::ReferenceMode::IMPLICIT) } }));
  identifier.setValue(S("iref"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // ndref
  tmplt = Ast::Template::create();
  tmplt->setVarDefs(Core::Data::Ast::List::create({}, {
    newSrdObj<Ast::TemplateVarDef>(S("type"), Ast::TemplateVarType::TYPE)
  }));
  tmplt->setBody(Ast::ReferenceType::create({ { S("mode"), Ast::ReferenceMode(Ast::ReferenceMode::NO_DEREF) } }));
  identifier.setValue(S("ndref"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // array
  auto defaultArraySize = Core::Data::Ast::IntegerLiteral::create({{ S("value"), TiStr(S("1")) }});
  tmplt = Ast::Template::create();
  tmplt->setVarDefs(Core::Data::Ast::List::create({}, {
    newSrdObj<Ast::TemplateVarDef>(S("type"), Ast::TemplateVarType::TYPE),
    newSrdObj<Ast::TemplateVarDef>(S("size"), Ast::TemplateVarType::INTEGER, defaultArraySize)
  }));
  tmplt->setBody(Ast::ArrayType::create());
  identifier.setValue(S("array"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());
}


void LibraryGateway::removeBuiltInTypes(Core::Main::RootManager *manager)
{
  Core::Data::Ast::Identifier identifier;
  auto root = manager->getRootScope().get();

  identifier.setValue(S("Void"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(S("Null"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(S("Int"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(S("Word"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(S("Float"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(S("ptr"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(S("ref"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(S("temp_ref"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(S("iref"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(S("ndref"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(S("array"));
  manager->getSeeker()->tryRemove(&identifier, root);
}


void LibraryGateway::createGlobalDefs(Core::Main::RootManager *manager)
{
  manager->processString(S(R"SRC(
    module Process {
      def argCount: Int;
      def args: ptr[array[ptr[array[Word[8]]]]];
      def language: ptr[array[Word[8]]];
      def platform: ptr[array[Word[8]]];
    };
    عرّف الـعملية: لقب Process؛
    @دمج عرف Process: وحدة
    {
      عرف عدد_المعطيات: لقب argCount؛
      عرف المعطيات: لقب args؛
      عرف اللغة: لقب language؛
      عرف النظام: لقب platform؛
    }؛
  )SRC"), S("spp"));
}


void LibraryGateway::removeGlobalDefs(Core::Main::RootManager *manager)
{
  Core::Data::Ast::Identifier identifier;
  auto root = manager->getRootScope().get();

  identifier.setValue(S("Process"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(S("الـعملية"));
  manager->getSeeker()->tryRemove(&identifier, root);
}


void LibraryGateway::initializeGlobalItemRepo(Core::Main::RootManager *manager)
{
  auto argCount = manager->getProcessArgCount();
  auto args = manager->getProcessArgs();
  auto language = manager->getLanguage().getBuf();

  Char const *platform;
  #ifdef WINDOWS
    platform = "windows";
  #elif __APPLE__
    platform = "macos";
  #else
    platform = "linux";
  #endif

  this->globalItemRepo->addItem(S("!Process.argCount"), sizeof(argCount), &argCount);
  this->globalItemRepo->addItem(S("!Process.args"), sizeof(args), &args);
  this->globalItemRepo->addItem(S("!Process.language"), sizeof(language), &language);
  this->globalItemRepo->addItem(S("!Process.platform"), sizeof(platform), &platform);
  this->globalItemRepo->addItem(S("!Core.rootManager"), sizeof(void*), &manager);
  this->globalItemRepo->addItem(
    S("RootManager_importFile"), (void*)&RootManagerExtension::_importFile
  );
  Rt::GrammarMgr::initializeRuntimePointers(this->globalItemRepo.get(), this->rtGrammarMgr.get());
  Rt::AstMgr::initializeRuntimePointers(this->globalItemRepo.get(), this->rtAstMgr.get());
  Rt::BuildMgr::initializeRuntimePointers(this->globalItemRepo.get(), this->rtBuildMgr.get());
}

} // namespace
