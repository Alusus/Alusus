/**
 * @file Core/Notices/L18nDictionary.cpp
 * Contains the implementation of class Core::Notices::L18nDictionary.
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Notices
{

//==============================================================================
// Member Function

void L18nDictionary::initialize(Char const *locale, Char const *l18nPath)
{
  this->dictionary.clear();
  this->currentLocale = locale;
  Srl::String filename;
  if (l18nPath != 0) {
    filename = l18nPath;
  } else {
    filename = Main::getModuleDirectory();
    filename += S("../Notices_L18n/");
  }
  filename += locale;
  filename += ".txt";
  std::ifstream fin(filename);
  if (!fin.fail()) {
    while (!fin.eof()) {
      std::string line;
      std::getline(fin, line);
      Int pos = line.find(C(':'));
      if (pos != -1) {
        Str key(line.c_str(), pos);
        auto value = TiStr::create(line.c_str() + pos + 1);
        this->dictionary.add(key, value);
      }
    }
  }
}


void L18nDictionary::addEntry(Char const *locale, Char const *key, Char const *value)
{
  auto index = this->dictionary.findIndex(key);
  if (index == -1) {
    // If we don't already have an entry, then use the provided one even if it's not the correct locale.
    this->dictionary.add(Str(key), TiStr::create(value));
  } else if (this->currentLocale == locale) {
    // We already have an entry, but the new entry is of the current locale, so we'll overwrite what we have, which is
    // likely of a different locale, with the new value.
    this->dictionary.set(index, TiStr::create(value));
  }
}


Char const* L18nDictionary::get(Char const *key) const
{
  auto index = this->dictionary.findIndex(key);
  if (index == -1) return 0;
  else return this->dictionary.get(index)->get();
}


L18nDictionary* L18nDictionary::getSingleton()
{
  static L18nDictionary *l18nDictionary=0;
  if (l18nDictionary == 0) {
    l18nDictionary = reinterpret_cast<L18nDictionary*>(GLOBAL_STORAGE->getObject(S("Core::Notices::L18nDictionary")));
    if (l18nDictionary == 0) {
      l18nDictionary = new L18nDictionary;
      GLOBAL_STORAGE->setObject(S("Core::Notices::L18nDictionary"), reinterpret_cast<void*>(l18nDictionary));
    }
  }
  return l18nDictionary;
}

} // namespace
