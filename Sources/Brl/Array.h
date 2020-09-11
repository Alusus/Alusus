/**
 * @file Brl/Array.h
 * Contains the header of class Brl::Array.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef BRL_ARRAY_H
#define BRL_ARRAY_H

namespace Brl
{

template <class T> class Array
{
  //============================================================================
  // Member Variables

  private: Int *refCount;
  private: Int *length;
  private: Int *bufSize;
  private: T *buf;

  //============================================================================
  // Cosntructors & Destructor

  public: Array() {
    this->_init();
  }

  public: Array(Array const &src) {
    this->_init();
    this->assign(src);
  }

  public: Array(Int size, T val) {
    this->_init();
    this->_alloc(size);
    for (; size > 0; --size) this->add(val);
  }

  public: ~Array() {
    this->_release();
  }

  //============================================================================
  // Member Functions

  private: void _init() {
    this->refCount = 0;
    this->length = 0;
    this->bufSize = 0;
    this->buf = 0;
  }

  private: void _alloc(Int size) {
    if (size < 2) size = 2;
    Int byteCount = sizeof(T) * size + sizeof(Int) * 3;
    this->refCount = (Int*)malloc(byteCount);
    this->length = (Int*)((PtrInt)this->refCount + sizeof(*this->refCount));
    this->bufSize = (Int*)((PtrInt)this->length + sizeof(*this->length));
    this->buf = (T*)((PtrInt)this->bufSize + sizeof(*this->bufSize));
    *this->bufSize = size;
    *this->length = 0;
    *this->refCount = 1;
  }

  private: void _realloc(Int newSize) {
    if (newSize < 2) newSize = 2;
    Int byteCount = sizeof(T) * newSize + sizeof(Int) * 3;
    this->refCount = (Int*)realloc(this->refCount, byteCount);
    this->length = (Int*)((PtrInt)this->refCount + sizeof(*this->refCount));
    this->bufSize = (Int*)((PtrInt)this->length + sizeof(*this->length));
    this->buf = (T*)((PtrInt)this->bufSize + sizeof(*this->bufSize));
    *this->bufSize = newSize;
  }

  private: void _release() {
    if (this->refCount != 0) {
      --(*this->refCount);
      if (*this->refCount == 0) {
        Int i;
        for (i = 0; i < *this->length; ++i) this->buf[i].~T();
        free(this->refCount);
      }
      this->_init();
    }
  }

  public: void reserve(Int size) {
    if (this->refCount == 0) this->_alloc(size);
    else this->_realloc(size);
  }

  public: Int getLength() const {
    if (this->length == 0) return 0;
    else return *this->length;
  }

  public: Int getBufSize() const {
    if (this->bufSize == 0) return 0;
    else return *this->bufSize;
  }

  public: void assign (Array<T> const &ary) {
    this->_release();
    this->refCount = ary.refCount;
    this->length = ary.length;
    this->bufSize = ary.bufSize;
    this->buf = ary.buf;
    if (this->refCount != 0) {
      ++(*this->refCount);
    }
  }

  private: void _prepareToModify (Bool enlarge) {
    if (this->refCount == 0) {
      this->_alloc(2);
    } else if (*this->refCount == 1) {
      if (enlarge && *this->length >= *this->bufSize) this->_realloc(*this->bufSize + (*this->bufSize >> 1));
    } else {
      Int curLength = *this->length;
      T *curBuf = this->buf;
      --(*this->refCount);
      this->_alloc(curLength + curLength >> 1);
      Int i;
      for (i = 0; i < curLength; ++i) new(this->buf + i) T(curBuf[i]);
      *this->length = curLength;
    }
  }

  public: void add (T item) {
    this->_prepareToModify(true);
    new (this->buf + *this->length) T(item);
    ++(*this->length);
  }

  public: void insert (Int index, T item) {
    if (index < 0 || index >= this->getLength()) {
      this->add(item);
    } else {
      this->_prepareToModify(true);
      memcpy(this->buf + index + 1, this->buf + index, sizeof(T) * (*this->length - index));
      new(this->buf + index) T(item);
      ++(*this->length);
    }
  }

  public: void remove (Int index) {
    if (index >= 0 && index < this->getLength()) {
      this->_prepareToModify(false);
      this->buf[index].~T();
      if (index < this->getLength() - 1) {
        memcpy(this->buf + index, this->buf + index + 1, sizeof(T) * (*this->length - (index + 1)));
      };
      --(*this->length);
    }
  }

  public: void clear() {
    this->_release();
  };

  public: T& at (Int i) {
    static T dummy;
    if (i >= 0 && i < this->getLength()) return this->buf[i]; else return dummy;
  }

  public: T const& at (Int i) const {
    static T dummy;
    if (i >= 0 && i < this->getLength()) return this->buf[i]; else return dummy;
  }

  //============================================================================
  // Operators

  public: void operator=(Array<T> const &value) {
    this->assign(value);
  }

  public: T& operator()(Int i) {
    return this->at(i);
  };

  public: T const& operator()(Int i) const {
    return this->at(i);
  };

}; // class

} // namespace

#endif
