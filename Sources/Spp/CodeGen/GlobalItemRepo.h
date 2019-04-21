/**
 * @file Spp/CodeGen/GlobalItemRepo.h
 * Contains the header of class Spp::CodeGen::GlobalItemRepo.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_GLOBALITEMREPO_H
#define SPP_CODEGEN_GLOBALITEMREPO_H

namespace Spp::CodeGen
{

class GlobalItemRepo
{
  //============================================================================
  // Data Types

  private: struct Entry
  {
    Str name;
    Word size;
    void *ptr;
  };

  private: typedef DirectSortedIndex<Entry, Str, &Entry::name> Index;


  //============================================================================
  // Member Variables

  private: std::vector<Entry> entries;

  private: Index index;


  //============================================================================
  // Constructor

  public: GlobalItemRepo() : index(&entries)
  {
  }

  public: ~GlobalItemRepo();


  //============================================================================
  // Member Functions

  public: void addItem(Char const *name, Word size, void *ptr = 0);
  public: void addItem(Char const *name, void *ptr);

  public: Word getItemCount() const
  {
    return this->entries.size();
  }

  public: Str const& getItemName(Int i) const;

  public: Word getItemSize(Int i) const;

  public: void* getItemPtr(Int i) const;

  public: Int findItem(Char const *name) const
  {
    return this->index.find(Str(name));
  }

}; // class

} // namespace

#endif
