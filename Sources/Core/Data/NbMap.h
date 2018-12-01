/**
 * @file Core/Data/NbMap.h
 * Contains the header of class Core::Data::NbMap.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_NBMAP_H
#define CORE_DATA_NBMAP_H

namespace Core::Data
{

class NbMap : public SharedMapBase<TiObject, Node>, public virtual DataHaving
{
  //============================================================================
  // Type Info

  typedef SharedMapBase<TiObject, Node> _MyBase;
  TYPE_INFO(NbMap, _MyBase, "Core.Data", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(DataHaving)
  ));


  //============================================================================
  // Constructors

  public: using SharedMapBase<TiObject, Node>::SharedMapBase;

  public: static SharedPtr<NbMap> create(Bool useIndex = false)
  {
    return std::make_shared<NbMap>(useIndex);
  }


  //============================================================================
  // Member Functions

  /// @name Abstract Function Implementations
  /// @{

  private: virtual SharedPtr<TiObject> prepareForSet(
    Char const *key, Int index, SharedPtr<TiObject> const &obj, Bool inherited, Bool newEntry
  ) {
    if (!inherited && obj != 0 && obj->isDerivedFrom<Node>()) {
      obj.s_cast_get<Node>()->setOwner(this);
    }
    return obj;
  }

  private: virtual void prepareForUnset(
    Char const *key, Int index, SharedPtr<TiObject> const &obj, Bool inherited
  ) {
    if (!inherited && obj != 0 && obj->isDerivedFrom<Node>()) {
      obj.s_cast_get<Node>()->setOwner(0);
    }
  }

  /// @}

  /// @name Inheritted Functions
  /// @{

  public: void setBase(NbMap *b)
  {
    SharedMapBase<TiObject, Node>::setBase(b);
  }

  public: NbMap* getBase() const
  {
    return static_cast<NbMap*>(this->base);
  }

  /// @}

  /// @name DataHaving Implementation
  /// @{

  /// @sa DataHaving::unsetIndexes()void
  public: virtual void unsetIndexes(Int from, Int to)
  {
    for (Word i = 0; i < this->getCount(); ++i) {
      TiObject *obj = this->get(i).get();
      if (obj != 0) Data::unsetIndexes(obj, from, to);
    }
  }

  /// @}

}; // class

} // namespace

#endif
