/**
 * @file Spp/LibraryGateway.h
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
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

  private: SeekerExtension *seekerExtension = 0;
  private: LlvmCodeGen::Generator *llvmGenerator = 0;


  //============================================================================
  // Constructor

  public: LibraryGateway()
  {
  }

  public: virtual ~LibraryGateway()
  {
    if (this->llvmGenerator != 0) delete this->llvmGenerator;
  }


  //============================================================================
  // Member Functions

  public: virtual void initialize(Core::Standard::RootManager *manager);

  public: virtual void uninitialize(Core::Standard::RootManager *manager);

  private: Core::Data::SharedList* getLeadingCommandsList(Core::Data::GrammarRepository *grammarRepository);

  private: Core::Data::SharedList* getInnerCommandsList(Core::Data::GrammarRepository *grammarRepository);

  private: Core::Data::SharedList* getTildeCommandsList(Core::Data::GrammarRepository *grammarRepository);

  private: void addReferenceToCommandList(Core::Data::SharedList *cmdList, Char const *qualifier);

  private: void removeReferenceFromCommandList(Core::Data::SharedList *cmdList, Char const *qualifier);

  private: void createBuiltInTypes(Core::Standard::RootManager *manager);

  private: void removeBuiltInTypes(Core::Standard::RootManager *manager);

}; // class

} // namespace

#endif

