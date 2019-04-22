/**
 * @file Core/Data/NbList.h
 * Contains the header of class Core::Data::NbList.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
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

class NbList : public SharedListBase<TiObject, Node>, public DataHaving
{
  //============================================================================
  // Type Info

  typedef SharedListBase<TiObject, Node> _MyBase;
  TYPE_INFO(NbList, _MyBase, "Core.Data", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(DataHaving)
  ));


  //============================================================================
  // Constructors & Destructor

  public: NbList()
  {
  }

  public: NbList(std::initializer_list<SharedPtr<TiObject>> const &args)
  {
    this->add(args);
  }

  public: virtual ~NbList()
  {
    this->destruct();
  }

  public: static SharedPtr<NbList> create()
  {
    return std::make_shared<NbList>();
  }

  public: static SharedPtr<NbList> create(std::initializer_list<SharedPtr<TiObject>> const &args)
  {
    return std::make_shared<NbList>(args);
  }


  //============================================================================
  // Member Functions

  /// @name Abstract Function Implementations
  /// @{

  protected: virtual SharedPtr<TiObject> prepareForSet(
    Int index, SharedPtr<TiObject> const &obj, Bool inherited, Bool newEntry
  ) {
    return obj;
  }

  protected: virtual void finalizeSet(
    Int index, SharedPtr<TiObject> const &obj, Bool inherited, Bool newEntry
  ) {
    if (!inherited && obj != 0 && obj->isDerivedFrom<Node>()) {
      obj.s_cast_get<Node>()->setOwner(this);
    }
  }

  protected: virtual void prepareForUnset(
    Int index, SharedPtr<TiObject> const &obj, Bool inherited
  ) {
    if (!inherited && obj != 0 && obj->isDerivedFrom<Node>() && obj.s_cast_get<Node>()->getOwner() == this) {
      obj.s_cast_get<Node>()->setOwner(0);
    }
  }

  /// @}

  /// @name Inheritted Functions
  /// @{

  public: void setBase(NbList *b)
  {
    SharedListBase<TiObject, Node>::setBase(b);
  }

  public: NbList* getBase() const
  {
    return static_cast<NbList*>(this->base);
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
