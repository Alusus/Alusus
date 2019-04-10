/**
 * @file Core/Notices/L18nDictionary.h
 * Contains the header of class Core::Notices::L18nDictionary.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_NOTICES_L18NDICTIONARY_H
#define CORE_NOTICES_L18NDICTIONARY_H

namespace Core::Notices
{

class L18nDictionary
{
  //============================================================================
  // Member Variables

  private: SharedMap<TiStr> dictionary;


  //============================================================================
  // Constructor

  /// Prevent the singleton class from being inistantiated.
  private: L18nDictionary() : dictionary(true)
  {
  }


  //============================================================================
  // Member Functions

  public: void initialize(Char const *locale, Char const *l18nPath = 0);

  public: Char const* get(Char const *key) const;

  public: Char const* get(Char const *key, Char const *defaultValue) const
  {
    auto translatedVal = this->get(key);
    return translatedVal != 0 ? translatedVal : defaultValue;
  }

  /// Get the singleton object.
  public: static L18nDictionary* getSingleton();

}; // class

} // namespace

#endif
