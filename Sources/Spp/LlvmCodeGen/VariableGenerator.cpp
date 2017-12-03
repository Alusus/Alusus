/**
 * @file Spp/LlvmCodeGen/VariableGenerator.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::VariableGenerator.
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
// Initialization Functions

void VariableGenerator::initBindingCaches()
{
  Core::Basic::initBindingCaches(this, {
    &this->generateVarDefinition,
    &this->generateVarAccess,
    &this->generateVarReference,
    &this->generateMemberVarAccess
  });
}


void VariableGenerator::initBindings()
{
  this->generateVarDefinition = &VariableGenerator::_generateVarDefinition;
  this->generateVarAccess = &VariableGenerator::_generateVarAccess;
  this->generateVarReference = &VariableGenerator::_generateVarReference;
  this->generateMemberVarAccess = &VariableGenerator::_generateMemberVarAccess;
}


//==============================================================================
// Code Generation Functions

Bool VariableGenerator::_generateVarDefinition(TiObject *self, Core::Data::Ast::Definition *definition)
{
  PREPARE_SELF(varGenerator, VariableGenerator);

  TiObject *astVar = definition->getTarget().get();
  Core::Data::Ast::Metadata *metadata = ti_cast<Core::Data::Ast::Metadata>(astVar);
  if (!metadata) {
    throw EXCEPTION(GenericException, STR("Invalid variable definition. Target AST object is of invalid type."));
  }
  auto existingCgVar = metadata->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Variable>();

  if (existingCgVar == 0) {
    // Generate the type of the variable.
    Ast::Type *astType;
    llvm::Type *llvmType;
    if (!varGenerator->generator->getTypeGenerator()->getGeneratedLlvmType(astVar, llvmType, &astType)) {
      return false;
    }

    SharedPtr<Variable> cgVar = std::make_shared<Variable>();
    cgVar->setAstType(astType);

    auto astBlock = Core::Data::findOwner<Ast::Block>(definition);
    if (ti_cast<Ast::Module>(astBlock) != 0) {
      // Generate a global variable.
      // Generate global name.
      Str name = std::regex_replace(
        varGenerator->generator->getNodePathResolver()->doResolve(definition), std::regex("[^a-zA-Z0-9_]"), STR("_")
      );
      // Generate the default value.
      llvm::Constant *defaultVal;
      if (!varGenerator->generator->getTypeGenerator()->generateDefaultValue(astType, defaultVal)) return false;
      // Create the llvm global var.
      cgVar->setLlvmGlobalVariable(new llvm::GlobalVariable(
        *(varGenerator->generator->getLlvmModule().get()), llvmType, false, llvm::GlobalVariable::ExternalLinkage,
        defaultVal, name.c_str()
      ));
    } else {
      if (ti_cast<Ast::Type>(astBlock->getOwner()) != 0) {
        // This is a type member, so we don't need to generate anything.
      } else {
        // Generate a local variable.
        // At this point we should already have an llvm ir builder.
        auto cgBlock = astBlock->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Block>();
        if (cgBlock == 0) {
          throw EXCEPTION(GenericException, STR("Generated block object not found."));
        }
        ASSERT(cgBlock->getIrBuilder() != 0);
        cgVar->setLlvmAllocaInst(cgBlock->getIrBuilder()->CreateAlloca(llvmType, 0, definition->getName().get()));
      }
    }

    // Assign the result to definition target.
    metadata->setExtra(META_EXTRA_NAME, cgVar);
  }

  return true;
}


Bool VariableGenerator::_generateVarAccess(
  TiObject *self, Core::Data::Ast::Identifier *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
) {
  PREPARE_SELF(varGenerator, VariableGenerator);
  resultType = 0;
  llvmResult = 0;
  lastProcessedNode = 0;
  Bool result = false;
  Bool symbolFound = false;
  varGenerator->generator->getSeeker()->doForeach(astNode, astNode->getOwner(),
    [=, &result, &lastProcessedNode, &llvmResult, &resultType, &symbolFound]
    (TiObject *obj, Core::Data::Notice*)->Core::Data::Seeker::Verb
    {
      symbolFound = true;
      // Check if the found obj is a variable definition.
      if (Ast::isVarDefinition(obj)) {
        // Is it already generated?
        auto metadata = ti_cast<Core::Data::Ast::Metadata>(obj);
        auto cgVar = metadata->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Variable>();
        if (cgVar == 0) {
          // Generate the variable definition.
          // At this point the variable shouldn't be a function argument.
          auto varDef = ti_cast<Core::Data::Ast::Definition>(static_cast<Core::Data::Node*>(obj)->getOwner());
          if (varDef == 0) {
            throw EXCEPTION(GenericException, STR("Unexpected error while looking for variable definition."));
          }
          if (!varGenerator->generateVarDefinition(varDef)) return Core::Data::Seeker::Verb::STOP;

          cgVar = metadata->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Variable>();
          if (cgVar == 0) {
            throw EXCEPTION(GenericException, STR("Variable definition was not generated correctly."));
          }
        }
        // Generate variable access.
        if (cgVar->getLlvmAllocaInst() != 0) {
          llvmResult = llvmIrBuilder->CreateLoad(cgVar->getLlvmAllocaInst());
        } else if (cgVar->getLlvmGlobalVariable() != 0) {
          llvmResult = llvmIrBuilder->CreateLoad(cgVar->getLlvmGlobalVariable());
        } else {
          throw EXCEPTION(GenericException, STR("Variable definition was not generated correctly."));
        }
        // Set return values.
        resultType = cgVar->getAstType();
        lastProcessedNode = astNode;
        result = true;
      } else if (obj->isDerivedFrom<Ast::Function>()) {
        result = true;
      } else {
        varGenerator->generator->getNoticeStore()->add(
          std::make_shared<InvalidReferenceNotice>(astNode->findSourceLocation())
        );
      }
      return Core::Data::Seeker::Verb::STOP;
    }
  );
  if (!symbolFound) {
    varGenerator->generator->getNoticeStore()->add(
      std::make_shared<UnknownSymbolNotice>(astNode->findSourceLocation())
    );
  }
  return result;
}


Bool VariableGenerator::_generateVarReference(
  TiObject *self, Core::Data::Ast::Identifier *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
) {
  PREPARE_SELF(varGenerator, VariableGenerator);

  resultType = 0;
  llvmResult = 0;
  lastProcessedNode = 0;
  Bool result = false;
  Bool symbolFound = false;
  varGenerator->generator->getSeeker()->doForeach(astNode, astNode->getOwner(),
    [=, &result, &lastProcessedNode, &llvmResult, &resultType, &symbolFound]
    (TiObject *obj, Core::Data::Notice*)->Core::Data::Seeker::Verb
    {
      symbolFound = true;
      // Check if the found obj is a variable definition.
      if (Ast::isVarDefinition(obj)) {
        // Is variable already generated?.
        auto metadata = ti_cast<Core::Data::Ast::Metadata>(obj);
        auto cgVar = metadata->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Variable>();
        if (cgVar == 0) {
          // Generate the variable definition.
          // At this point the variable should not be a function argument.
          auto varDef = ti_cast<Core::Data::Ast::Definition>(static_cast<Core::Data::Node*>(obj)->getOwner());
          if (varDef == 0) {
            throw EXCEPTION(GenericException, STR("Unexpected error while looking for variable definition."));
          }
          if (!varGenerator->generateVarDefinition(varDef)) return Core::Data::Seeker::Verb::STOP;

          cgVar = metadata->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Variable>();
          if (cgVar == 0) {
            throw EXCEPTION(GenericException, STR("Variable definition was not generated correctly."));
          }
        }
        // Generate variable reference.
        if (cgVar->getLlvmAllocaInst() != 0) {
          llvmResult = cgVar->getLlvmAllocaInst();
        } else if (cgVar->getLlvmGlobalVariable() != 0) {
          llvmResult = cgVar->getLlvmGlobalVariable();
        } else {
          throw EXCEPTION(GenericException, STR("Variable definition was not generated correctly."));
        }
        // Set return values.
        resultType = cgVar->getAstType();
        lastProcessedNode = astNode;
        result = true;
      } else {
        varGenerator->generator->getNoticeStore()->add(
          std::make_shared<InvalidReferenceNotice>(astNode->findSourceLocation())
        );
      }
      return Core::Data::Seeker::Verb::STOP;
    }
  );
  if (!symbolFound) {
    varGenerator->generator->getNoticeStore()->add(
      std::make_shared<UnknownSymbolNotice>(astNode->findSourceLocation())
    );
  }
  return result;
}


Bool VariableGenerator::_generateMemberVarAccess(
  TiObject *self, Core::Data::Ast::Identifier *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
) {
  PREPARE_SELF(varGenerator, VariableGenerator);

  throw EXCEPTION(GenericException, STR("Not implemented yet."));
}

} } // namespace
