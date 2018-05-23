/**
 * @file Spp/LibraryGateway.cpp
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
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

  // Extend Seeker.
  this->seekerExtensionOverrides = SeekerExtension::extend(manager->getSeeker(), this->astHelper);

  // Create the generator.
  this->noOpTargetGenerator = std::make_shared<CodeGen::NoOpTargetGenerator>();
  this->typeGenerator = std::make_shared<CodeGen::TypeGenerator>(this->astHelper.get());
  this->expressionGenerator = std::make_shared<CodeGen::ExpressionGenerator>(
    this->astHelper.get(), this->noOpTargetGenerator.get()
  );
  this->commandGenerator = std::make_shared<CodeGen::CommandGenerator>(this->astHelper.get());
  this->generator = std::make_shared<CodeGen::Generator>(
    manager,
    this->astHelper.get(),
    this->typeGenerator.get(),
    this->commandGenerator.get(),
    this->expressionGenerator.get()
  );
  this->targetGenerator = std::make_shared<LlvmCodeGen::TargetGenerator>();

  // Add the grammar.
  GrammarFactory factory;
  factory.createGrammar(
    manager->getRootScope().get(), manager, this->astHelper.get(), this->generator.get(), this->targetGenerator.get()
  );

  // Add built-in types and functions.
  this->createBuiltInTypes(manager);
  this->createBuiltInFunctions(manager);
}


void LibraryGateway::uninitialize(Main::RootManager *manager)
{
  // Unextend Seeker.
  SeekerExtension::unextend(manager->getSeeker(), this->seekerExtensionOverrides);
  this->seekerExtensionOverrides = 0;

  // Reset generators.
  this->targetGenerator.reset();
  this->generator.reset();
  this->commandGenerator.reset();
  this->expressionGenerator.reset();
  this->typeGenerator.reset();
  this->noOpTargetGenerator.reset();
  this->nodePathResolver.reset();
  this->astHelper.reset();

  // Clean the grammar.
  GrammarFactory factory;
  factory.cleanGrammar(manager->getRootScope().get());

  // Remove built-in types and functions.
  this->removeBuiltInFunctions(manager);
  this->removeBuiltInTypes(manager);
}


void LibraryGateway::createBuiltInTypes(Core::Main::RootManager *manager)
{
  Core::Data::Ast::Identifier identifier;
  auto root = manager->getRootScope().get();
  SharedPtr<Ast::Template> tmplt;

  // Void
  identifier.setValue(STR("Void"));
  manager->getSeeker()->doSet(&identifier, root, Ast::VoidType::create().get());

  // Int
  tmplt = Ast::Template::create();
  tmplt->setVarDefs({{ STR("bitCount"), Ast::Template::VarType::INTEGER }});
  tmplt->setTemplateBody(Ast::IntegerType::create());
  identifier.setValue(STR("Int"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // Float
  tmplt = Ast::Template::create();
  tmplt->setVarDefs({{ STR("bitCount"), Ast::Template::VarType::INTEGER }});
  tmplt->setTemplateBody(Ast::FloatType::create());
  identifier.setValue(STR("Float"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // ptr
  tmplt = Ast::Template::create();
  tmplt->setVarDefs({{ STR("type"), Ast::Template::VarType::TYPE }});
  tmplt->setTemplateBody(Ast::PointerType::create());
  identifier.setValue(STR("ptr"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // ref
  tmplt = Ast::Template::create();
  tmplt->setVarDefs({{ STR("type"), Ast::Template::VarType::TYPE }});
  tmplt->setTemplateBody(Ast::ReferenceType::create());
  identifier.setValue(STR("ref"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());

  // array
  tmplt = Ast::Template::create();
  tmplt->setVarDefs({
    { STR("type"), Ast::Template::VarType::TYPE },
    { STR("size"), Ast::Template::VarType::INTEGER }
  });
  tmplt->setTemplateBody(Ast::ArrayType::create());
  identifier.setValue(STR("array"));
  manager->getSeeker()->doSet(&identifier, root, tmplt.get());
}


void LibraryGateway::removeBuiltInTypes(Core::Main::RootManager *manager)
{
  Core::Data::Ast::Identifier identifier;
  auto root = manager->getRootScope().get();

  identifier.setValue(STR("Void"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(STR("Int"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(STR("Float"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(STR("ptr"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(STR("ref"));
  manager->getSeeker()->tryRemove(&identifier, root);

  identifier.setValue(STR("array"));
  manager->getSeeker()->tryRemove(&identifier, root);
}


void LibraryGateway::createBuiltInFunctions(Core::Main::RootManager *manager)
{
  Core::Data::Ast::Identifier identifier;
  auto root = manager->getRootScope().get();
  TioSharedPtr hook;

  Char const *binaryOps[] = { STR("add"), STR("sub"), STR("mul"), STR("div") };
  Char const *intBinaryOps[] = {
    STR("rem"), STR("shr"), STR("shl"), STR("and"), STR("or"), STR("xor")
  };
  Char const *comparisonOps[] = {
    STR("equal"), STR("notEqual"),
    STR("greaterThan"), STR("greaterThanOrEqual"),
    STR("lessThan"), STR("lessThanOrEqual")
  };
  Char const *assignmentOps[] = {
    STR("addAssign"), STR("subAssign"), STR("mulAssign"), STR("divAssign")
  };
  Char const *intAssignmentOps[] = {
    STR("remAssign"), STR("shrAssign"), STR("shlAssign"), STR("andAssign"), STR("orAssign"), STR("xorAssign")
  };
  Char const *unaryValOps[] = {
    STR("neg"), STR("pos")
  };
  Char const *intUnaryValOps[] = {
    STR("not")
  };
  Char const *unaryVarOps[] = {
    STR("earlyInc"), STR("earlyDec"), STR("lateInc"), STR("lateDec")
  };

  Char const *types[] = {
    STR("Int[8]"), STR("Int[16]"), STR("Int[32]"), STR("Int[64]"), STR("Float[32]"), STR("Float[64]")
  };
  Char const *refTypes[] = {
    STR("ref[Int[8]]"), STR("ref[Int[16]]"), STR("ref[Int[32]]"), STR("ref[Int[64]]"),
    STR("ref[Float[32]]"), STR("ref[Float[64]]")
  };
  Char const *intTypes[] = {
    STR("Int[8]"), STR("Int[16]"), STR("Int[32]"), STR("Int[64]")
  };
  Char const *intRefTypes[] = {
    STR("ref[Int[8]]"), STR("ref[Int[16]]"), STR("ref[Int[32]]"), STR("ref[Int[64]]")
  };

  // Add binary math operators.
  for (Int i = 0; i < sizeof(binaryOps) / sizeof(binaryOps[0]); ++i) {
    Str path = STR("__");
    path += binaryOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(types) / sizeof(types[0]); ++j) {
      Str funcName = STR("#");
      funcName += binaryOps[i];

      manager->getSeeker()->set(&identifier, root,
        [=,&hook](TiObject *&obj, Core::Notices::Notice*)->Core::Data::Seeker::Verb {
          if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
          hook = this->createBinaryFunction(manager, funcName.c_str(), types[j], types[j], types[j]);
          obj = hook.get();
          return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
        }, 0
      );
    }
  }

  // Add int-only binary math operators.
  for (Int i = 0; i < sizeof(intBinaryOps) / sizeof(intBinaryOps[0]); ++i) {
    Str path = STR("__");
    path += intBinaryOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(intTypes) / sizeof(intTypes[0]); ++j) {
      Str funcName = STR("#");
      funcName += intBinaryOps[i];

      manager->getSeeker()->set(&identifier, root,
        [=,&hook](TiObject *&obj, Core::Notices::Notice*)->Core::Data::Seeker::Verb {
          if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
          hook = this->createBinaryFunction(manager, funcName.c_str(), intTypes[j], intTypes[j], intTypes[j]);
          obj = hook.get();
          return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
        }, 0
      );
    }
  }

  // Add comparison operators.
  for (Int i = 0; i < sizeof(comparisonOps) / sizeof(comparisonOps[0]); ++i) {
    Str path = STR("__");
    path += comparisonOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(types) / sizeof(types[0]); ++j) {
      Str funcName = STR("#");
      funcName += comparisonOps[i];

      manager->getSeeker()->set(&identifier, root,
        [=,&hook](TiObject *&obj, Core::Notices::Notice*)->Core::Data::Seeker::Verb {
          if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
          hook = this->createBinaryFunction(manager, funcName.c_str(), types[j], types[j], STR("Int[1]"));
          obj = hook.get();
          return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
        }, 0
      );
    }
  }

  // Add assignment operators.
  for (Int i = 0; i < sizeof(assignmentOps) / sizeof(assignmentOps[0]); ++i) {
    Str path = STR("__");
    path += assignmentOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(types) / sizeof(types[0]); ++j) {
      Str funcName = STR("#");
      funcName += assignmentOps[i];

      manager->getSeeker()->set(&identifier, root,
        [=,&hook](TiObject *&obj, Core::Notices::Notice*)->Core::Data::Seeker::Verb {
          if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
          hook = this->createBinaryFunction(manager, funcName.c_str(), refTypes[j], types[j], refTypes[j]);
          obj = hook.get();
          return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
        }, 0
      );
    }
  }

  // Add int-only assignment operators.
  for (Int i = 0; i < sizeof(intAssignmentOps) / sizeof(intAssignmentOps[0]); ++i) {
    Str path = STR("__");
    path += intAssignmentOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(intTypes) / sizeof(intTypes[0]); ++j) {
      Str funcName = STR("#");
      funcName += intAssignmentOps[i];

      manager->getSeeker()->set(&identifier, root,
        [=,&hook](TiObject *&obj, Core::Notices::Notice*)->Core::Data::Seeker::Verb {
          if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
          hook = this->createBinaryFunction(manager, funcName.c_str(), intRefTypes[j], intTypes[j], intRefTypes[j]);
          obj = hook.get();
          return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
        }, 0
      );
    }
  }

  // Add unary val operators.
  for (Int i = 0; i < sizeof(unaryValOps) / sizeof(unaryValOps[0]); ++i) {
    Str path = STR("__");
    path += unaryValOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(types) / sizeof(types[0]); ++j) {
      Str funcName = STR("#");
      funcName += unaryValOps[i];

      manager->getSeeker()->set(&identifier, root,
        [=,&hook](TiObject *&obj, Core::Notices::Notice*)->Core::Data::Seeker::Verb {
          if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
          hook = this->createUnaryFunction(manager, funcName.c_str(), types[j], types[j]);
          obj = hook.get();
          return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
        }, 0
      );
    }
  }

  // Add int-only unary val operators.
  for (Int i = 0; i < sizeof(intUnaryValOps) / sizeof(intUnaryValOps[0]); ++i) {
    Str path = STR("__");
    path += intUnaryValOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(intTypes) / sizeof(intTypes[0]); ++j) {
      Str funcName = STR("#");
      funcName += intUnaryValOps[i];

      manager->getSeeker()->set(&identifier, root,
        [=,&hook](TiObject *&obj, Core::Notices::Notice*)->Core::Data::Seeker::Verb {
          if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
          hook = this->createUnaryFunction(manager, funcName.c_str(), intTypes[j], intTypes[j]);
          obj = hook.get();
          return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
        }, 0
      );
    }
  }

  // Add unary var operators.
  for (Int i = 0; i < sizeof(unaryVarOps) / sizeof(unaryVarOps[0]); ++i) {
    Str path = STR("__");
    path += unaryVarOps[i];
    identifier.setValue(path.c_str());

    for (Int j = 0; j < sizeof(types) / sizeof(types[0]); ++j) {
      Str funcName = STR("#");
      funcName += unaryVarOps[i];

      manager->getSeeker()->set(&identifier, root,
        [=,&hook](TiObject *&obj, Core::Notices::Notice*)->Core::Data::Seeker::Verb {
          if (obj != 0) return Core::Data::Seeker::Verb::MOVE;
          hook = this->createUnaryFunction(manager, funcName.c_str(), refTypes[j], types[j]);
          obj = hook.get();
          return Core::Data::Seeker::Verb::PERFORM_AND_MOVE;
        }, 0
      );
    }
  }
}


void LibraryGateway::removeBuiltInFunctions(Core::Main::RootManager *manager)
{
  Core::Data::Ast::Identifier identifier;
  auto root = manager->getRootScope().get();

  Char const *operations[] = {
    STR("add"), STR("sub"), STR("mul"), STR("div"),
    STR("rem"), STR("shr"), STR("shl"), STR("and"), STR("or"), STR("xor"),
    STR("equal"), STR("notEqual"),
    STR("greaterThan"), STR("greaterThanOrEqual"),
    STR("lessThan"), STR("lessThanOrEqual"),
    STR("addAssign"), STR("subAssign"), STR("mulAssign"), STR("divAssign"),
    STR("remAssign"), STR("shrAssign"), STR("shlAssign"), STR("andAssign"), STR("orAssign"), STR("xorAssign"),
    STR("neg"), STR("pos"),
    STR("not"),
    STR("earlyInc"), STR("earlyDec"), STR("lateInc"), STR("lateDec")
  };

  for (Int i = 0; i < sizeof(operations) / sizeof(operations[0]); ++i) {
    Str name = STR("__");
    name += operations[i];
    identifier.setValue(name.c_str());
    manager->getSeeker()->tryRemove(&identifier, root);
  }
}


SharedPtr<Ast::Function> LibraryGateway::createBinaryFunction(
  Core::Main::RootManager *manager, Char const *name, Char const *in1, Char const *in2, Char const *out
) {
  auto retType = manager->parseExpression(out);
  auto argTypes = Core::Data::Ast::Map::create(false, {}, {
    { STR("in1"), manager->parseExpression(in1) },
    { STR("in2"), manager->parseExpression(in2) }
  });
  return Ast::Function::create({
    { STR("name"), TiStr(name) }
  }, {
    { STR("argTypes"), argTypes },
    { STR("retType"), retType }
  });
}


SharedPtr<Ast::Function> LibraryGateway::createUnaryFunction(
  Core::Main::RootManager *manager, Char const *name, Char const *in, Char const *out
) {
  auto retType = manager->parseExpression(out);
  auto argTypes = Core::Data::Ast::Map::create(false, {}, {
    { STR("in"), manager->parseExpression(in) }
  });
  return Ast::Function::create({
    { STR("name"), TiStr(name) }
  }, {
    { STR("argTypes"), argTypes },
    { STR("retType"), retType }
  });
}

} // namespace
