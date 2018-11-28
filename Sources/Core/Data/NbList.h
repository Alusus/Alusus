/**
 * @file Core/Data/NbList.h
 * Contains the header of class Core::Data::NbList.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_NBLIST_H
#define CORE_DATA_NBLIST_H

namespace Core::Data
{

class NbList : public SharedListBase<TiObject, Node>, public virtual DataHaving
{
  //============================================================================
  // Type Info

  typedef SharedListBase<TiObject, Node> _MyBase;
  TYPE_INFO(NbList, _MyBase, "Core.Data", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(DataHaving)
  ));


  //============================================================================
  // Constructors

  public: using SharedListBase<TiObject, Node>::SharedListBase;

  public: static SharedPtr<NbList> create()
  {
    return std::make_shared<NbList>();
  }

  public: static SharedPtr<NbList> create(const std::initializer_list<SharedPtr<TiObject>> &args)
  {
    return std::make_shared<NbList>(args);
  }


  //============================================================================
  // Member Functions

  /// @name Abstract Function Implementations
  /// @{

  private: virtual SharedPtr<TiObject> prepareForSet(
    Int index, SharedPtr<TiObject> const &obj, Bool inherited, Bool newEntry
  ) {
    if (!inherited && obj != 0 && obj->isDerivedFrom<Node>()) {
      obj.s_cast_get<Node>()->setOwner(this);
    }
    return obj;
  }

  private: virtual void prepareForUnset(
    Int index, SharedPtr<TiObject> const &obj, Bool inherited
  ) {
    if (!inherited && obj != 0 && obj->isDerivedFrom<Node>()) {
      obj.s_cast_get<Node>()->setOwner(0);
    }
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
