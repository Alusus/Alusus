/**
 * @file Core/Standard/LibraryManager.h
 * Contains the header of class Core::Standard::LibraryManager.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef STANDARD_LIBRARY_MANAGER_H
#define STANDARD_LIBRARY_MANAGER_H

namespace Core { namespace Standard
{

// TODO: DOC

class LibraryManager : public IdentifiableObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(LibraryManager, IdentifiableObject, "Core.Standard", "Core", "alusus.net");


  //============================================================================
  // Types

  private: struct Entry
  {
    PtrWord id;
    LibraryGateway *gateway;
    Int refCount;

    Entry(PtrWord i, LibraryGateway *g) : id(i), gateway(g), refCount(1) {}
  };


  //============================================================================
  // Member Variables

  private: std::vector<Entry> entries;
  private: RootManager *root;


  //============================================================================
  // Constructor

  public: LibraryManager(RootManager *r) : root(r)
  {
  }

  public: virtual ~LibraryManager()
  {
    this->unloadAll();
  }


  //============================================================================
  // Member Functions

  public: void addLibrary(PtrWord id, LibraryGateway *gateway);

  public: void removeLibrary(PtrWord id);

  public: PtrWord findLibrary(const Char *libId);

  public: LibraryGateway* getGateway(PtrWord id);

  public: LibraryGateway* getGateway(const Char *libId);

  public: PtrWord load(const Char *path);

  public: void unload(PtrWord id);

  public: void unloadAll();

}; // class

} } // namespace

#endif
