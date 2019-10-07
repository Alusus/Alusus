/**
 * @file Spp/LibraryGateway.cpp
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
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
  // Create AST helpers.
  this->nodePathResolver = std::make_shared<Ast::NodePathResolver>();
  this->astHelper = std::make_shared<Ast::Helper>(manager, this->nodePathResolver.get());

  // Create the preprocessor.
  this->macroProcessor = std::make_shared<CodeGen::MacroProcessor>(this->astHelper.get());

  // Create and initialize global item repo.
  this->globalItemRepo = std::make_shared<CodeGen::GlobalItemRepo>();
  this->initializeGlobalItemRepo(manager);

  // Create the generator.
  this->typeGenerator = std::make_shared<CodeGen::TypeGenerator>(this->astHelper.get());
  this->expressionGenerator = std::make_shared<CodeGen::ExpressionGenerator>(this->astHelper.get());
  this->commandGenerator = std::make_shared<CodeGen::CommandGenerator>(this->astHelper.get());
  this->generator = std::make_shared<CodeGen::Generator>(
    manager,
    this->astHelper.get(),
    this->globalItemRepo.get(),
    this->typeGenerator.get(),
    this->commandGenerator.get(),
    this->expressionGenerator.get()
  );
  this->targetGenerator = std::make_shared<LlvmCodeGen::TargetGenerator>();

  // Extend Core singletons.
  this->seekerExtensionOverrides = SeekerExtension::extend(manager->getSeeker(), this->astHelper);
  this->rootScopeHandlerExtensionOverrides = RootScopeHandlerExtension::extend(manager->getRootScopeHandler(), manager);
  this->rootManagerExtensionOverrides = RootManagerExtension::extend(
    manager, this->astHelper, this->macroProcessor, this->generator, this->targetGenerator
  );

  // Prepare the target generator.
  this->targetGenerator->prepareBuild();

  // Add the grammar.
  GrammarFactory factory;
  factory.createGrammar(
    manager->getRootScope().get(), manager, this->astHelper.get(), this->macroProcessor.get(),
    this->generator.get(), this->targetGenerator.get()
  );

  this->createBuiltInTypes(manager);
  this->createGlobalDefs(manager);
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

  // Reset generators.
  this->targetGenerator.reset();
  this->generator.reset();
  this->commandGenerator.reset();
  this->expressionGenerator.reset();
  this->typeGenerator.reset();
  this->globalItemRepo.reset();
  this->nodePathResolver.reset();
  this->astHelper.reset();

  // Clean the grammar.
  GrammarFactory factory;
  factory.cleanGrammar(manager->getRootScope().get());

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
    std::make_shared<Ast::TemplateVarDef>(S("bitCount"), Ast::TemplateVarType::INTEGER, defaultIntBitCount)
  }));
  tmplt->setBody(Ast::IntegerType::create({ { S("withSign"), TiBool(true) } }));
  identifier.setValue(S("Int"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // Word
  auto defaultWordBitCount = Core::Data::Ast::IntegerLiteral::create({{ S("value"), TiStr(S("32")) }});
  tmplt = Ast::Template::create();
  tmplt->setVarDefs(Core::Data::Ast::List::create({}, {
    std::make_shared<Ast::TemplateVarDef>(S("bitCount"), Ast::TemplateVarType::INTEGER, defaultWordBitCount)
  }));
  tmplt->setBody(Ast::IntegerType::create({ { S("withSign"), TiBool(false) } }));
  identifier.setValue(S("Word"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // Float
  auto defaultFloatBitCount = Core::Data::Ast::IntegerLiteral::create({{ S("value"), TiStr(S("32")) }});
  tmplt = Ast::Template::create();
  tmplt->setVarDefs(Core::Data::Ast::List::create({}, {
    std::make_shared<Ast::TemplateVarDef>(S("bitCount"), Ast::TemplateVarType::INTEGER, defaultFloatBitCount)
  }));
  tmplt->setBody(Ast::FloatType::create());
  identifier.setValue(S("Float"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // ptr
  tmplt = Ast::Template::create();
  auto defaultPtrType = Core::Data::Ast::Identifier::create({{ S("value"), TiStr(S("Void")) }});
  tmplt->setVarDefs(Core::Data::Ast::List::create({}, {
    std::make_shared<Ast::TemplateVarDef>(S("type"), Ast::TemplateVarType::TYPE, defaultPtrType)
  }));
  tmplt->setBody(Ast::PointerType::create());
  identifier.setValue(S("ptr"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // ref
  tmplt = Ast::Template::create();
  tmplt->setVarDefs(Core::Data::Ast::List::create({}, {
    std::make_shared<Ast::TemplateVarDef>(S("type"), Ast::TemplateVarType::TYPE)
  }));
  tmplt->setBody(Ast::ReferenceType::create());
  identifier.setValue(S("ref"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // array
  auto defaultArraySize = Core::Data::Ast::IntegerLiteral::create({{ S("value"), TiStr(S("1")) }});
  tmplt = Ast::Template::create();
  tmplt->setVarDefs(Core::Data::Ast::List::create({}, {
    std::make_shared<Ast::TemplateVarDef>(S("type"), Ast::TemplateVarType::TYPE),
    std::make_shared<Ast::TemplateVarDef>(S("size"), Ast::TemplateVarType::INTEGER, defaultArraySize)
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
      def os: ptr[array[Word[8]]];
    };
    module Core {
      def rootManager: ptr;
      def parser: ptr;
      def noticeStore: ptr;
      def _importFile: @expname[RootManager_importFile] function (
        rootManager: ptr, filename: ptr[array[Word[8]]]
      );
      function importFile(filename: ptr[array[Word[8]]]) {
        _importFile(rootManager, filename);
      };
    };
    module Spp {
      def _dumpLlvmIrForElement: @expname[RootManager_dumpLlvmIrForElement] function (
        rootManager: ptr, element: ptr, noticeStore: ptr, parser: ptr
      );
      function dumpLlvmIrForElement (element: ptr) {
        _dumpLlvmIrForElement(Core.rootManager, element, Core.noticeStore, Core.parser);
      };
      def _buildObjectFileForElement: @expname[RootManager_buildObjectFileForElement] function (
        rootManager: ptr, element: ptr, filename: ptr[array[Word[8]]], noticeStore: ptr, parser: ptr
      ) => Word[1];
      function buildObjectFileForElement (element: ptr, filename: ptr[array[Word[8]]]) => Word[1] {
        return _buildObjectFileForElement(Core.rootManager, element, filename, Core.noticeStore, Core.parser);
      };
      def _getModifierStrings: @expname[RootManager_getModifierStrings] function (
        rootManager: ptr, element: ptr, modifierKwd: ptr[array[Word[8]]],
        resultStrs: ptr[ptr[array[ptr[array[Word[8]]]]]], resultCount: ptr[Word],
        noticeStore: ptr, parser: ptr
      ) => Word[1];
      function getModifierStrings(
        element: ptr, modifierKwd: ptr[array[Word[8]]],
        resultStrs: ptr[ptr[array[ptr[array[Word[8]]]]]], resultCount: ptr[Word]
      ) => Word[1] {
        return _getModifierStrings(
          Core.rootManager, element, modifierKwd, resultStrs, resultCount, Core.noticeStore, Core.parser
        );
      };
    };
  )SRC"), S("spp"));
}


void LibraryGateway::removeGlobalDefs(Core::Main::RootManager *manager)
{
  Core::Data::Ast::Identifier identifier;
  auto root = manager->getRootScope().get();

  identifier.setValue(S("Process"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(S("Core"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(S("Spp"));
  manager->getSeeker()->tryRemove(&identifier, root);
}


void LibraryGateway::initializeGlobalItemRepo(Core::Main::RootManager *manager)
{
  auto argCount = manager->getProcessArgCount();
  auto args = manager->getProcessArgs();
  auto language = manager->getLanguage().c_str();
#if defined(__MINGW32__) // Windows under MinGW i686.
  auto os = "Win32-i686";
#elif defined(__MINGW64__) // Windows under MinGW X86 64Bit.
  auto os = "Win32-x86_64";
#elif defined(APPLE) // Apple (Mac OS, iOS, Apple Watch).
  auto os = "Darwin";
#else // Linux, Android, ...etc.
  auto os = "Linux";
#endif
  this->globalItemRepo->addItem(S("Process.argCount"), sizeof(argCount), &argCount);
  this->globalItemRepo->addItem(S("Process.args"), sizeof(args), &args);
  this->globalItemRepo->addItem(S("Process.language"), sizeof(language), &language);
  this->globalItemRepo->addItem(S("Process.os"), sizeof(os), &os);
  this->globalItemRepo->addItem(S("Core.rootManager"), sizeof(void*), &manager);
  this->globalItemRepo->addItem(S("Core.parser"), sizeof(void*));
  this->globalItemRepo->addItem(S("Core.noticeStore"), sizeof(void*));
  this->globalItemRepo->addItem(
    S("RootManager_dumpLlvmIrForElement"), (void*)&RootManagerExtension::_dumpLlvmIrForElement
  );
  this->globalItemRepo->addItem(
    S("RootManager_buildObjectFileForElement"), (void*)&RootManagerExtension::_buildObjectFileForElement
  );
  this->globalItemRepo->addItem(
    S("RootManager_importFile"), (void*)&RootManagerExtension::_importFile
  );
  this->globalItemRepo->addItem(
    S("RootManager_getModifierStrings"), (void*)&RootManagerExtension::_getModifierStrings
  );
}

} // namespace
