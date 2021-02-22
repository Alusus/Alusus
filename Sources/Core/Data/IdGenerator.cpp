/**
 * @file Core/Data/IdGenerator.cpp
 * Contains the implementation of class Core::Data::IdGenerator.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data
{

//==============================================================================
// Member Function

Word IdGenerator::getId(Char const *desc)
{
  Int id = this->index.findPos(Str(true, desc));
  if (id == -1) {
    this->ids.add(desc);
    this->index.add(-1);
    return this->ids.getLength()-1;
  } else {
    return static_cast<Word>(id);
  }
}


Str const& IdGenerator::getDesc(Word id) const
{
  if (id >= this->ids.getLength()) {
    throw EXCEPTION(InvalidArgumentException, S("id"), S("No desc available for this id."), id);
  }
  return this->ids(id);
}


IdGenerator* IdGenerator::getSingleton()
{
  static IdGenerator *idGenerator=0;
  if (idGenerator == 0) {
    idGenerator = reinterpret_cast<IdGenerator*>(GLOBAL_STORAGE->getObject(S("Core::Data::IdGenerator")));
    if (idGenerator == 0) {
      idGenerator = new IdGenerator;
      GLOBAL_STORAGE->setObject(S("Core::Data::IdGenerator"), reinterpret_cast<void*>(idGenerator));
    }
  }
  return idGenerator;
}

} } // namespace
