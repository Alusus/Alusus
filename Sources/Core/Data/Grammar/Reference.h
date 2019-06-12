/**
 * @file Core/Data/Grammar/Reference.h
 * Contains the header of class Core::Data::Grammar::Reference.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_REFERENCE_H
#define CORE_DATA_GRAMMAR_REFERENCE_H

namespace Core::Data::Grammar
{

class Reference : public Node, public Binding, public DataHaving
{
  //============================================================================
  // Type Info

  TYPE_INFO(Reference, Node, "Core.Data.Grammar", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(Binding, DataHaving)
  ));
  OBJECT_FACTORY(Reference);


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> next;
  private: TiStr key;
  private: mutable Int cachedIndex;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (next, Reference, SHARED_REF, setNext(value), next.get()),
    (key, TiStr, VALUE, setKey(value), &this->key)
  );


  //============================================================================
  // Constructor

  public: Reference() : cachedIndex(-1)
  {
  }

  public: Reference(Char const *k) : key(k), cachedIndex(-1)
  {
  }

  public: Reference(Char const *k, Int s) : key(k, s), cachedIndex(-1)
  {
  }

  public: virtual ~Reference()
  {
    RESET_OWNED_SHAREDPTR(this->next);
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Set the next object in the reference chain.
   * The next object in the chain will be referencing an object within whatever
   * is referenced by this object.
   */
  public: void setNext(SharedPtr<Reference> const &n)
  {
    UPDATE_OWNED_SHAREDPTR(this->next, n);
  }
  public: void setNext(Reference *n)
  {
    this->setNext(getSharedPtr(n));
  }

  /// @sa setNext()
  public: SharedPtr<Reference> const& getNext() const
  {
    return this->next;
  }

  public: void setKey(Char const *k)
  {
    this->key = k;
    this->cachedIndex = -1;
  }
  public: void setKey(TiStr const *k)
  {
    this->key = k == 0 ? "" : k->get();
  }

  public: void setKey(Char const *k, Int s)
  {
    this->key.set(k, s);
    this->cachedIndex = -1;
  }

  public: TiStr const& getKey() const
  {
    return this->key;
  }

  public: Bool setValue(TiObject *source, TiObject *value) const;

  public: Bool removeValue(TiObject *source) const;

  public: Bool getValue(TiObject *source, TiObject *&value, Module **ownerModule = 0) const;

  private: Bool _getValue(TiObject *source, TiObject *&value, Module **ownerModule = 0) const;

  public: Bool isEqual(Reference *ref);

  /// @sa DataHaving::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    if (this->cachedIndex >= from && this->cachedIndex <= to) this->cachedIndex = -1;
  }

}; // class

} // namespace

#endif
