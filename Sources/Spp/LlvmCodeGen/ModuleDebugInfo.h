/**
 * @file Spp/LlvmCodeGen/ModuleDebugInfo.h
 * Contains the header of class Spp::LlvmCodeGen::ModuleDebugInfo.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_MODULEDEBUGINFO_H
#define SPP_LLVMCODEGEN_MODULEDEBUGINFO_H

namespace Spp::LlvmCodeGen
{

class Type;
class FunctionType;

/**
 * @brief Holds the debug info (DWARF) state of a single LLVM module.
 *
 * Debug metadata can't be shared between modules, so every module that needs debug info gets its own instance of
 * this class. The instance must not outlive the module it was created for, and finalize() must be called once all
 * debug info has been added and before the module is emitted or handed to the JIT.
 */
class ModuleDebugInfo
{
  //============================================================================
  // Member Variables

  private: llvm::Module *llvmModule;
  private: std::unique_ptr<llvm::DIBuilder> diBuilder;
  private: llvm::DICompileUnit *compileUnit = 0;
  private: Bool finalized = false;
  private: Bool optimized;
  private: std::map<std::string, llvm::DIFile*> files;
  private: std::map<Type const*, llvm::DIType*> types;


  //============================================================================
  // Constructors & Destructor

  public: ModuleDebugInfo(llvm::Module *module, Char const *targetTriple, Bool optimized);


  //============================================================================
  // Member Functions

  public: llvm::DIBuilder* getDiBuilder() const
  {
    return this->diBuilder.get();
  }

  public: llvm::DICompileUnit* getCompileUnit() const
  {
    return this->compileUnit;
  }

  /**
   * @brief Find the source code location record that best describes a node.
   *
   * If the node's location is a stack (a node that was produced through macro expansion has more than one location),
   * the innermost record is used, which is the one pointing to where the node was originally written, i.e. inside
   * the macro body. Returns null if the node has no usable location.
   */
  public: static Core::Ast::SourceLocationRecord const* findLocationRecord(Core::Ast::Node const *node);

  /// Get the DIFile for the given source file name, creating it if needed.
  public: llvm::DIFile* getFile(Str const &filename);

  /**
   * @brief Get the debug info type describing a target type.
   * Returns null for void, which is how LLVM represents it in debug info.
   */
  public: llvm::DIType* getType(Type const *type);

  /**
   * @brief Create the DISubprogram for a function defined at the given location.
   *
   * No linkage name is recorded because debuggers use it in place of the name when it's present, assuming it's a
   * mangled name that needs demangling, whereas Alusus symbol names are already readable.
   */
  public: llvm::DISubprogram* createFunction(
    Char const *name, Core::Ast::SourceLocationRecord const *location, FunctionType const *functionType
  );

  /**
   * @brief Describe a local variable or an argument and tie it to its storage.
   * @param argIndex The 1-based index of the argument, or 0 if this is not an argument.
   */
  public: void declareLocalVariable(
    llvm::DISubprogram *scope, llvm::Value *storage, Char const *name, Type const *type,
    Core::Ast::SourceLocationRecord const *location, Word argIndex
  );

  /// Describe a global variable and attach the description to it.
  public: void describeGlobalVariable(
    llvm::GlobalVariable *globalVar, Char const *name, Type const *type,
    Core::Ast::SourceLocationRecord const *location
  );

  /// Resolves all pending debug metadata. Calling it more than once has no effect.
  public: void finalize();

}; // class

} // namespace

#endif
