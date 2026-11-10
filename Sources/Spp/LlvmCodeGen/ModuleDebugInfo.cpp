/**
 * @file Spp/LlvmCodeGen/ModuleDebugInfo.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::ModuleDebugInfo.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::LlvmCodeGen
{

ModuleDebugInfo::ModuleDebugInfo(llvm::Module *module, Char const *targetTriple, Bool optimized)
  : llvmModule(module), optimized(optimized)
{
  llvm::Triple triple(targetTriple);

  // Module flags telling LLVM which debug info format to emit.
  if (triple.isWindowsMSVCEnvironment()) {
    module->addModuleFlag(llvm::Module::Warning, "CodeView", 1);
  } else {
    module->addModuleFlag(llvm::Module::Max, "Dwarf Version", triple.isOSDarwin() ? 4 : 5);
  }
  module->addModuleFlag(llvm::Module::Warning, "Debug Info Version", llvm::DEBUG_METADATA_VERSION);

  this->diBuilder = std::make_unique<llvm::DIBuilder>(*module);

  // The compile unit requires a primary file, but an Alusus program is made of many files, so we use a placeholder
  // here and give each function and variable its own file. The placeholder is named like a C file on purpose: gdb
  // guesses the language of each source file from its extension, which it can't do for ours, and a file it can't
  // guess takes the language of the file before it, which is this one. Without that, gdb reports the language of our
  // source files as unknown, even though it evaluates expressions in the language of the compile unit.
  auto placeholderFile = this->diBuilder->createFile("<alusus>.c", ".");
  // There's no DWARF language code for Alusus, so we use C, which gives debuggers sensible expression evaluation.
  this->compileUnit = this->diBuilder->createCompileUnit(
    llvm::dwarf::DW_LANG_C, placeholderFile, "Alusus", optimized, "", 0, "", llvm::DICompileUnit::FullDebug, 0, true,
    false, llvm::DICompileUnit::DebugNameTableKind::None
  );
}


static void flattenLocation(
  Core::Ast::SourceLocation const *sl, std::vector<Core::Ast::SourceLocationRecord const*> &records
) {
  if (sl == 0) return;
  auto record = ti_cast<Core::Ast::SourceLocationRecord const>(sl);
  if (record != 0) {
    if (record->line <= 0) return;
    if (!records.empty() && *records.back() == *record) return;
    records.push_back(record);
    return;
  }
  auto stack = ti_cast<Core::Ast::SourceLocationStack const>(sl);
  if (stack != 0) {
    for (Int i = 0; i < stack->getCount(); ++i) flattenLocation(stack->get(i).get(), records);
  }
}


void ModuleDebugInfo::findLocationChain(
  Core::Ast::Node const *node, std::vector<Core::Ast::SourceLocationRecord const*> &chain
) {
  chain.clear();
  if (node != 0) flattenLocation(node->findSourceLocation().get(), chain);
}


Core::Ast::SourceLocationRecord const* ModuleDebugInfo::findLocationRecord(Core::Ast::Node const *node)
{
  std::vector<Core::Ast::SourceLocationRecord const*> chain;
  findLocationChain(node, chain);
  return chain.empty() ? 0 : chain.back();
}


llvm::DILexicalBlock* ModuleDebugInfo::createLexicalBlock(
  llvm::DIScope *parent, Core::Ast::SourceLocationRecord const *location
) {
  return this->diBuilder->createLexicalBlock(
    parent, this->getFile(location->filename), location->line, location->column
  );
}


llvm::DILocalScope* ModuleDebugInfo::getScopeForCodeFromFile(llvm::DILocalScope *scope, Str const &filename)
{
  auto file = this->getFile(filename);
  if (scope->getFile() == file) return scope;
  return this->diBuilder->createLexicalBlockFile(scope, file);
}


llvm::DISubprogram* ModuleDebugInfo::getInlinedCodeScope(Str const &filename)
{
  auto it = this->inlinedCodeScopes.find(filename.getBuf());
  if (it != this->inlinedCodeScopes.end()) return it->second;

  auto file = this->getFile(filename);
  auto type = this->diBuilder->createSubroutineType(this->diBuilder->getOrCreateTypeArray({}));
  auto flags = llvm::DISubprogram::SPFlagDefinition;
  if (this->optimized) flags |= llvm::DISubprogram::SPFlagOptimized;
  auto scope = this->diBuilder->createFunction(
    file, "<macro>", "", file, 1, type, 1, llvm::DINode::FlagZero, flags
  );
  this->inlinedCodeScopes[filename.getBuf()] = scope;
  return scope;
}


Int ModuleDebugInfo::findLastLine(Core::Ast::Node const *node, Str const &filename)
{
  Int lastLine = 0;
  if (node == 0) return lastLine;

  std::vector<Core::Ast::SourceLocationRecord const*> chain;
  flattenLocation(node->getSourceLocation().get(), chain);
  if (!chain.empty() && chain.front()->filename == filename) lastLine = chain.front()->line;

  auto container = node->getInterface<Containing<Core::Ast::Node> const>();
  if (container != 0) {
    for (Int i = 0; i < container->getElementCount(); ++i) {
      // Only the children the node owns are part of its code. The rest are references to other parts of the tree,
      // which can lead back to this node.
      if (container->getElementHoldMode(i) != HoldMode::SHARED_REF) continue;
      lastLine = std::max(lastLine, findLastLine(container->getElement(i), filename));
    }
  }
  return lastLine;
}


llvm::DIFile* ModuleDebugInfo::getFile(Str const &filename)
{
  auto it = this->files.find(filename.getBuf());
  if (it != this->files.end()) return it->second;

  llvm::DIFile *file;
  if (Core::Notices::getSourceLocationPathSkipping()) {
    // The tests ask for file paths to be left out of the output so that it doesn't depend on where the tests are.
    file = this->diBuilder->createFile(std::filesystem::path(filename.getBuf()).filename().string(), "");
  } else {
    // Debuggers need absolute paths to find the sources reliably.
    std::error_code ec;
    std::filesystem::path path = std::filesystem::absolute(std::filesystem::path(filename.getBuf()), ec);
    if (ec) path = filename.getBuf();
    path = path.lexically_normal();
    file = this->diBuilder->createFile(path.filename().string(), path.parent_path().string());
  }
  this->files[filename.getBuf()] = file;
  return file;
}


llvm::DIType* ModuleDebugInfo::getType(Type const *type)
{
  if (type == 0 || type->isDerivedFrom<VoidType>()) return 0;

  auto it = this->types.find(type);
  if (it != this->types.end()) return it->second;

  // The cache is keyed by address, so the type has to stay alive as long as the cache does, otherwise a new type
  // allocated at the same address would be mistaken for it.
  this->retainedTypes.push_back(getSharedPtr(const_cast<Type*>(type)));

  auto &dl = this->llvmModule->getDataLayout();
  llvm::DIType *result = 0;

  if (auto intType = ti_cast<IntegerType const>(type)) {
    unsigned encoding;
    if (intType->getSize() == 1) encoding = llvm::dwarf::DW_ATE_boolean;
    else if (intType->isSigned()) encoding = llvm::dwarf::DW_ATE_signed;
    // Bytes are usually characters, so debuggers should print them as such.
    else if (intType->getSize() == 8) encoding = llvm::dwarf::DW_ATE_unsigned_char;
    else encoding = llvm::dwarf::DW_ATE_unsigned;
    std::string name = (intType->isSigned() ? "Int[" : "Word[") + std::to_string(intType->getSize()) + "]";
    result = this->diBuilder->createBasicType(name, intType->getSize(), encoding);

  } else if (auto floatType = ti_cast<FloatType const>(type)) {
    result = this->diBuilder->createBasicType(
      "Float[" + std::to_string(floatType->getSize()) + "]", floatType->getSize(), llvm::dwarf::DW_ATE_float
    );

  } else if (auto ptrType = ti_cast<PointerType const>(type)) {
    result = this->diBuilder->createPointerType(
      this->getType(ptrType->getContentType().get()), dl.getPointerSizeInBits()
    );

  } else if (auto arrayType = ti_cast<ArrayType const>(type)) {
    auto subscripts = this->diBuilder->getOrCreateArray({
      this->diBuilder->getOrCreateSubrange(0, (int64_t)arrayType->getSize())
    });
    result = this->diBuilder->createArrayType(
      dl.getTypeAllocSizeInBits(arrayType->getLlvmType()), dl.getABITypeAlign(arrayType->getLlvmType()).value() * 8,
      this->getType(arrayType->getContentType().get()), subscripts
    );

  } else if (auto structType = ti_cast<StructType const>(type)) {
    auto llvmStruct = llvm::cast<llvm::StructType>(structType->getLlvmType());
    auto name = structType->getName().getBuf();
    if (llvmStruct->isOpaque()) {
      result = this->diBuilder->createStructType(
        this->compileUnit, name, 0, 0, 0, 0, llvm::DINode::FlagFwdDecl, 0, llvm::DINodeArray()
      );
    } else {
      // A struct can refer to itself through pointers, so we register a placeholder first.
      auto placeholder = this->diBuilder->createReplaceableCompositeType(
        llvm::dwarf::DW_TAG_structure_type, name, this->compileUnit, 0, 0
      );
      this->types[type] = placeholder;

      auto layout = dl.getStructLayout(llvmStruct);
      std::vector<llvm::Metadata*> members;
      auto &memberNames = structType->getMemberNames();
      auto &memberTypes = structType->getMemberTypes();
      for (size_t i = 0; i < memberTypes.size() && i < llvmStruct->getNumElements(); ++i) {
        auto llvmMemberType = llvmStruct->getElementType(i);
        members.push_back(this->diBuilder->createMemberType(
          placeholder, memberNames[i], 0, 0, dl.getTypeSizeInBits(llvmMemberType),
          dl.getABITypeAlign(llvmMemberType).value() * 8, layout->getElementOffsetInBits(i), llvm::DINode::FlagZero,
          this->getType(memberTypes[i].get())
        ));
      }
      auto composite = this->diBuilder->createStructType(
        this->compileUnit, name, 0, 0, layout->getSizeInBits(), layout->getAlignment().value() * 8,
        llvm::DINode::FlagZero, 0, this->diBuilder->getOrCreateArray(members)
      );
      this->diBuilder->replaceTemporary(llvm::TempMDNode(placeholder), composite);
      result = composite;
    }

  } else if (auto funcType = ti_cast<FunctionType const>(type)) {
    std::vector<llvm::Metadata*> types;
    types.push_back(this->getType(funcType->getRetType().get()));
    for (Int i = 0; i < funcType->getArgs()->getElementCount(); ++i) {
      types.push_back(this->getType(funcType->getArgs()->getElement(i)));
    }
    // A trailing null stands for the variadic part.
    if (funcType->isVariadic()) types.push_back(0);
    result = this->diBuilder->createSubroutineType(this->diBuilder->getOrCreateTypeArray(types));
  }

  this->types[type] = result;
  return result;
}


llvm::DISubprogram* ModuleDebugInfo::createFunction(
  Char const *name, Core::Ast::SourceLocationRecord const *location, FunctionType const *functionType
) {
  auto file = this->getFile(location->filename);
  auto type = functionType != 0 ?
    llvm::cast<llvm::DISubroutineType>(this->getType(functionType)) :
    this->diBuilder->createSubroutineType(this->diBuilder->getOrCreateTypeArray({}));

  // The function names include the signature, which is needed to tell overloads apart in the symbol name but only
  // gets in the way when typing a function name in a debugger, so we cut it off for the display name.
  std::string displayName = name;
  auto sigPos = displayName.find('(');
  if (sigPos != std::string::npos && sigPos > 0) displayName.resize(sigPos);

  auto flags = llvm::DISubprogram::SPFlagDefinition;
  if (this->optimized) flags |= llvm::DISubprogram::SPFlagOptimized;
  return this->diBuilder->createFunction(
    file, displayName, "", file, location->line, type, location->line, llvm::DINode::FlagZero, flags
  );
}


void ModuleDebugInfo::declareLocalVariable(
  llvm::DILocalScope *scope, llvm::Value *storage, Char const *name, Type const *type,
  Core::Ast::SourceLocationRecord const *location, Word argIndex
) {
  auto file = this->getFile(location->filename);
  auto diType = this->getType(type);
  llvm::DILocalVariable *variable;
  if (argIndex > 0) {
    variable = this->diBuilder->createParameterVariable(scope, name, argIndex, file, location->line, diType, true);
  } else {
    variable = this->diBuilder->createAutoVariable(scope, name, file, location->line, diType, true);
  }
  auto diLocation = llvm::DILocation::get(this->llvmModule->getContext(), location->line, location->column, scope);

  // The declaration goes right after the allocation so that it's always in the entry block.
  auto storageInst = llvm::cast<llvm::Instruction>(storage);
  auto expression = this->diBuilder->createExpression();
  if (storageInst->getNextNode() != 0) {
    this->diBuilder->insertDeclare(storage, variable, expression, diLocation, storageInst->getNextNode());
  } else {
    this->diBuilder->insertDeclare(storage, variable, expression, diLocation, storageInst->getParent());
  }
}


void ModuleDebugInfo::describeGlobalVariable(
  llvm::GlobalVariable *globalVar, Char const *name, Type const *type,
  Core::Ast::SourceLocationRecord const *location
) {
  auto file = this->getFile(location->filename);
  auto expression = this->diBuilder->createGlobalVariableExpression(
    this->compileUnit, name, "", file, location->line, this->getType(type), false
  );
  globalVar->addDebugInfo(expression);
}


void ModuleDebugInfo::finalize()
{
  if (this->finalized) return;
  this->diBuilder->finalize();
  this->finalized = true;

  // Malformed debug info can make the backend crash or emit garbage, so if it fails verification we report that and
  // drop it rather than fail the build. Other problems in the module are not our concern here: they are reported
  // separately in the process of the build.
  std::string errors;
  llvm::raw_string_ostream errorStream(errors);
  bool brokenDebugInfo = false;
  llvm::verifyModule(*this->llvmModule, &errorStream, &brokenDebugInfo);
  if (brokenDebugInfo) {
    errorStream.flush();
    outStream << S("WARNING: Invalid debug info was generated and will be dropped:\n") << errors.c_str() << S("\n");
    llvm::StripDebugInfo(*this->llvmModule);
  }
}

} // namespace
