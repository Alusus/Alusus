/**
 * @file Spp/DependencyList.h
 * Contains the header of class Spp::DependencyList.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_DEPENDENCYLIST_H
#define SPP_DEPENDENCYLIST_H

namespace Spp
{

template<class CTYPE> class DependencyList : public PlainList<CTYPE>
{
  //============================================================================
  // Constructor & Destructor

  public: using PlainList<CTYPE>::PlainList;


  //============================================================================
  // Member Functions

  public: void add(CTYPE *f)
  {
    for (Int i = 0; i < this->getCount(); ++i) if (this->get(i) == f) return;
    PlainList<CTYPE>::add(f);
  }

}; // class

} // namespace

#endif
