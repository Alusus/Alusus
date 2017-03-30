/**
 * @file Spp/LlvmCodeGen/Generator.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::Generator.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"
#include <regex>

namespace Spp { namespace LlvmCodeGen
{

//==============================================================================
// Main Operation Functions

void Generator::initBindingCaches()
{
  Core::Basic::initBindingCaches(this, {
    &this->generateModule,
    &this->generateType,
    &this->generateBuiltInType,
    &this->generateUserType,
    &this->generateFunction,
    &this->generateBlock,
    &this->generateStatements
  });
}


void Generator::initBindings()
{
  this->nodePathResolver = std::make_shared<NodePathResolver>(this->seeker);

  this->generateModule = &Generator::_generateModule;
  this->generateType = &Generator::_generateType;
  this->generateBuiltInType = &Generator::_generateBuiltInType;
  this->generateUserType = &Generator::_generateUserType;
  this->generateFunction = &Generator::_generateFunction;
  this->generateBlock = &Generator::_generateBlock;
  this->generateStatements = &Generator::_generateStatements;
  //  this->generateStatement = &Generator::_generateStatement;
  //  this->generateIfStatement = &Generator::_generateIfStatement;
  //  this->generateWhileStatement = &Generator::_generateWhileStatement;
  //  this->generateExpression = &Generator::_generateExpression;
}


Str Generator::generateIr(Core::Data::Ast::Scope *root)
{
  if (root == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("root"), STR("Must not be null."));
  }

  llvm::InitializeNativeTarget();

  SharedPtr<llvm::DataLayout> llvmDataLayout(new llvm::DataLayout(""));
  SharedPtr<llvm::Module> llvmModule(new llvm::Module("AlususProgram", llvm::getGlobalContext()));
  llvmModule->setDataLayout(llvmDataLayout->getStringRepresentation());

  // Generates code for all modules.
  for (Int i = 0; i < root->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(root->get(i));
    if (def != 0) {
      auto module = def->getTarget().ti_cast_get<Spp::Ast::Module>();
      if (module != 0) {
        this->call<void>(this->generateModule, module, llvmModule.get());
      }
    }
  }

  Str out;
  llvm::raw_string_ostream ostream(out);
  llvm::PassManager passManager;
  passManager.add(llvm::createPrintModulePass(&ostream));
  passManager.run(*(llvmModule.get()));

  return out;
}


//==============================================================================
// Code Generation Functions

void Generator::_generateModule(TiObject *self, Spp::Ast::Module *astModule, llvm::Module *llvmModule)
{
  PREPARE_SELF(generator, Generator);
  for (Int i = 0; i < astModule->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(astModule->get(i));
    if (def != 0) {
      auto obj = def->getTarget().get();
      if (obj->isDerivedFrom<Spp::Ast::Module>()) {
        generator->generateModule(static_cast<Spp::Ast::Module*>(obj), llvmModule);
      } else if (obj->isDerivedFrom<Spp::Ast::Function>()) {
        generator->generateFunction(static_cast<Spp::Ast::Function*>(obj), llvmModule);
      }
      // TODO: Generate for types and global variables.
    }
  }
}


void Generator::_generateType(TiObject *self, Spp::Ast::Type *astType, llvm::Module *llvmModule)
{
  PREPARE_SELF(generator, Generator);

  auto cgType = astType->getExtra(Generator::META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Type>();
  if (cgType != 0) return;

  Str typeName;
  if (generator->isRootTemplateInstance(astType, typeName)) {
    if (typeName == STR("Int") || typeName == STR("Float") || typeName == STR("ptr")) {
      generator->generateBuiltInType(typeName.c_str(), astType, llvmModule);
      return;
    }
  }
  generator->generateUserType(astType, llvmModule);
}


void Generator::_generateBuiltInType(TiObject *self, Char const *typeName, Spp::Ast::Type *astType,
                                     llvm::Module *llvmModule)
{
  PREPARE_SELF(generator, Generator);
  Core::Data::Ast::Identifier identifier;
  if (SBSTR(typeName) == STR("Int")) {
    // Generate Int.
    identifier.setValue("bitCount");
    auto bitCountBox = ti_cast<Core::Basic::TioSharedBox>(
      generator->getSeeker()->doGet(&identifier, astType->getOwner())
    );
    if (bitCountBox == 0) {
      throw EXCEPTION(GenericException, STR("Could not find bitCount of built-in Int type."));
    }
    auto bitCount = bitCountBox->get().ti_cast_get<Core::Data::Ast::IntegerLiteral>();
    if (bitCount == 0) {
      throw EXCEPTION(GenericException, STR("Could not find bitCount of built-in Int type."));
    }
    auto cgType = std::make_shared<LlvmCodeGen::IntegerType>(std::stoi(bitCount->getValue().get()));
    astType->setExtra(Generator::META_EXTRA_NAME, cgType);
  } else if (SBSTR(typeName) == STR("Float")) {
    // Generate Float.
    identifier.setValue("bitCount");
    auto bitCountBox = ti_cast<Core::Basic::TioSharedBox>(
      generator->getSeeker()->doGet(&identifier, astType->getOwner())
    );
    if (bitCountBox == 0) {
      throw EXCEPTION(GenericException, STR("Could not find bitCount of built-in Int type."));
    }
    auto bitCount = bitCountBox->get().ti_cast_get<Core::Data::Ast::IntegerLiteral>();
    if (bitCount == 0) {
      throw EXCEPTION(GenericException, STR("Could not find bitCount of built-in Float type."));
    }
    auto cgType = std::make_shared<LlvmCodeGen::FloatType>(std::stoi(bitCount->getValue().get()));
    astType->setExtra(Generator::META_EXTRA_NAME, cgType);
  } else if (SBSTR(typeName) == STR("ptr")) {
    // Generate ptr.
    identifier.setValue("type");
    auto contentAstTypeBox = ti_cast<Core::Basic::TioSharedBox>(
      generator->getSeeker()->doGet(&identifier, astType->getOwner())
    );
    auto contentAstType = contentAstTypeBox->get().ti_cast_get<Spp::Ast::Type>();
    if (contentAstType == 0) {
      throw EXCEPTION(GenericException, STR("Could not find pointer content type."));
    }
    generator->generateType(contentAstType, llvmModule);
    auto contentCgType = contentAstType->getExtra(Generator::META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Type>();
    auto cgType = std::make_shared<LlvmCodeGen::PointerType>(contentCgType);
    astType->setExtra(Generator::META_EXTRA_NAME, cgType);
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("typeName"), STR("Invalid built-in type name."), typeName);
  }
}


void Generator::_generateUserType(TiObject *self, Spp::Ast::Type *astType, llvm::Module *llvmModule)
{
  // TODO:
}


void Generator::_generateFunction(TiObject *self, Spp::Ast::Function *astFunc, llvm::Module *llvmModule)
{
  PREPARE_SELF(generator, Generator);

  auto genType = astFunc->getExtra(Generator::META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Function>();
  if (genType != 0) return;

  // Construct the list of argument LLVM types.
  // TODO: Support functions that take no args.
  auto astArgs = astFunc->getArgTypes().get();
  auto argCount = astArgs == 0 ? 0 : astArgs->getCount();
  std::vector<llvm::Type*> llvmArgTypes(argCount);
  for (Int i = 0; i < argCount; ++i) {
    llvmArgTypes[i] = generator->getGeneratedLlvmType(astArgs->get(i), llvmModule);
  }

  // Get the return LLVM type.
  llvm::Type *llvmRetType = 0;
  if (astFunc->getRetType() != 0) {
    llvmRetType = generator->getGeneratedLlvmType(astFunc->getRetType().get(), llvmModule);
  } else {
    llvmRetType = llvm::Type::getVoidTy(llvm::getGlobalContext());
  }

  // Generate the function object.
  auto llvmFuncType = llvm::FunctionType::get(llvmRetType, llvmArgTypes, false);
  Str name = generator->getFunctionName(astFunc);
  auto cgFunc = std::make_shared<LlvmCodeGen::UserFunction>();
  cgFunc->setLlvmFunction(llvm::Function::Create(llvmFuncType, llvm::Function::ExternalLinkage, name, llvmModule));
  astFunc->setExtra(Generator::META_EXTRA_NAME, cgFunc);

  auto astBlock = astFunc->getBody().get();
  if (astBlock != 0) {
    // Create an llvm block and its irbuilder.
    auto cgBlock = std::make_shared<LlvmCodeGen::Block>();
    cgBlock->setLlvmBlock(llvm::BasicBlock::Create(
      llvm::getGlobalContext(),
      generator->getNewBlockName(),
      cgFunc->getLlvmFunction()
    ));
    cgBlock->setIrBuilder(new llvm::IRBuilder<>(cgBlock->getLlvmBlock()));
    astBlock->setExtra(Generator::META_EXTRA_NAME, cgBlock);

    // Create variables for each argument and assign them to cgFunc.
    auto i = 0;
    for (auto iter = cgFunc->getLlvmFunction()->arg_begin(); i != argCount; ++iter, ++i) {
      iter->setName(astArgs->getKey(i).c_str());
      auto llvmAlloca = cgBlock->getIrBuilder()->CreateAlloca(llvmArgTypes[i], 0, astArgs->getKey(i).c_str());
      auto llvmStore = cgBlock->getIrBuilder()->CreateStore(iter, llvmAlloca);
      cgFunc->addArg(llvmAlloca, llvmStore);
    }

    // Generate the function's statements.
    generator->generateStatements(astBlock, cgFunc->getLlvmFunction());
  }
}

// TODO:

void Generator::_generateBlock(TiObject *self, Spp::Ast::Block *astBlock, llvm::Function *llvmFunc)
{
}


void Generator::_generateStatements(TiObject *self, Spp::Ast::Block *astBlock, llvm::Function *llvmFunc)
{
}


//==============================================================================
// Helper Functions

Spp::Ast::Type* Generator::getGeneratedType(TiObject *ref, llvm::Module *llvmModule)
{
  Spp::Ast::Type *type = ti_cast<Spp::Ast::Type>(ref);
  if (type == 0) {
    this->seeker->doForeach(ref, ref,
      [=,&type](TiObject *obj)->Core::Data::Seeker::SeekVerb
      {
        type = ti_cast<Spp::Ast::Type>(obj);
        if (type != 0) {
          return Core::Data::Seeker::SeekVerb::STOP;
        }
        // TODO: Support template defaults.
        // TODO: Handle aliases.
        return Core::Data::Seeker::SeekVerb::MOVE;
      });
  }
  if (type == 0) {
    throw EXCEPTION(GenericException, STR("AST Type is not found."));
  }
  this->call<void>(this->generateType, type, llvmModule);
  return type;
}


llvm::Type* Generator::getGeneratedLlvmType(TiObject *ref, llvm::Module *llvmModule)
{
  auto type = this->getGeneratedType(ref, llvmModule);
  auto genType = type->getExtra(Generator::META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Type>();
  if (genType == 0) {
    throw EXCEPTION(GenericException, STR("AST Type is missing the generated Type object."));
  }
  return genType->getLlvmType();
}


Str const& Generator::getFunctionName(Spp::Ast::Function *astFunc)
{
  if (astFunc->getName()->getStr() == STR("")) {
    Str name = this->nodePathResolver->doResolve(astFunc);
    // Replace special characters with _.
    Str formattedName = std::regex_replace(name, std::regex("[^a-zA-Z0-9_]"), STR("_"));
    astFunc->setName(formattedName.c_str());
  }
  return astFunc->getName()->getStr();
}


Str Generator::getNewBlockName()
{
  static Int index = 0;
  return Str("block") + std::to_string(index++);
}


Bool Generator::isRootTemplateInstance(Ast::Type *type, Str &name)
{
  auto block = ti_cast<Ast::Block>(type->getOwner());
  if (block == 0) return false;
  auto tmplt = ti_cast<Ast::Template>(block->getOwner());
  if (tmplt == 0) return false;
  auto def = ti_cast<Core::Data::Ast::Definition>(tmplt->getOwner());
  if (def == 0) return false;
  if (def->getOwner() == 0 || def->getOwner()->getOwner() != 0) return false;
  name = def->getName().get();
  return true;
}

} } // namespace
