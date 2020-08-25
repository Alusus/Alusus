/**
 * @file Core/Data/NbMap.h
 * Contains the header of class Core::Data::NbMap.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_NBMAP_H
#define CORE_DATA_NBMAP_H

namespace Core::Data
{

class NbMap : public SharedMapBase<TiObject, Node>
{
  //============================================================================
  // Type Info

  typedef SharedMapBase<TiObject, Node> _MyBase;
  TYPE_INFO(NbMap, _MyBase, "Core.Data", "Core", "alusus.org");


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
    return newSrdObj<NbMap>(useIndex);
  }

  public: static SharedPtr<NbMap> create(std::initializer_list<Argument> const &args, Bool useIndex = false)
  {
    return newSrdObj<NbMap>(args, useIndex);
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
    if (obj != 0 && obj->isDerivedFrom<Node>()) {
      obj.s_cast_get<Node>()->setOwner(this);
    }
  }

  protected: virtual void prepareForUnset(
    Char const *key, Int index, SharedPtr<TiObject> const &obj, Bool inherited
  ) {
    if (obj != 0 && obj->isDerivedFrom<Node>() && obj.s_cast_get<Node>()->getOwner() == this) {
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

}; // class

} // namespace

#endif
