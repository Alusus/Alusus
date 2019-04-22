/**
 * @file Core/Data/NbMap.h
 * Contains the header of class Core::Data::NbMap.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
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

class NbMap : public SharedMapBase<TiObject, Node>, public DataHaving
{
  //============================================================================
  // Type Info

  typedef SharedMapBase<TiObject, Node> _MyBase;
  TYPE_INFO(NbMap, _MyBase, "Core.Data", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(DataHaving)
  ));


  //============================================================================
  // Constructors

  public: NbMap(Bool useIndex = false) : _MyBase(useIndex)
  {
  }

  public: NbMap(std::initializer_list<Argument> const &args, Bool useIndex = false) : _MyBase(useIndex)
  {
    this->add(args);
  }

  public: virtual ~NbMap()
  {
    this->destruct();
  }

  public: static SharedPtr<NbMap> create(Bool useIndex = false)
  {
    return std::make_shared<NbMap>(useIndex);
  }

  public: static SharedPtr<NbMap> create(std::initializer_list<Argument> const &args, Bool useIndex = false)
  {
    return std::make_shared<NbMap>(args, useIndex);
  }


  //============================================================================
  // Member Functions

  /// @name Abstract Function Implementations
  /// @{

  protected: virtual SharedPtr<TiObject> prepareForSet(
    Char const *key, Int index, SharedPtr<TiObject> const &obj, Bool inherited, Bool newEntry
  ) {
    return obj;
  }

  protected: virtual void finalizeSet(
    Char const *key, Int index, SharedPtr<TiObject> const &obj, Bool inherited, Bool newEntry
  ) {
    if (!inherited && obj != 0 && obj->isDerivedFrom<Node>()) {
      obj.s_cast_get<Node>()->setOwner(this);
    }
  }

  protected: virtual void prepareForUnset(
    Char const *key, Int index, SharedPtr<TiObject> const &obj, Bool inherited
  ) {
    if (!inherited && obj != 0 && obj->isDerivedFrom<Node>() && obj.s_cast_get<Node>()->getOwner() == this) {
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
