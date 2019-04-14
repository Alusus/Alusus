/**
 * @file Spp/LibraryGateway.cpp
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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

  // Create the generator.
  this->typeGenerator = std::make_shared<CodeGen::TypeGenerator>(this->astHelper.get());
  this->expressionGenerator = std::make_shared<CodeGen::ExpressionGenerator>(this->astHelper.get());
  this->commandGenerator = std::make_shared<CodeGen::CommandGenerator>(this->astHelper.get());
  this->generator = std::make_shared<CodeGen::Generator>(
    manager,
    this->astHelper.get(),
    this->typeGenerator.get(),
    this->commandGenerator.get(),
    this->expressionGenerator.get()
  );
  this->targetGenerator = std::make_shared<LlvmCodeGen::TargetGenerator>();

  // Extend Core singletons.
  this->seekerExtensionOverrides = SeekerExtension::extend(manager->getSeeker(), this->astHelper);
  this->rootScopeHandlerExtensionOverrides = RootScopeHandlerExtension::extend(
    manager->getRootScopeHandler(), manager, this->astHelper, this->macroProcessor,
    this->generator, this->targetGenerator
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
}


void LibraryGateway::uninitialize(Main::RootManager *manager)
{
  // Unextend Seeker.
  SeekerExtension::unextend(manager->getSeeker(), this->seekerExtensionOverrides);
  this->seekerExtensionOverrides = 0;
  RootScopeHandlerExtension::unextend(manager->getRootScopeHandler(), this->rootScopeHandlerExtensionOverrides);
  this->rootScopeHandlerExtensionOverrides = 0;

  // Reset generators.
  this->targetGenerator.reset();
  this->generator.reset();
  this->commandGenerator.reset();
  this->expressionGenerator.reset();
  this->typeGenerator.reset();
  this->nodePathResolver.reset();
  this->astHelper.reset();

  // Clean the grammar.
  GrammarFactory factory;
  factory.cleanGrammar(manager->getRootScope().get());

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
  tmplt->setVarDefs({{ S("bitCount"), Ast::Template::VarType::INTEGER, defaultIntBitCount }});
  tmplt->setTemplateBody(Ast::IntegerType::create({ { S("withSign"), TiBool(true) } }));
  identifier.setValue(S("Int"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // Word
  auto defaultWordBitCount = Core::Data::Ast::IntegerLiteral::create({{ S("value"), TiStr(S("32")) }});
  tmplt = Ast::Template::create();
  tmplt->setVarDefs({{ S("bitCount"), Ast::Template::VarType::INTEGER, defaultWordBitCount }});
  tmplt->setTemplateBody(Ast::IntegerType::create({ { S("withSign"), TiBool(false) } }));
  identifier.setValue(S("Word"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // Float
  auto defaultFloatBitCount = Core::Data::Ast::IntegerLiteral::create({{ S("value"), TiStr(S("32")) }});
  tmplt = Ast::Template::create();
  tmplt->setVarDefs({{ S("bitCount"), Ast::Template::VarType::INTEGER, defaultFloatBitCount }});
  tmplt->setTemplateBody(Ast::FloatType::create());
  identifier.setValue(S("Float"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // ptr
  tmplt = Ast::Template::create();
  auto defaultPtrType = Core::Data::Ast::Identifier::create({{ S("value"), TiStr(S("Void")) }});
  tmplt->setVarDefs({{ S("type"), Ast::Template::VarType::TYPE, defaultPtrType }});
  tmplt->setTemplateBody(Ast::PointerType::create());
  identifier.setValue(S("ptr"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // ref
  tmplt = Ast::Template::create();
  tmplt->setVarDefs({{ S("type"), Ast::Template::VarType::TYPE }});
  tmplt->setTemplateBody(Ast::ReferenceType::create());
  identifier.setValue(S("ref"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // array
  auto defaultArraySize = Core::Data::Ast::IntegerLiteral::create({{ S("value"), TiStr(S("1")) }});
  tmplt = Ast::Template::create();
  tmplt->setVarDefs({
    { S("type"), Ast::Template::VarType::TYPE },
    { S("size"), Ast::Template::VarType::INTEGER, defaultArraySize }
  });
  tmplt->setTemplateBody(Ast::ArrayType::create());
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

} // namespace
