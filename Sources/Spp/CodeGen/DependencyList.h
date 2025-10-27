/**
 * @file Spp/CodeGen/DependencyList.h
 * Contains the header of class Spp::CodeGen::DependencyList.
 *
 * @copyright Copyright (C) 2025 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_DEPENDENCYLIST_H
#define SPP_CODEGEN_DEPENDENCYLIST_H

namespace Spp { namespace CodeGen
{

template<class CTYPE> class DependencyList : public PlainList<CTYPE>
{
  //============================================================================
  // Constructor & Destructor

  public: using PlainList<CTYPE>::PlainList;


  //============================================================================
  // Member Functions

  public: void add(CTYPE *f, Bool highPriority)
  {
    if (this->find(f) != -1) return;
    if (highPriority) PlainList<CTYPE>::insert(0, f);
    else PlainList<CTYPE>::add(f);
  }

  public: Int find(CTYPE *f) const
  {
    for (Int i = 0; i < this->getCount(); ++i) {
      if (this->get(i) == f) return i;
    }
    return -1;
  }

}; // class

} } // namespace

#endif
