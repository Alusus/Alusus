/**
 * @file Spp/CodeGen/StringLiteralRepo.h
 * Contains the header of class Spp::CodeGen::StringLiteralRepo.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_STRINGLITERALREPO_H
#define SPP_CODEGEN_STRINGLITERALREPO_H

namespace Spp::CodeGen
{

class StringLiteralRepo
{
  //============================================================================
  // Data Types

  private: struct Entry
  {
    SbStr str;
    Entry(Char *s) : str(s) {}
  };

  private: typedef DirectSortedIndex<Entry, SbStr, &Entry::str> Index;


  //============================================================================
  // Member Variables

  private: std::vector<Entry> entries;

  private: Index index;


  //============================================================================
  // Constructor

  public: StringLiteralRepo() : index(&entries)
  {
  }

  public: ~StringLiteralRepo();


  //============================================================================
  // Member Functions

  public: Int addString(Char const *str);
  public: Char const* getString(Int index) const;

}; // class

} // namespace

#endif
