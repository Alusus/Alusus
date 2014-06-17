/**
 * @file Core/Data/SharedNamedList.cpp
 * Contains the implementation of class Core::Data::SharedNamedList.
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
// Static Variables

const Int SharedNamedList::recordCountIncrement = 100;


//==============================================================================
// Initialization Functions

void SharedNamedList::initialize(Word maxStrSize, Word reservedCount)
{
  this->release();
  this->maxStrSize = maxStrSize;
  this->reservedCount = reservedCount;
  this->buffer = new Byte[this->reservedCount * this->getRecordSize()];
  this->count = 0;
}


void SharedNamedList::reinitialize(Word maxStrSize, Word reservedCount)
{
  if (this->buffer == 0) {
    throw GeneralException(STR("Buffer is not initialized."), STR("Core::Data::SharedNamedList::reinitialize"));
  }
  if (this->maxStrSize == maxStrSize && this->reservedCount == reservedCount) return;
  Byte *oldBuf = this->buffer;
  Word oldStrSize = this->maxStrSize;
  this->maxStrSize = maxStrSize;
  this->reservedCount = reservedCount;
  this->buffer = new Byte[this->reservedCount * this->getRecordSize()];

  if (!this->isEmpty()) {
    if (oldStrSize == this->maxStrSize) {
      memcpy(this->buffer, oldBuf, this->getRecordSize()*this->count);
    } else {
      for (Int i = 0; static_cast<Word>(i) < this->count; ++i) {
        Byte *destBuf = this->buffer + i*this->getRecordSize();
        Byte *srcBuf = oldBuf + i*SharedNamedList::getRecordSize(oldStrSize);
        sbstr_cast(destBuf).assign(sbstr_cast(srcBuf), this->maxStrSize);
        SharedPtr<IdentifiableObject> *destPtr = new(destBuf+sizeof(Char)*this->maxStrSize) SharedPtr<IdentifiableObject>();
        *destPtr = *reinterpret_cast<SharedPtr<IdentifiableObject>*>(srcBuf+sizeof(Char)*oldStrSize);
      }
    }
  }
  delete[] oldBuf;
}


void SharedNamedList::copy(const SharedNamedList *src)
{
  if (src->buffer == 0) {
    throw InvalidArgumentException(STR("src"), STR("Core::Data::SharedNamedList::copy"),
                                   STR("Stack is not initialized."));
  }

  // If we have a small buffer, we can drop it.
  if (this->buffer!=0 && (this->maxStrSize < src->maxStrSize ||
                          this->reservedCount < src->reservedCount)) {
    delete[] this->buffer;
    this->buffer = 0;
  }

  // If we don't have a buffer, we need to create one with the same size.
  if (this->buffer == 0) {
    this->maxStrSize = src->maxStrSize;
    this->reservedCount = src->reservedCount;
    this->count = 0;
    this->buffer = new Byte[this->reservedCount * this->getRecordSize()];
  }

  // Copy members one by one.
  this->clear();
  for (Int i = 0; i < src->getCount(); ++i) {
    this->add(src->getName(i).c_str(), src->get(i));
  }
}


void SharedNamedList::release()
{
  if (this->buffer != 0) delete[] this->buffer;
  this->buffer = 0;
}


void SharedNamedList::clear()
{
  if (this->buffer == 0) {
    throw GeneralException(STR("List not initialized."), STR("Core::Data::SharedNamedList::clear"));
  }
  ASSERT(this->count >= 0);
  for (Int i = 0; i < this->count; ++i) {
    Byte *buf = this->buffer + i*this->getRecordSize();
    reinterpret_cast<SharedPtr<IdentifiableObject>*>(buf+sizeof(Char)*this->maxStrSize)->reset();
  }
  this->count = 0;
}


//==============================================================================
// DataOwner Implementation

void SharedNamedList::unsetIndexes(Int from, Int to)
{
  if (this->buffer == 0) return;
  for (Word i = 0; i < this->count; ++i) {
    IdentifiableObject *obj = this->get(i).get();
    if (obj != 0) Data::unsetIndexes(obj, from, to);
  }
}


//==============================================================================
// ListSharedContainer Implementation

void SharedNamedList::set(Int index, SharedPtr<IdentifiableObject> const &val)
{
  if (this->buffer == 0) {
    throw GeneralException(STR("List not initialized."), STR("Core::Data::SharedNamedList::set"));
  }
  if (index < 0 || index >= this->count) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedNamedList::set"),
                                   STR("Out of range."), index);
  }
  Byte *buf = this->buffer + index*this->getRecordSize();
  *reinterpret_cast<SharedPtr<IdentifiableObject>*>(buf+sizeof(Char)*this->maxStrSize) = val;
}


void SharedNamedList::remove(Int index)
{
  if (this->buffer == 0) {
    throw GeneralException(STR("List not initialized."), STR("Core::Data::SharedNamedList::remove"));
  }
  ASSERT(this->count >= 0);
  if (index < 0 || index >= this->count) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedNamedList::remove"),
                                   STR("Out of range."), index);
  }
  for (Int i = index; i < this->count-1; ++i) {
    this->set(i, this->getName(i+1).c_str(), this->get(i+1));
  }
  Byte *buf = this->buffer + (this->count-1)*this->getRecordSize();
  reinterpret_cast<SharedPtr<IdentifiableObject>*>(buf+sizeof(Char)*this->maxStrSize)->reset();
  this->count--;
}


SharedPtr<IdentifiableObject> const& SharedNamedList::get(Int index) const
{
  if (this->buffer == 0) {
    throw GeneralException(STR("List not initialized."), STR("Core::Data::SharedNamedList::get"));
  }
  if (index < 0 || index >= this->count) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedNamedList::get"),
                                   STR("Out of range."), index);
  }
  Byte *buf = this->buffer + index*this->getRecordSize();
  return *reinterpret_cast<SharedPtr<IdentifiableObject>*>(buf+sizeof(Char)*this->maxStrSize);
}


Int SharedNamedList::add(Char const *name, SharedPtr<IdentifiableObject> const &val)
{
  if (this->buffer == 0) {
    throw GeneralException(STR("List not initialized."), STR("Core::Data::SharedNamedList::add"));
  }
  if (this->count >= this->reservedCount) {
    this->reinitialize(this->maxStrSize, this->reservedCount+SharedNamedList::recordCountIncrement);
  }
  Byte *buf = this->buffer + this->count*this->getRecordSize();
  sbstr_cast(buf).assign(name==0?STR(""):name, this->maxStrSize);
  new(buf+sizeof(Char)*this->maxStrSize) SharedPtr<IdentifiableObject>(val);
  this->count++;
  return this->count - 1;
}


void SharedNamedList::set(Int index, Char const *name, SharedPtr<IdentifiableObject> const &val)
{
  if (this->buffer == 0) {
    throw GeneralException(STR("List not initialized."), STR("Core::Data::SharedNamedList::set"));
  }
  if (index < 0 || index >= this->count) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedNamedList::set"),
                                   STR("Out of range."), index);
  }
  Byte *buf = this->buffer + index*this->getRecordSize();
  sbstr_cast(buf).assign(name==0?STR(""):name, this->maxStrSize);
  *reinterpret_cast<SharedPtr<IdentifiableObject>*>(buf+sizeof(Char)*this->maxStrSize) = val;
}


const SbStr& SharedNamedList::getName(Int index) const
{
  if (this->buffer == 0) {
    throw GeneralException(STR("List not initialized."), STR("Core::Data::SharedNamedList::getName"));
  }
  if (index < 0 || index >= this->count) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedNamedList::getName"),
                                   STR("Out of range."), index);
  }
  Byte *buf = this->buffer + index*this->getRecordSize();
  return sbstr_cast(buf);
}

} } // namespace
