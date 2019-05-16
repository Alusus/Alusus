/**
 * @file Core/Main/LibraryManager.h
 * Contains the header of class Core::Main::LibraryManager.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_MAIN_LIBRARYMANAGER_H
#define CORE_MAIN_LIBRARYMANAGER_H

namespace Core::Main
{

// TODO: DOC

class LibraryManager : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(LibraryManager, TiObject, "Core.Main", "Core", "alusus.org");


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

  public: PtrWord findLibrary(Char const *libId);

  public: LibraryGateway* getGateway(PtrWord id);

  public: LibraryGateway* getGateway(Char const *libId);

  public: PtrWord load(Char const *path, Str &error);

  public: void unload(PtrWord id);

  public: void unloadAll();

}; // class

} // namespace

#endif
