/**
 * @file Srl/refs.h
 * Contains definitions of different reference types.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SRL_REFS_H
#define SRL_REFS_H

namespace Srl
{

template<class T> class WkRef;

//==============================================================================
// RefCounter
// A ref counting object to be used by the shared references.
class RefCounter
{
  //=================
  // Member Variables

  public: Int count;
  public: Bool singleAllocation;
  public: void (*terminator)(void*);
  public: void *managedObj;

  //=================
  // Member Functions

  public: static RefCounter* alloc(Int size, void (*terminator)(void*)) {
    RefCounter *refCounter;
    refCounter = (RefCounter*)malloc(sizeof(RefCounter) + size);
    refCounter->count = 0;
    refCounter->singleAllocation = true;
    refCounter->terminator = terminator;
    refCounter->managedObj = (void*)((PtrInt)refCounter + sizeof(RefCounter));
    return refCounter;
  }

  public: static RefCounter* alloc(void *managedObj, void (*terminator)(void*)) {
    RefCounter *refCounter;
    refCounter = (RefCounter*)malloc(sizeof(RefCounter));
    refCounter->count = 0;
    refCounter->singleAllocation = false;
    refCounter->terminator = terminator;
    refCounter->managedObj = managedObj;
    return refCounter;
  }

  public: static void release(RefCounter *refCounter) {
    refCounter->terminator(refCounter->managedObj);
    if (!refCounter->singleAllocation) {
      free(refCounter->managedObj);
    }
    free(refCounter);
  }
}; // class


//==============================================================================
// SrdRef
// Shared Reference
template<class T> class SrdRef
{
  //=================
  // Member Variables

  protected: RefCounter *refCounter;
  protected: T *obj;

  //==========================
  // Constructors & Destructor

  public: SrdRef() {
    this->_init();
  }

  public: SrdRef(SrdRef<T> const &r) {
    this->_init();
    this->assign(r);
  }

  public: SrdRef(SrdRef<T> &&r) {
    this->_init();
    this->assign(r);
  }

  public: template<class T2> SrdRef(SrdRef<T2> const &p) {
    this->_init();
    this->assign(p.getRefCounter(), p.get());
  }

  public: template<class T2> SrdRef(SrdRef<T2> &&p) {
    this->_init();
    this->assign(p.getRefCounter(), p.get());
  }

  public: SrdRef(WkRef<T> const &r) {
    this->_init();
    this->assign(r);
  };

  public: SrdRef(WkRef<T> &&r) {
    this->_init();
    this->assign(r);
  };

  public: template<class T2> SrdRef(WkRef<T2> const &p) {
    this->_init();
    this->assign(p.getRefCounter(), p.get());
  }

  public: template<class T2> SrdRef(WkRef<T2> &&p) {
    this->_init();
    this->assign(p.getRefCounter(), p.get());
  }

  public: SrdRef(RefCounter *c, T *r) {
    this->_init();
    this->assign(c, r);
  };

  public: SrdRef(T *r) {
    this->_init();
    this->own(r);
  }

  public: ~SrdRef() {
    this->release();
  }

  //=================
  // Member Functions

  protected: void _init() {
    this->refCounter = 0;
    this->obj = 0;
  }

  public: T* alloc() {
    this->release();
    this->refCounter = RefCounter::alloc(sizeof(T), &SrdRef<T>::terminate);
    this->refCounter->count = 1;
    this->obj = (T*)this->refCounter->managedObj;
    return this->obj;
  }

  public: static SrdRef<T> allocToNew() {
    SrdRef<T> r;
    r.alloc();
    return r;
  }

  public: template<class ...ARGS> void construct(ARGS... args) {
    this->alloc();
    new(this->refCounter->managedObj) T(args...);
  }

  public: template<class ...ARGS> static SrdRef<T> constructToNew(ARGS... args) {
    SrdRef<T> r;
    r.construct(args...);
    return r;
  }

  public: void own(T *existingObj) {
    if (existingObj == this->obj) return;
    this->release();
    if (existingObj != 0) {
      this->refCounter = RefCounter::alloc(existingObj, &SrdRef<T>::terminate);
      this->refCounter->count = 1;
      this->obj = existingObj;
    }
  }

  public: static SrdRef<T> ownToNew(T *existingObj) {
    SrdRef<T> r;
    r.own(existingObj);
    return r;
  }

  public: void release() {
    if (this->refCounter != 0) {
      --this->refCounter->count;
      if (this->refCounter->count == 0) {
        RefCounter::release(this->refCounter);
      }
      this->_init();
    }
  }

  public: void assign(SrdRef<T> const &sr) {
    this->assign(sr.refCounter, sr.obj);
  }

  public: void assign(WkRef<T> const &wr) {
    this->assign(wr.refCounter, wr.obj);
  }

  public: void assign(RefCounter *c, T *r) {
    if (c != this->refCounter) {
      this->release();
      this->refCounter = c;
      if (this->refCounter != 0) {
        ++this->refCounter->count;
      }
    }
    this->obj = r;
  }

  public: T* get() const {
    return this->obj;
  }

  public: RefCounter* getRefCounter() const {
    return this->refCounter;
  }

  private: static void terminate(void *p) {
    ((T*)p)->~T();
  }

  //==========
  // Operators

  public: T* operator->() const {
    return this->obj;
  }

  public: SrdRef<T>& operator=(SrdRef<T> const &r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: SrdRef<T>& operator=(SrdRef<T> &&r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: template<class T2> SrdRef<T>& operator=(SrdRef<T2> const &r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: template<class T2> SrdRef<T>& operator=(SrdRef<T2> &&r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: SrdRef<T>& operator=(WkRef<T> const &r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: SrdRef<T>& operator=(WkRef<T> &&r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: template<class T2> SrdRef<T>& operator=(WkRef<T2> const &r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: template<class T2> SrdRef<T>& operator=(WkRef<T2> &&r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: Bool operator==(T *obj) const {
    return this->obj == obj;
  }

  public: Bool operator==(std::nullptr_t p) const {
    return this->obj == p;
  }
}; // class


//==============================================================================
// WkRef
// Weak Reference
template<class T> class WkRef
{
  //=================
  // Member Variables

  protected: RefCounter *refCounter;
  protected: T *obj;

  //==========================
  // Constructors & Destructor

  public: WkRef() {
    this->_init();
  }

  public: WkRef(SrdRef<T> const &r) {
    this->_init();
    this->assign(r);
  }

  public: WkRef(SrdRef<T> &&r) {
    this->_init();
    this->assign(r);
  }

  public: template<class T2> WkRef(SrdRef<T2> const &p) {
    this->_init();
    this->assign(p.getRefCounter(), p.get());
  }

  public: template<class T2> WkRef(SrdRef<T2> &&p) {
    this->_init();
    this->assign(p.getRefCounter(), p.get());
  }

  public: WkRef(WkRef<T> const &r) {
    this->_init();
    this->assign(r);
  };

  public: WkRef(WkRef<T> &&r) {
    this->_init();
    this->assign(r);
  };

  public: template<class T2> WkRef(WkRef<T2> const &p) {
    this->_init();
    this->assign(p.getRefCounter(), p.get());
  }

  public: template<class T2> WkRef(WkRef<T2> &&p) {
    this->_init();
    this->assign(p.getRefCounter(), p.get());
  }

  public: WkRef(RefCounter *c, T *r) {
    this->_init();
    this->assign(c, r);
  };

  //=================
  // Member Functions

  protected: void _init() {
    this->refCounter = 0;
    this->obj = 0;
  }

  public: void release() {
    this->_init();
  }

  public: void assign(SrdRef<T> const &r) {
    this->refCounter = r.refCounter;
    this->obj = r.obj;
  }

  public: void assign(WkRef<T> const &r) {
    this->refCounter = r.refCounter;
    this->obj = r.obj;
  }

  public: void assign(RefCounter *c, T *r) {
    this->refCounter = c;
    this->obj = r;
  };

  public: T* get() const {
    return this->obj;
  }

  public: RefCounter* getRefCounter() const {
    return this->refCounter;
  }

  //==========
  // Operators

  public: T* operator->() const {
    return this->obj;
  }

  public: WkRef<T>& operator=(SrdRef<T> const &r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: WkRef<T>& operator=(SrdRef<T> &&r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: template<class T2> WkRef<T>& operator=(SrdRef<T2> const &r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: template<class T2> WkRef<T>& operator=(SrdRef<T2> &&r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: WkRef<T>& operator=(WkRef<T> const &r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: WkRef<T>& operator=(WkRef<T> &&r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: template<class T2> WkRef<T>& operator=(WkRef<T2> const &r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: template<class T2> WkRef<T>& operator=(WkRef<T2> &&r) {
    this->assign(r.getRefCounter(), r.get());
    return *this;
  }

  public: Bool operator==(T *obj) const {
    return this->obj == obj;
  }

  public: Bool operator==(std::nullptr_t p) const {
    return this->obj == p;
  }
}; // class


//==============================================================================
// Global Operators

template<typename T>
inline Bool operator==(SrdRef<T> const & r, std::nullptr_t) {
  return r.get() == nullptr;
}

template<typename T>
inline Bool operator!=(SrdRef<T> const & r, std::nullptr_t) {
  return r.get() != nullptr;
}

template<typename T>
inline Bool operator==(WkRef<T> const & r, std::nullptr_t) {
  return r.get() == nullptr;
}

template<typename T>
inline Bool operator!=(WkRef<T> const & r, std::nullptr_t) {
  return r.get() != nullptr;
}

} // namespace

#endif
