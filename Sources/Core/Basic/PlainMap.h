/**
 * @file Core/Basic/PlainMap.h
 * Contains the header of class Core::Basic::PlainMap.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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
  TEMPLATE_TYPE_INFO(PlainMap, _MyBase, "Core.Basic", "Core", "alusus.net", (CTYPE));


  //============================================================================
  // Constructors & Destructor

  public: using PlainMapBase<CTYPE, TiObject>::PlainMapBase;

  public: static SharedPtr<PlainMap<CTYPE>> create(
    Bool useIndex, const std::initializer_list<Argument> &args)
  {
    return std::make_shared<PlainMap<CTYPE>>(useIndex, args);
  }


  //============================================================================
  // Member Functions

  /// @name Abstract Implementations
  /// @{

  private: virtual CTYPE* prepareForSet(
    Char const *key, Int index, CTYPE *obj, Bool inherited, Bool newEntry
  ) {
    return obj;
  }

  private: virtual void prepareForUnset(
    Char const *key, Int index, CTYPE *obj, Bool inherited
  ) {
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
