/**
 * @file Core/Basic/SharedList.h
 * Contains the header of class Core::Basic::SharedList.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_SHAREDLIST_H
#define CORE_BASIC_SHAREDLIST_H

namespace Core { namespace Basic
{

template<class CTYPE, class PTYPE> class SharedList : public PTYPE
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(SharedList, PTYPE, "Core.Basic", "Core", "alusus.net", CTYPE, PTYPE);


  //============================================================================
  // Member Variables

  /// The vector in which the object pointers will be stored.
  private: std::vector<SharedPtr<CTYPE>> list;


  //============================================================================
  // Constructors

  public: SharedList()
  {
  }

  public: SharedList(const std::initializer_list<SharedPtr<CTYPE>> &args)
  {
    this->reserve(args.size());
    for (auto arg : args) this->add(arg);
  }

  public: virtual ~SharedList()
  {
  }

  public: static SharedPtr<SharedList> create(const std::initializer_list<SharedPtr<CTYPE>> &args)
  {
    return std::make_shared<SharedList>(args);
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Adds a list of new objects to the list.
   * This is more efficient than individually calling add() on each item
   * because it preallocates any needed memory in advance.
   */
  public: void add(const std::initializer_list<SharedPtr<CTYPE>> &objs)
  {
    if (this->list.capacity() < this->list.size() + objs.size()) this->list.reserve(this->list.size() + objs.size());
    for (auto obj : objs) {
      this->list.push_back(obj);
    }
  }

  public: Int add(SharedPtr<CTYPE> const &val)
  {
    this->list.push_back(val);
    return this->list.size()-1;
  }

  public: void insert(Int index, SharedPtr<CTYPE> const &val)
  {
    if (index < 0 || index > this->getCount()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
    }
    this->list.insert(this->list.begin()+index, val);
  }

  public: void set(Int index, SharedPtr<CTYPE> const &val)
  {
    if (static_cast<Word>(index) >= this->list.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index out of range."), index);
    } else {
      this->list[index] = val;
    }
  }

  public: void remove(Int index)
  {
    if (static_cast<Word>(index) >= this->list.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index out of range."), index);
    }
    this->list.erase(this->list.begin()+index);
  }

  public: Word getCount() const
  {
    return this->list.size();
  }

  public: SharedPtr<CTYPE> const& get(Int index) const
  {
    if (static_cast<Word>(index) >= this->list.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index out of range."), index);
    }
    return this->list[index];
  }

  public: void clear()
  {
    this->list.clear();
  }

  public: void reserve(Int size)
  {
    this->list.reserve(size);
  }

}; // class

} } // namespace

#endif
