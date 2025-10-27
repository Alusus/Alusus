/**
 * @file Spp/CodeGen/GlobalItemRepo.h
 * Contains the header of class Spp::CodeGen::GlobalItemRepo.
 *
 * @copyright Copyright (C) 2025 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
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
    Word size;
    void *ptr;
    Int state;
    Entry(): size(0), ptr(0), state(0) {}
    Entry(Word s, void *p): size(s), ptr(p), state(0) {}
  };


  //============================================================================
  // Member Variables

  private: Srl::Map<Str, Entry> map;


  //============================================================================
  // Constructor

  public: GlobalItemRepo() : map(true)
  {
  }

  public: ~GlobalItemRepo();


  //============================================================================
  // Member Functions

  public: void addItem(Char const *name, Word size, void *ptr = 0);
  public: void addItem(Char const *name, void *ptr);

  public: Word getItemCount() const
  {
    return this->map.getLength();
  }

  public: Str const& getItemName(Int i) const;

  public: Word getItemSize(Int i) const;

  public: void* getItemPtr(Int i) const;

  public: void setItemState(Int i, Int s);

  public: Int getItemState(Int i) const;

  public: Int findItem(Char const *name) const
  {
    return this->map.findPos(Str(true, name));
  }

}; // class

} // namespace

#endif
