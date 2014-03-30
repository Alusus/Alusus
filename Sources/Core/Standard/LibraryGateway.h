/**
 * @file Core/Standard/LibraryGateway.h
 * Contains the header of class Standard::LibraryGateway.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef STANDARD_LIBRARY_GATEWAY_H
#define STANDARD_LIBRARY_GATEWAY_H

namespace Core { namespace Standard
{

// TODO: DOC

class LibraryGateway : public IdentifiableObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(LibraryGateway, IdentifiableObject, "Core.Standard", "Core", "alusus.net");


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

  public: const Str& getLibraryId()
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
