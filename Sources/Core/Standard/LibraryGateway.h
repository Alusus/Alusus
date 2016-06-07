/**
 * @file Core/Standard/LibraryGateway.h
 * Contains the header of class Core::Standard::LibraryGateway.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_STANDARD_LIBRARY_GATEWAY_H
#define CORE_STANDARD_LIBRARY_GATEWAY_H

namespace Core { namespace Standard
{

// TODO: DOC

class LibraryGateway : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(LibraryGateway, TiObject, "Core.Standard", "Core", "alusus.net");


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

} } // namespace

#endif
