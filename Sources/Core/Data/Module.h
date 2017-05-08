/**
 * @file Core/Data/Module.h
 * Contains the header of class Core::Data::Module.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_MODULE_H
#define CORE_DATA_MODULE_H

namespace Core { namespace Data
{

// TODO: DOC

class Module : public Node,
               public virtual Bindings, public virtual MapContainer, public virtual IdHolder, public virtual DataOwner
{
  //============================================================================
  // Type Info

  TYPE_INFO(Module, Node, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Node, Bindings, MapContainer, IdHolder, DataOwner);


  //============================================================================
  // Member Variables

  protected: SharedMap definitions;


  //============================================================================
  // Signals & Slots

  public: SignalRelay<void, Container*, ContentChangeOp, Int> contentChangeNotifier;


  //============================================================================
  // Implementations

  IMPLEMENT_IDHOLDER(Module);

  IMPLEMENT_BINDINGS(Bindings, (id, TiWord, VALUE, setId(value), &id));


  //============================================================================
  // Constructor & Destructor

  public: Module() : definitions(true)
  {
    this->contentChangeNotifier.relay(this->definitions.contentChangeNotifier);
  }

  public: Module(const std::initializer_list<Argument<Char const*>> &args);

  public: virtual ~Module();

  public: static SharedPtr<Module> create()
  {
    return std::make_shared<Module>();
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

  /// @}

  /// @name Definitions Access Functions
  /// @{

  public: Int add(Char const *key, SharedPtr<TiObject> const &val);

  public: void insert(Int index, Char const *key, SharedPtr<TiObject> const &val);

  public: Int set(Char const *key, SharedPtr<TiObject> const &val, Bool insertIfNew=true);

  public: void set(Int index, SharedPtr<TiObject> const &val);

  public: SharedPtr<TiObject> const& getShared(Int index) const
  {
    return this->definitions.getShared(index);
  }

  public: void clear();

  public: Bool isInherited(Int idx) const
  {
    return this->definitions.isInherited(idx);
  }

  public: Int getIndex(Char const *key) const
  {
    return this->definitions.getIndex(key);
  }

  /// @}

  /// @name MapContainer Implementation
  /// @{

  public: virtual void set(Int index, TiObject *val);

  public: virtual void remove(Int index);

  public: virtual Word getCount() const
  {
    return this->definitions.getCount();
  }

  public: virtual TiObject* get(Int index) const
  {
    return this->definitions.get(index);
  }

  public: virtual Int set(Char const *key, TiObject *val);

  public: virtual void remove(Char const *key);

  public: virtual TiObject* get(Char const *key) const
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

  /// @name DataOwner Implementation
  /// @{

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    this->definitions.unsetIndexes(from, to);
  }

  /// @}

}; // class

} } // namespace

#endif
