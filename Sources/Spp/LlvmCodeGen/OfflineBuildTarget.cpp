/**
 * @file Spp/LlvmCodeGen/OfflineBuildTarget.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::OfflineBuildTarget.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::LlvmCodeGen
{

void OfflineBuildTarget::setupBuild()
{
  BuildTarget::setupBuild();

  this->llvmGlobalCtorDtorEntryTypes = LlvmGlobalCtorDtorEntryTypes();
  this->llvmModule.reset();

  std::string error;
  auto target = llvm::TargetRegistry::lookupTarget(this->targetTriple, error);

  // Print an error and exit if we couldn't find the requested target.
  // This generally occurs if we've forgotten to initialise the
  // TargetRegistry or we have a bogus target triple.
  if (!target) {
    throw EXCEPTION(GenericException, error.c_str());
  }

  auto cpu = "generic";
  auto features = "";

  llvm::TargetOptions opt;
  auto rm = llvm::Optional<llvm::Reloc::Model>();
  this->targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);

  this->llvmDataLayout = std::make_unique<llvm::DataLayout>(this->targetMachine->createDataLayout());

  this->llvmContext = std::make_unique<llvm::LLVMContext>();
  #ifdef USE_LOGS
    if (Core::Basic::Logger::getFilter() & Spp::LogLevel::LLVMCODEGEN_DIAGNOSTIC) {
      this->llvmContext->setDiagnosticHandlerCallBack(&llvmDiagnosticCallback);
    }
  #endif
}


llvm::Module* OfflineBuildTarget::getGlobalLlvmModule()
{
  if (this->llvmModule == 0) {
    this->llvmModule = std::make_unique<llvm::Module>("AlususProgram", *this->llvmContext);
    this->llvmModule->setDataLayout(*this->llvmDataLayout);
  }
  return this->llvmModule.get();
}


void OfflineBuildTarget::addLlvmModule(std::unique_ptr<llvm::Module> module)
{
  #ifdef USE_LOGS
    if (Core::Basic::Logger::getFilter() & Spp::LogLevel::LLVMCODEGEN_IR) {
      // Dump the module to be compiled.
      outStream << S(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
      llvm::raw_os_ostream ostream(outStream);
      llvm::createPrintModulePass(ostream)->runOnModule(*(module));
      outStream << S("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    }
  #endif

  // Compile the module.
  this->llvmModule = std::move(module);
  this->llvmModule->setDataLayout(*this->llvmDataLayout);
}


Str OfflineBuildTarget::generateLlvmIr(Array<Str> const *ctorNames, Array<Str> const *dtorNames)
{
  if (this->llvmModule == 0) {
    throw EXCEPTION(GenericException, S("LLVM module is not generated yet."));
  }

  this->buildCtorOrDtorArray(ctorNames, "llvm.global_ctors");
  this->buildCtorOrDtorArray(dtorNames, "llvm.global_dtors");

  StrStream strStream;
  llvm::raw_os_ostream ostream(strStream);
  llvm::createPrintModulePass(ostream)->runOnModule(*(this->llvmModule));

  return Str(strStream.str().c_str());
}


void OfflineBuildTarget::generateObjectFile(
  Char const *filename, Array<Str> const *ctorNames, Array<Str> const *dtorNames
) {
  VALIDATE_NOT_NULL(filename);
  if (this->llvmModule == 0) {
    throw EXCEPTION(GenericException, S("LLVM module is not generated yet."));
  }

  this->buildCtorOrDtorArray(ctorNames, "llvm.global_ctors");
  this->buildCtorOrDtorArray(dtorNames, "llvm.global_dtors");

  this->llvmModule->setTargetTriple(this->targetTriple);

  std::error_code ec;
  llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::F_None);

  if (ec) {
    throw EXCEPTION(FileException, ec.message().c_str(), C('w'));
  }

  llvm::legacy::PassManager pass;
  auto fileType = llvm::CGFT_ObjectFile;

  if (this->targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
    throw EXCEPTION(GenericException, S("TheTargetMachine can't emit a file of this type"));
  }

  pass.run(*this->llvmModule);
  dest.flush();
}


void OfflineBuildTarget::buildCtorOrDtorArray(Array<Str> const *funcNames, Char const *globalVarName)
{
  if (this->llvmModule == 0) {
    throw EXCEPTION(GenericException, S("LLVM module is not generated yet."));
  }

  // Prepare needed types.
  if (this->llvmGlobalCtorDtorEntryTypes.llvmStructType == 0) {
    auto llvmVoidType = llvm::Type::getVoidTy(*this->llvmContext);
    std::vector<llvm::Type*> llvmArgTypes;
    auto llvmFuncType = llvm::FunctionType::get(llvmVoidType, llvmArgTypes, false);
    this->llvmGlobalCtorDtorEntryTypes.llvmFuncPtrType = llvmFuncType->getPointerTo();
    auto llvmIntType = llvm::Type::getIntNTy(*this->llvmContext, 32);
    this->llvmGlobalCtorDtorEntryTypes.llvmIntPtrType = llvmIntType->getPointerTo();
    this->llvmGlobalCtorDtorEntryTypes.llvmStructType = llvm::StructType::create(
      *this->llvmContext, "LlvmGlobalCtorDtor"
    );
    std::vector<llvm::Type*> structMembers({
      llvmIntType,
      this->llvmGlobalCtorDtorEntryTypes.llvmFuncPtrType,
      this->llvmGlobalCtorDtorEntryTypes.llvmIntPtrType
    });
    this->llvmGlobalCtorDtorEntryTypes.llvmStructType->setBody(structMembers);
  }

  auto llvmArrayType = llvm::ArrayType::get(
    this->llvmGlobalCtorDtorEntryTypes.llvmStructType, funcNames->getLength()
  );

  // Prepare array items.
  std::vector<llvm::Constant*> llvmArrayItems;
  for (Int i = 0; i < funcNames->getLength(); i++) {
    llvm::Function *llvmFunc = this->llvmModule->getFunction(funcNames->at(i).getBuf());
    if (!llvmFunc) {
      throw EXCEPTION(GenericException, S("Failed to find constructor function."));
    }
    std::vector<llvm::Constant*> llvmStructItems({
      llvm::ConstantInt::get(*this->llvmContext, llvm::APInt(32, 0, true)),
      llvm::ConstantExpr::getBitCast(llvmFunc, this->llvmGlobalCtorDtorEntryTypes.llvmFuncPtrType),
      llvm::ConstantPointerNull::get(this->llvmGlobalCtorDtorEntryTypes.llvmIntPtrType)
    });
    llvmArrayItems.push_back(llvm::ConstantStruct::get(
      this->llvmGlobalCtorDtorEntryTypes.llvmStructType, llvmStructItems
    ));
  }

  // Create the global variable.
  auto llvmArrayConst = llvm::ConstantArray::get(llvmArrayType, llvmArrayItems);
  new llvm::GlobalVariable(
    *(this->llvmModule.get()), llvmArrayType, true,
    llvm::GlobalValue::AppendingLinkage, llvmArrayConst, globalVarName
  );
}

} // namespace
