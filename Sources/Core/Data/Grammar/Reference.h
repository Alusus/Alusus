/**
 * @file Core/Data/Grammar/Reference.h
 * Contains the header of class Core::Data::Grammar::Reference.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
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

class Reference : public Node, public Binding, public CacheHaving
{
  //============================================================================
  // Type Info

  TYPE_INFO(Reference, Node, "Core.Data.Grammar", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(Binding, CacheHaving)
  ));
  OBJECT_FACTORY(Reference);


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> next;
  private: TiStr key;
  private: TiBool valueCacheEnabled = false;
  private: mutable Int cachedIndex = -1;
  private: mutable TiObject *cachedValue = 0;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (next, Reference, SHARED_REF, setNext(value), next.get()),
    (key, TiStr, VALUE, setKey(value), &this->key),
    (valueCacheEnabled, TiBool, VALUE, setValueCacheEnabled(value), &this->valueCacheEnabled)
  );


  //============================================================================
  // Constructor

  public: Reference()
  {
  }

  public: Reference(Char const *k) : key(k)
  {
  }

  public: Reference(Char const *k, Int s) : key(k, s)
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

  public: void setValueCacheEnabled(Bool e)
  {
    this->valueCacheEnabled = e;
  }
  public: void setValueCacheEnabled(TiBool const *e)
  {
    this->valueCacheEnabled = e == 0 ? false : e->get();
  }

  public: Bool isValueCacheEnabled() const
  {
    return this->valueCacheEnabled;
  }

  public: void setCachedValue(TiObject *val) const
  {
    this->cachedValue = val;
  }

  public: TiObject* getCachedValue() const
  {
    return this->cachedValue;
  }

  public: Bool setValue(TiObject *source, TiObject *value) const;

  public: Bool removeValue(TiObject *source) const;

  public: Bool getValue(TiObject *source, TiObject *&value) const;

  private: Bool _getValue(TiObject *source, TiObject *&value) const;

  public: Bool isEqual(Reference *ref);

  /// @sa CacheHaving::clearCache()
  public: virtual void clearCache()
  {
    this->cachedIndex = -1;
    this->cachedValue = 0;
  }

}; // class

} // namespace

#endif
