/**
 * @file Core/Basic/BindingIndexCache.h
 * Contains the header of interface Core::Basic::BindingIndexCache.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_BINDINGINDEXCACHE_H
#define CORE_BASIC_BINDINGINDEXCACHE_H

namespace Core { namespace Basic
{

class BindingIndexCache
{
  //============================================================================
  // Member Variables

  private: BindingMap *bindingMap;
  private: Char const *name;
  private: Int index = -1;


  //============================================================================
  // Slots

  private: Slot<void, BindingMapBase*, ContentChangeOp, Int> bindingChangeSlot = {
    this, &BindingIndexCache::onBindingChanged
  };


  //============================================================================
  // Constructors & Destructor

  public: BindingIndexCache(Char const *n, BindingMap *bm = 0) : name(n), bindingMap(bm)
  {
    if (this->bindingMap != 0) {
      this->bindingMap->changeNotifier.connect(this->bindingChangeSlot);
    }
  }


  //============================================================================
  // Member Functions

  public: void connect(BindingMap *bm)
  {
    this->bindingChangeSlot.disconnect();
    this->bindingMap = bm;
    this->bindingMap->changeNotifier.connect(this->bindingChangeSlot);
  }

  private: void onBindingChanged(BindingMapBase *src, ContentChangeOp op, Int i)
  {
    if (i <= this->index) this->index = -1;
  }

  public: Char const* getName() const
  {
    return this->name;
  }

  public: Int get()
  {
    if (this->index == -1) {
      this->index = this->bindingMap->findIndex(this->name);
    }
    if (this->index == -1) {
      throw EXCEPTION(GenericException, STR("Item is not found."));
    }
    return this->index;
  }

}; // class

} } // namespace


//==============================================================================
// Macros

#define _BINDING_INDEX_CACHE(name) BindingIndexCache name = { #name }
#define _BINDING_INDEX_CACHE_WITH_BM(name, bm) BindingIndexCache name = { #name, bm }

#define BINDING_INDEX_CACHE(name, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, _, _, _, _, \
               _BINDING_INDEX_CACHE_WITH_BM, \
               _BINDING_INDEX_CACHE)(name, __VA_ARGS__)

#endif
