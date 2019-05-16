/**
 * @file Core/Main/LibraryGateway.h
 * Contains the header of class Core::Main::LibraryGateway.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_MAIN_LIBRARYGATEWAY_H
#define CORE_MAIN_LIBRARYGATEWAY_H

namespace Core::Main
{

// TODO: DOC

class LibraryGateway : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(LibraryGateway, TiObject, "Core.Main", "Core", "alusus.org");


  //============================================================================
  // Protected Constructor

  protected: LibraryGateway()
  {
  }

  public: virtual ~LibraryGateway()
  {
  }


  //============================================================================
  // Member Functions

  // TODO: get_dependencies.

  public: Str const& getLibraryId()
  {
    return this->getMyTypeInfo()->getUniqueName();
  }

  public: virtual void initialize(RootManager *manager) = 0;

  public: virtual void initializeDuplicate(RootManager *manager) {};

  public: virtual void uninitialize(RootManager *manager) = 0;

  public: virtual void uninitializeDuplicate(RootManager *manager) {};

}; // class

} // namespace

#endif
