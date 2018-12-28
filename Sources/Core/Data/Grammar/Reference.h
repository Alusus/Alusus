/**
 * @file Core/Data/Grammar/Reference.h
 * Contains the header of class Core::Data::Grammar::Reference.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_REFERENCE_H
#define CORE_DATA_GRAMMAR_REFERENCE_H

namespace Core::Data::Grammar
{

class Reference : public Node, public virtual DataHaving
{
  //============================================================================
  // Type Info

  TYPE_INFO(Reference, Node, "Core.Data.Grammar", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(Node, DataHaving);


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> next;
  private: Str key;
  private: mutable Int cachedIndex;


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

  public: void setKey(Char const *k, Int s)
  {
    this->key.assign(k, s);
    this->cachedIndex = -1;
  }

  public: Str const& getKey() const
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
