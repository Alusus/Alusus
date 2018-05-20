/**
 * @file Spp/LibraryGateway.h
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LIBRARYGATEWAY_H
#define SPP_LIBRARYGATEWAY_H

namespace Spp
{

class LibraryGateway : public Core::Standard::LibraryGateway
{
  //============================================================================
  // Type Info

  TYPE_INFO(LibraryGateway, Core::Standard::LibraryGateway, "Spp", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: SeekerExtension::Overrides *seekerExtensionOverrides = 0;
  private: SharedPtr<Ast::Helper> astHelper;
  private: SharedPtr<Ast::NodePathResolver> nodePathResolver;
  private: SharedPtr<CodeGen::TypeGenerator> typeGenerator;
  private: SharedPtr<CodeGen::ExpressionGenerator> expressionGenerator;
  private: SharedPtr<CodeGen::CommandGenerator> commandGenerator;
  private: SharedPtr<CodeGen::Generator> generator;
  private: SharedPtr<CodeGen::NoOpTargetGenerator> noOpTargetGenerator;
  private: SharedPtr<LlvmCodeGen::TargetGenerator> targetGenerator;


  //============================================================================
  // Constructor

  public: LibraryGateway()
  {
  }

  public: virtual ~LibraryGateway()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void initialize(Core::Standard::RootManager *manager);

  public: virtual void uninitialize(Core::Standard::RootManager *manager);

  private: void createBuiltInTypes(Core::Standard::RootManager *manager);

  private: void removeBuiltInTypes(Core::Standard::RootManager *manager);

  private: void createBuiltInFunctions(Core::Standard::RootManager *manager);

  private: void removeBuiltInFunctions(Core::Standard::RootManager *manager);

  private: SharedPtr<Ast::Function> createBinaryFunction(
    Core::Standard::RootManager *manager, Char const *name, Char const *in1, Char const *in2, Char const *out
  );

  private: SharedPtr<Ast::Function> createUnaryFunction(
    Core::Standard::RootManager *manager, Char const *name, Char const *in, Char const *out
  );

}; // class

} // namespace

#endif

