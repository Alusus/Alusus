/**
 * @file Spp/LibraryGateway.h
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LIBRARYGATEWAY_H
#define SPP_LIBRARYGATEWAY_H

namespace Spp
{

class LibraryGateway : public Core::Main::LibraryGateway
{
  //============================================================================
  // Type Info

  TYPE_INFO(LibraryGateway, Core::Main::LibraryGateway, "Spp", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: SeekerExtension::Overrides *seekerExtensionOverrides = 0;
  private: RootScopeHandlerExtension::Overrides *rootScopeHandlerExtensionOverrides = 0;
  private: RootManagerExtension::Overrides *rootManagerExtensionOverrides = 0;
  private: SharedPtr<Ast::Helper> astHelper;
  private: SharedPtr<Ast::NodePathResolver> nodePathResolver;
  private: SharedPtr<CodeGen::GlobalItemRepo> globalItemRepo;
  private: SharedPtr<CodeGen::StringLiteralRepo> stringLiteralRepo;
  private: SharedPtr<SharedList<TiObject>> astLiteralRepo;
  private: SharedPtr<CodeGen::AstProcessor> astProcessor;
  private: SharedPtr<CodeGen::TypeGenerator> typeGenerator;
  private: SharedPtr<CodeGen::ExpressionGenerator> expressionGenerator;
  private: SharedPtr<CodeGen::CommandGenerator> commandGenerator;
  private: SharedPtr<CodeGen::Generator> generator;
  private: SharedPtr<LlvmCodeGen::TargetGenerator> rootExecTargetGenerator;
  private: SharedPtr<LlvmCodeGen::TargetGenerator> outputTargetGenerator;
  private: SharedPtr<LlvmCodeGen::TargetGenerator> astProcessingTargetGenerator;
  private: SharedPtr<BuildManager> rootExecBuildManager;
  private: SharedPtr<BuildManager> outputBuildManager;
  private: SharedPtr<BuildManager> astProcessingBuildManager;
  private: SharedPtr<Rt::AstMgr> rtAstMgr;
  private: SharedPtr<Rt::BuildMgr> rtBuildMgr;


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

  public: virtual void initialize(Core::Main::RootManager *manager);
  public: virtual void uninitialize(Core::Main::RootManager *manager);

  private: void createBuiltInTypes(Core::Main::RootManager *manager);
  private: void removeBuiltInTypes(Core::Main::RootManager *manager);

  private: void createGlobalDefs(Core::Main::RootManager *manager);
  private: void removeGlobalDefs(Core::Main::RootManager *manager);

  private: void initializeGlobalItemRepo(Core::Main::RootManager *manager);

}; // class

} // namespace

#endif

