/**
 * @file Core/Notices/L18nDictionary.cpp
 * Contains the implementation of class Core::Notices::L18nDictionary.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
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
  std::string filename;
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
        this->dictionary.add(key.c_str(), value);
      }
    }
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
