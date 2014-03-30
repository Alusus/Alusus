/**
 * @file Core/Data/IdGenerator.cpp
 * Contains the implementation of class Data::IdGenerator.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data
{

//==============================================================================
// Member Function

Word IdGenerator::getId(const Char *desc)
{
  Int id = this->index.find(Str(desc));
  if (id == -1) {
    ReferenceParser::validateQualifier(desc, true);
    this->ids.push_back(Desc());
    this->ids.back().str = desc;
    this->index.add();
    return this->ids.size()-1;
  } else {
    return static_cast<Word>(id);
  }
}

const Str& IdGenerator::getDesc(Word id) const
{
  if (id >= this->ids.size()) {
    throw InvalidArgumentException(STR("id"), STR("Core::Data::IdGenerator::getDesc"));
  }
  return this->ids[id].str;
}

/// Get the singleton object.
IdGenerator* IdGenerator::getSingleton()
{
  // This can be a static object rather than pointer because it doesn't depend on anything
  // else in initialization.
  static IdGenerator *idGenerator=0;
  if (idGenerator == 0) {
    idGenerator = reinterpret_cast<IdGenerator*>(GLOBAL_STORAGE->getObject(STR("Core::Data::IdGenerator")));
    if (idGenerator == 0) {
      idGenerator = new IdGenerator;
      GLOBAL_STORAGE->setObject(STR("Core::Data::IdGenerator"), reinterpret_cast<void*>(idGenerator));
    }
  }
  return idGenerator;
}

} } // namespace
