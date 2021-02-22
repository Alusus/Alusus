/**
 * @file Core/Basic/PlainMap.h
 * Contains the header of class Core::Basic::PlainMap.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_PLAINMAP_H
#define CORE_BASIC_PLAINMAP_H

namespace Core::Basic
{

template<class CTYPE> class PlainMap : public PlainMapBase<CTYPE, TiObject>
{
  //============================================================================
  // Type Info

  typedef PlainMapBase<CTYPE, TiObject> _MyBase;
  TEMPLATE_TYPE_INFO(PlainMap, _MyBase, "Core.Basic", "Core", "alusus.org", (CTYPE));


  //============================================================================
  // Constructors & Destructor

  public: PlainMap(Bool useIndex = false) : _MyBase(useIndex)
  {
  }

  public: PlainMap(std::initializer_list<Argument> const &args, Bool useIndex = false) : _MyBase(useIndex)
  {
    this->add(args);
  }

  public: virtual ~PlainMap()
  {
    this->destruct();
  }

  public: static SharedPtr<PlainMap<CTYPE>> create(std::initializer_list<Argument> const &args, Bool useIndex = false)
  {
    return newSrdObj<PlainMap<CTYPE>>(useIndex, args);
  }


  //============================================================================
  // Member Functions

  /// @name Abstract Implementations
  /// @{

  protected: virtual CTYPE* prepareForSet(Char const *key, Int index, CTYPE *obj, Bool inherited, Bool newEntry)
  {
    return obj;
  }

  protected: virtual void finalizeSet(Char const *key, Int index, CTYPE *obj, Bool inherited, Bool newEntry)
  {
  }

  protected: virtual void prepareForUnset(Char const *key, Int index, CTYPE *obj, Bool inherited)
  {
  }

  /// @}

  /// @name Inheritted Functions
  /// @{

  public: void setBase(PlainMap<CTYPE> *b)
  {
    PlainMapBase<CTYPE, TiObject>::setBase(b);
  }

  public: PlainMap<CTYPE>* getBase() const
  {
    return static_cast<PlainMap<CTYPE>*>(this->base);
  }

  /// @}

}; // class

} // namespace

#endif
