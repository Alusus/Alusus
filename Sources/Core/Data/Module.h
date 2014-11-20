/**
 * @file Core/Data/Module.h
 * Contains the header of class Core::Data::Module.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_MODULE_H
#define DATA_MODULE_H

namespace Core { namespace Data
{

// TODO: DOC

class Module : public IdentifiableObject,
               public virtual MapSharedContainer, public virtual IdHolder, public virtual DataOwner
{
  //============================================================================
  // Type Info

  TYPE_INFO(Module, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_3(IdentifiableObject, MapSharedContainer, IdHolder, DataOwner);


  //============================================================================
  // Member Variables

  protected: SharedMap definitions;


  //============================================================================
  // Constructor & Destructor

  public: Module() : definitions(true)
  {
    this->definitions.contentChangeNotifier.connect(this, &Module::onDefinitionsContentChanged);
  }

  public: Module(const std::initializer_list<Argument<Char const*>> &args);

  public: virtual ~Module()
  {
  }

  public: static SharedPtr<Module> create(const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<Module>(args);
  }


  //============================================================================
  // Member Functions

  /// @name General Functions
  /// @{

  public: const SharedMap* getDefinitions() const
  {
    return &this->definitions;
  }

  public: SharedMap* getDefinitions()
  {
    return &this->definitions;
  }

  private: void onDefinitionsContentChanged(SharedContainer *obj, ContentChangeOp op, Int index)
  {
    this->contentChangeNotifier.emit(this, op, index);
  }

  /// @}

  /// @name Definitions Access Functions
  /// @{

  public: Int add(Char const *key, SharedPtr<IdentifiableObject> const &val)
  {
    return this->definitions.add(key, val);
  }

  public: void insert(Int index, Char const *key, SharedPtr<IdentifiableObject> const &val)
  {
    this->definitions.insert(index, key, val);
  }

  public: Int set(Char const *key, SharedPtr<IdentifiableObject> const &val, Bool insertIfNew)
  {
    return this->definitions.set(key, val, insertIfNew);
  }

  public: void clear()
  {
    this->definitions.clear();
  }

  public: Bool isInherited(Int idx) const
  {
    return this->definitions.isInherited(idx);
  }

  public: Int getIndex(Char const *key) const
  {
    return this->definitions.getIndex(key);
  }

  /// @}


  //============================================================================
  // MapSharedContainer Implementation

  /// @name MapSharedContainer Implementation
  /// @{

  public: virtual void set(Int index, SharedPtr<IdentifiableObject> const &val)
  {
    this->definitions.set(index, val);
  }

  public: virtual void remove(Int index)
  {
    this->definitions.remove(index);
  }

  public: virtual Word getCount() const
  {
    return this->definitions.getCount();
  }

  public: virtual SharedPtr<IdentifiableObject> const& get(Int index) const
  {
    return this->definitions.get(index);
  }

  public: virtual Int set(Char const *key, SharedPtr<IdentifiableObject> const &val)
  {
    return this->definitions.set(key, val);
  }

  public: virtual void remove(Char const *key)
  {
    this->definitions.remove(key);
  }

  public: virtual SharedPtr<IdentifiableObject> const& get(Char const *key) const
  {
    return this->definitions.get(key);
  }

  public: virtual const SbStr& getKey(Int index) const
  {
    return this->definitions.getKey(index);
  }

  /**
   * @brief Find the index of a specified key.
   * @return The index of the key, or -1 if the key doesn't exist.
   */
  public: virtual Int findIndex(Char const *key) const
  {
    return this->definitions.findIndex(key);
  }

  /// @}


  //============================================================================
  // DataOwner Implementation

  /// @name DataOwner Implementation
  /// @{

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

  /// @}

}; // class

} } // namespace

#endif
