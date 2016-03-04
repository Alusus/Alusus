/**
 * @file Core/Data/SharedNamedList.cpp
 * Contains the implementation of class Core::Data::SharedNamedList.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
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
    throw EXCEPTION(GenericException, STR("Buffer is not initialized."));
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
        sbstr_cast(destBuf).assign(sbstr_cast(srcBuf).c_str(), this->maxStrSize);
        SharedPtr<IdentifiableObject> *destPtr =
            new(destBuf+sizeof(Char)*this->maxStrSize) SharedPtr<IdentifiableObject>();
        *destPtr = *reinterpret_cast<SharedPtr<IdentifiableObject>*>(srcBuf+sizeof(Char)*oldStrSize);
      }
    }
  }
  delete[] oldBuf;
}


void SharedNamedList::copy(const SharedNamedList *src)
{
  if (src->buffer == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("src"), STR("Object is not initialized."));
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
  if (this->buffer != 0) this->clear();
  if (this->buffer != 0) delete[] this->buffer;
  this->buffer = 0;
}


void SharedNamedList::clear()
{
  if (this->buffer == 0) {
    throw EXCEPTION(GenericException, STR("List not initialized."));
  }
  ASSERT(this->count >= 0);
  for (Int i = 0; i < this->count; ++i) {
    Byte *buf = this->buffer + i*this->getRecordSize();
    SharedPtr<IdentifiableObject> *obj =
        reinterpret_cast<SharedPtr<IdentifiableObject>*>(buf+sizeof(Char)*this->maxStrSize);
    RESET_OWNED_SHAREDPTR(*obj);
  }
  this->count = 0;
}


//==============================================================================
// Data Access Functions

Int SharedNamedList::add(Char const *name, SharedPtr<IdentifiableObject> const &val)
{
  if (this->buffer == 0) {
    throw EXCEPTION(GenericException, STR("List not initialized."));
  }
  if (this->count >= this->reservedCount) {
    this->reinitialize(this->maxStrSize, this->reservedCount+SharedNamedList::recordCountIncrement);
  }
  Byte *buf = this->buffer + this->count*this->getRecordSize();
  sbstr_cast(buf).assign(name==0?STR(""):name, this->maxStrSize);
  new(buf+sizeof(Char)*this->maxStrSize) SharedPtr<IdentifiableObject>(val);
  if (this->owningEnabled) {
    OWN_SHAREDPTR(val);
  }
  this->count++;
  return this->count - 1;
}


void SharedNamedList::insert(Int index, Char const *name, SharedPtr<IdentifiableObject> const &val)
{
  if (this->buffer == 0) {
    throw EXCEPTION(GenericException, STR("List not initialized."));
  }
  if (index < 0 || index > this->getCount()) {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
  }

  if (this->count >= this->reservedCount) {
    // Allocate a new buffer.
    this->reservedCount += SharedNamedList::recordCountIncrement;
    Byte *oldBuf = this->buffer;
    this->buffer = new Byte[this->reservedCount * this->getRecordSize()];

    if (!this->isEmpty()) {
      if (index > 0) memcpy(this->buffer, oldBuf, this->getRecordSize()*index);
      if (index < this->count) {
        memcpy(this->buffer + (index+1) * this->getRecordSize(),
               oldBuf + index * this->getRecordSize(),
               (this->count - index) * this->getRecordSize());
      }
    }
    delete[] oldBuf;
  } else {
    // Shift contents of the current buffer.
    memmove(this->buffer + (index+1) * this->getRecordSize(),
            this->buffer + index * this->getRecordSize(),
            (this->count - index) * this->getRecordSize());
  }
  Byte *buf = this->buffer + index * this->getRecordSize();
  sbstr_cast(buf).assign(name==0?STR(""):name, this->maxStrSize);
  new(buf+sizeof(Char)*this->maxStrSize) SharedPtr<IdentifiableObject>(val);
  if (this->owningEnabled) {
    OWN_SHAREDPTR(val);
  }
}


void SharedNamedList::set(Int index, Char const *name, SharedPtr<IdentifiableObject> const &val)
{
  if (this->buffer == 0) {
    throw EXCEPTION(GenericException, STR("List not initialized."));
  }
  if (index < 0 || index >= this->count) {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
  }
  Byte *buf = this->buffer + index*this->getRecordSize();
  sbstr_cast(buf).assign(name==0?STR(""):name, this->maxStrSize);
  SharedPtr<IdentifiableObject> *obj =
      reinterpret_cast<SharedPtr<IdentifiableObject>*>(buf+sizeof(Char)*this->maxStrSize);
  DISOWN_SHAREDPTR(*obj);
  *obj = val;
  if (this->owningEnabled) {
    OWN_SHAREDPTR(val);
  }
}


void SharedNamedList::set(Int index, SharedPtr<IdentifiableObject> const &val)
{
  if (this->buffer == 0) {
    throw EXCEPTION(GenericException, STR("List not initialized."));
  }
  if (index < 0 || index >= this->count) {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
  }
  Byte *buf = this->buffer + index*this->getRecordSize();
  SharedPtr<IdentifiableObject> *obj =
      reinterpret_cast<SharedPtr<IdentifiableObject>*>(buf+sizeof(Char)*this->maxStrSize);
  DISOWN_SHAREDPTR(*obj);
  *obj = val;
  if (this->owningEnabled) {
    OWN_SHAREDPTR(val);
  }
}


SharedPtr<IdentifiableObject> const& SharedNamedList::getShared(Int index) const
{
  if (this->buffer == 0) {
    throw EXCEPTION(GenericException, STR("List not initialized."));
  }
  if (index < 0 || index >= this->count) {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
  }
  Byte *buf = this->buffer + index*this->getRecordSize();
  return *reinterpret_cast<SharedPtr<IdentifiableObject>*>(buf+sizeof(Char)*this->maxStrSize);
}


//==============================================================================
// DataOwner Implementation

void SharedNamedList::unsetIndexes(Int from, Int to)
{
  if (this->buffer == 0) return;
  for (Word i = 0; i < this->count; ++i) {
    IdentifiableObject *obj = this->get(i);
    if (obj != 0) Data::unsetIndexes(obj, from, to);
  }
}


//==============================================================================
// ListContainer Implementation

void SharedNamedList::remove(Int index)
{
  if (this->buffer == 0) {
    throw EXCEPTION(GenericException, STR("List not initialized."));
  }
  ASSERT(this->count >= 0);
  if (index < 0 || index >= this->count) {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
  }
  // Reset owner of the object to be removed.
  IdentifiableObject *o = this->get(index);
  DISOWN_PLAINPTR(o);
  // Shift the remaining items.
  for (Int i = index; i < this->count-1; ++i) {
    this->set(i, this->getName(i+1).c_str(), this->get(i+1));
  }
  // Remove the last item.
  Byte *buf = this->buffer + (this->count-1)*this->getRecordSize();
  SharedPtr<Node> *obj = reinterpret_cast<SharedPtr<Node>*>(buf+sizeof(Char)*this->maxStrSize);
  obj->reset();
  this->count--;
}


const SbStr& SharedNamedList::getName(Int index) const
{
  if (this->buffer == 0) {
    throw EXCEPTION(GenericException, STR("List not initialized."));
  }
  if (index < 0 || index >= this->count) {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
  }
  Byte *buf = this->buffer + index*this->getRecordSize();
  return sbstr_cast(buf);
}

} } // namespace
