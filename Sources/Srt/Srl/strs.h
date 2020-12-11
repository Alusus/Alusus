/**
 * @file Srl/strs.h
 * Contains string classes.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SRL_STRS_S
#define SRL_STRS_S

namespace Srl
{

template<class T> class StringBase
{
  //=================
  // Member Variables

  private: Int *refCount;
  private: T *buf;

  //==========================
  // Constructors & Destructor

  public: StringBase() {
    this->_init();
  }

  public: StringBase(StringBase<T> const &str) {
    this->_init();
    this->assign(str);
  }

  public: StringBase(T const *buf) {
    this->_init();
    this->assign(buf);
  }

  public: StringBase(T const *buf, LongInt n) {
    this->_init();
    this->assign(buf, n);
  }

  public: StringBase(Bool skipCopying, T const *buf) {
    this->_init();
    if (skipCopying) {
      this->buf = const_cast<T*>(buf);
    } else {
      this->assign(buf);
    }
  }

  public: ~StringBase() {
    this->_release();
  }

  //=================
  // Member Functions

  private: void _init() {
    static T strTerminator(0);
    this->buf = &strTerminator;
    this->refCount = 0;
  }

  private: void _alloc(LongInt length) {
    this->refCount = (Int*)malloc(sizeof(Int) + sizeof(T) * (length + 1));
    this->buf = (T*)((ArchInt)this->refCount + sizeof(*this->refCount));
    *this->refCount = 1;
  }

  private: void _realloc(LongInt newLength) {
    this->refCount = (Int*)realloc(this->refCount, sizeof(Int) + sizeof(T) * (newLength + 1));
    this->buf = (T*)((ArchInt)this->refCount + sizeof(*this->refCount));
  }

  private: void _release() {
    if (this->refCount != 0) {
      --*this->refCount;
      if (*this->refCount == 0) free(this->refCount);
      this->_init();
    }
  }

  public: LongInt getLength() const {
    return getLength(this->buf);
  }

  public: void alloc(LongInt length) {
    this->_release();
    this->_alloc(length);
  }

  public: void assign(StringBase<T> const &str) {
    this->_release();
    this->refCount = str.refCount;
    this->buf = str.buf;
    if (this->refCount != 0) {
      ++*this->refCount;
    }
  }

  public: void assign(T const *buf) {
    this->_release();
    if (buf != 0) {
      this->_alloc(getLength(buf));
      copy(this->buf, buf);
    }
  }

  public: void assign(T const *buf, LongInt n) {
    this->_release();
    this->_alloc(n);
    copy(this->buf, buf, n);
    this->buf[n] = 0;
  }

  public: void append(T const *buf) {
    this->append(buf, getLength(buf));
  }

  public: void append(T const *buf, LongInt n) {
    auto bufLen = getLength(buf);
    if (bufLen == 0) return;
    auto thisBufLen = this->getLength();
    if (thisBufLen == 0) {
      this->assign(buf, n);
      return;
    }
    LongInt newLength = thisBufLen + bufLen;
    if (this->refCount == 0) {
      T *currentBuf = this->buf;
      this->_alloc(newLength);
      copy(this->buf, currentBuf);
      concat(this->buf, buf, n);
    } else if (*this->refCount > 1) {
      T *currentBuf = this->buf;
      --*this->refCount;
      this->_alloc(newLength);
      copy(this->buf, currentBuf);
      concat(this->buf, buf, n);
    } else {
      this->_realloc(newLength);
      concat(this->buf, buf, n);
    }
  }

  public: void append(T c) {
    this->append(&c, 1);
  }

  public: void append(LongInt i);

  public: void append(Double d);

  public: StringBase<T> concat(T const *buf) const {
    StringBase<T> newStr = *this;
    newStr.append(buf);
    return newStr;
  }

  public: StringBase<T> concat(T const *buf, LongInt n) const {
    StringBase<T> newStr = *this;
    newStr.append(buf, n);
    return newStr;
  }

  public: StringBase<T> concat(T c) const {
    StringBase<T> newStr = *this;
    newStr.append(c);
    return newStr;
  }

  public: StringBase<T> concat(LongInt i) const {
    StringBase<T> newStr = *this;
    newStr.append(i);
    return newStr;
  }

  public: StringBase<T> concat(Double d) const {
    StringBase<T> newStr = *this;
    newStr.append(d);
    return newStr;
  }

  public: LongInt find(T const *buf) const {
    return this->find((LongInt)0, buf);
  }

  public: LongInt find(LongInt startPos, T const *buf) const {
    T *startBuf = this->buf;
    while (*startBuf != 0 && startPos > 0) { --startPos; ++startBuf; }
    if (startPos > 0) return -1;
    void const *pos = find(startBuf, buf);
    if (pos == 0) return -1;
    return (ArchInt)pos - (ArchInt)this->buf;
  }

  public: ArchInt findLast (Char const *buf) {
    if (this->buf == 0) return -1;
    Char const *pos = findLast(this->buf, buf);
    if (pos == 0) return -1;
    return (ArchInt)pos - (ArchInt)this->buf;
  };

  public: LongInt find(T c) const {
    return this->find((LongInt)0, c);
  }

  public: LongInt find(LongInt startPos, T c) const {
    T *startBuf = this->buf;
    while (*startBuf != 0 && startPos > 0) { --startPos; ++startBuf; }
    if (startPos > 0) return -1;
    void const *pos = find(startBuf, c);
    if (pos == 0) return -1;
    return (ArchInt)pos - (ArchInt)this->buf;
  }

  public: ArchInt findLast (T c) {
      if (this->buf == 0) return -1;
      Char const *pos = findLast(this->buf, c);
      if (pos == 0) return -1;
      return (ArchInt)pos - (ArchInt)this->buf;
  };

  public: Int compare(T const *s) const {
    return compare(this->buf, s);
  }

  public: Int compare(T const *s, LongInt n) const {
    return compare(this->buf, s, n);
  }

  public: StringBase<T> replace(T const *match, T const *replacement) const {
    StringBase<T> str;
    LongInt matchLength = getLength(match);
    T *buf = this->buf;
    while (1) {
      T *found = find(buf, match);
      if (found == 0) {
        str.append(buf);
        return str;
      }
      ArchInt n = (ArchInt)found - (ArchInt)buf;
      str.append(buf, n);
      str.append(replacement);
      buf = (T*)((ArchInt)found + matchLength);
    }
    return str;
  }

  public: StringBase<T> trim() const {
    return this->_trim(true, true);
  }

  public: StringBase<T> trimStart() const {
    return this->_trim(true, false);
  }

  public: StringBase<T> trimEnd() const {
    return this->_trim(false, true);
  }

  public: StringBase<T> _trim(Bool trimStart, Bool trimEnd) const {
    if (this->getLength() == 0) return StringBase<T>();
    LongInt begin = 0;
    LongInt end = this->getLength() - 1;
    if (trimStart) while (isSpace(this->buf[begin])) { ++begin; }
    if (trimEnd) while (isSpacce(this->buf[end])) { --end; }
    if (end >= begin) {
      StringBase<T> str;
      str.assign((T*)((ArchInt)this->buf + begin), end - begin + 1);
      return str;
    } else {
      return StringBase<T>();
    }
  }

  public: StringBase<T> toUpperCase() const {
    StringBase<T> str = StringBase<T>();
    for (LongInt charIndex = 0; charIndex < this->getLength(); ++charIndex) {
      str += toUpper(this->at(charIndex));
    }
    return str;
  }

  public: StringBase<T> toLowerCase() const {
    StringBase<T> str = StringBase<T>();
    for (LongInt charIndex = 0; charIndex < this->getLength(); ++charIndex) {
      str += toLower(this->at(charIndex));
    }
    return str;
  }

  public: StringBase<T> slice(LongInt begin, LongInt count) const {
    StringBase<T> str;
    LongInt l = this->getLength();
    if (begin >= l) return str;
    str.assign((T*)((ArchInt)this->buf + begin), count);
    return str;
  }

  public: Array<StringBase<T>> split(T const *separator) const {
    Array<StringBase<T>> ary;
    StringBase<T> str;
    LongInt matchLength = getLength(separator);
    T *buf = this->buf;
    while (1) {
      T *found = find(buf, separator);
      if (found == 0) {
        str.assign(buf);
        ary.add(str);
        return ary;
      }
      ArchInt n = (ArchInt)found - (ArchInt)buf;
      if (n == 0) str = StringBase<T>(); else str.assign(buf, n);
      ary.add(str);
      buf = (T*)((ArchInt)found + matchLength);
    }
    return ary;
  }

  public: static StringBase<T> merge(Array<StringBase<T>> ary, T const *separator) {
    StringBase<T> str;
    for (LongInt i = 0; i < ary.getLength(); ++i) {
      if (i != 0) str += separator;
      str += ary(i);
    }
    return str;
  }

  public: static StringBase<T> merge(Array<T*> ary, T const *separator) {
    StringBase<T> str;
    for (LongInt i = 0; i < ary.getLength(); ++i) {
      if (i != 0) str += separator;
      str += ary(i);
    }
    return str;
  }

  public: void clear() {
    this->_release();
  }

  public: T const* getBuf() const {
    return this->buf;
  }

  //==========
  // Operators

  public: StringBase<T>& operator=(StringBase<T> const &value) {
    this->assign(value);
    return *this;
  }

  public: StringBase<T>& operator=(T const *value) {
    this->assign(value);
    return *this;
  }

  public: operator T const*() const {
    return this->buf;
  }

  public: StringBase<T> operator+(T const *value) const {
    return this->concat(value);
  }
  public: StringBase<T> operator+(T value) const {
    return this->concat(value);
  }
  public: StringBase<T> operator+(LongInt value) const {
    return this->concat(value);
  }
  public: StringBase<T> operator+(Double value) const {
    return this->concat(value);
  }

  public: StringBase<T>& operator+=(T const *value) {
    this->append(value);
    return *this;
  }
  public: StringBase<T>& operator+=(T value) {
    this->append(value);
    return *this;
  }
  public: StringBase<T>& operator+=(LongInt value) {
    this->append(value);
    return *this;
  }
  public: StringBase<T>& operator+=(Double value) {
    this->append(value);
    return *this;
  }

  public: T operator()(LongInt i) const {
    return this->buf[i];
  }

  public: Bool operator==(T const *value) const {
    return this->compare(value) == 0;
  }
  public: Bool operator!=(T const *value) const {
    return this->compare(value) != 0;
  }
  public: Bool operator>(T const *value) const {
    return this->compare(value) > 0;
  }
  public: Bool operator<(T const *value) const {
    return this->compare(value) < 0;
  }
  public: Bool operator>=(T const *value) const {
    return this->compare(value) >= 0;
  }
  public: Bool operator<=(T const *value) const {
    return this->compare(value) <= 0;
  }

  //=================
  // Static Functions

  public: static Bool isSpace(T c) {
    return c != 0 && (c == ' ' || c == '\n' || c == '\r' || c == '\t');
  }

  public: static Bool isEqual(T const *s1, T const *s2) {
    return compare(s1, s2) == 0;
  }

  public: static T* remove(T *chrs, T chr) {
    T *pointer = find(chrs, chr);
    while (pointer != 0) {
      copy(pointer, (T*)((ArchInt)pointer + 1));
      pointer = find(pointer, chr);
    }
    return chrs;
  }

  public: static T* replace(T *chrs, T fromChr, T toChr) {
    T *pointer = find(chrs, fromChr);
    while (pointer != 0) {
      *pointer = toChr;
      pointer = find((T*)((ArchInt)pointer + 1), fromChr);
    }
    return chrs;
  }

  public: static T* charAt(T const *chrs, LongInt index) {
    static T buffer[2];
    copy(&buffer, (T*)((ArchInt)chrs + index), 1);
    buffer[1] = 0;
    return &buffer;
  }

  public: static T const* find(T const *s, T c);

  public: static T const* find(T const *s, T c, LongInt n);

  public: static T const* find(T const *haystack, T const *needle);

  public: static T const* findLast(T const *s, T c);

  public: static T const* findLast(T const *haystack, T const *needle)
  {
    if (*needle == '\0') return (T*) haystack;

    T *result = 0;
    for (;;) {
      T *p = strstr(haystack, needle);
      if (p == 0) break;
      result = p;
      haystack = p + 1;
    }

    return result;
  }

  public: static Int compare(T const *s1, T const *s2);

  public: static Int compare(T const *s1, T const *s2, LongInt n);

  public: static T* copy(T *dest, T const *src);

  public: static T* copy(T *dest, T const *src, LongInt n);

  public: static T* concat(T *dest, T const *src);

  public: static T* concat(T *dest, T const *src, LongInt n);

  public: static LongInt getLength(T const *s);

  public: static T toUpper(T c);

  public: static T toLower(T c);
}; // class


//==============================================================================
// Function Specializations

template<> inline void StringBase<Char>::append(LongInt i) {
  Char buf[20];
  sprintf(buf, "%ld", i);
  this->append(buf);
}
template<> inline void StringBase<WChar>::append(LongInt i) {
  WChar buf[20];
  swprintf(buf, 20, L"%ld", i);
  this->append(buf);
}

template<> inline void StringBase<Char>::append(Double d) {
  Char buf[20];
  sprintf(buf, "%f", d);
  this->append(buf);
}
template<> inline void StringBase<WChar>::append(Double d) {
  WChar buf[20];
  swprintf(buf, 20, L"%f", d);
  this->append(buf);
}

template<> inline Char const* StringBase<Char>::find(Char const *s, Char c) {
  return strchr(s, c);
}
template<> inline WChar const* StringBase<WChar>::find(WChar const *s, WChar c) {
  return wcschr(s, c);
}

template<> inline Char const* StringBase<Char>::find(Char const *s, Char c, LongInt n) {
  return (Char*)memchr(s, c, n);
}
template<> inline WChar const* StringBase<WChar>::find(WChar const *s, WChar c, LongInt n) {
  return wmemchr(s, c, n);
}

template<> inline Char const* StringBase<Char>::find(Char const *haystack, Char const *needle) {
  return strstr(haystack, needle);
}
template<> inline WChar const* StringBase<WChar>::find(WChar const *haystack, WChar const *needle) {
  return wcsstr(haystack, needle);
}

template<> inline Char const* StringBase<Char>::findLast(Char const *s, Char c) {
  return strrchr(s, c);
}
template<> inline WChar const* StringBase<WChar>::findLast(WChar const *s, WChar c) {
  return wcsrchr(s, c);
}

template<> inline Int StringBase<Char>::compare(Char const *s1, Char const *s2) {
  return strcmp(s1, s2);
}
template<> inline Int StringBase<WChar>::compare(WChar const *s1, WChar const *s2) {
  return wcscmp(s1, s2);
}

template<> inline Int StringBase<Char>::compare(Char const *s1, Char const *s2, LongInt n) {
  return strncmp(s1, s2, n);
}
template<> inline Int StringBase<WChar>::compare(WChar const *s1, WChar const *s2, LongInt n) {
  return wcsncmp(s1, s2, n);
}

template<> inline Char* StringBase<Char>::copy(Char *dest, Char const *src) {
  return strcpy(dest, src);
}
template<> inline WChar* StringBase<WChar>::copy(WChar *dest, WChar const *src) {
  return wcscpy(dest, src);
}

template<> inline Char* StringBase<Char>::copy(Char *dest, Char const *src, LongInt n) {
  return strncpy(dest, src, n);
}
template<> inline WChar* StringBase<WChar>::copy(WChar *dest, WChar const *src, LongInt n) {
  return wcsncpy(dest, src, n);
}

template<> inline Char* StringBase<Char>::concat(Char *dest, Char const *src) {
  return strcat(dest, src);
}
template<> inline WChar* StringBase<WChar>::concat(WChar *dest, WChar const *src) {
  return wcscat(dest, src);
}

template<> inline Char* StringBase<Char>::concat(Char *dest, Char const *src, LongInt n) {
  return strncat(dest, src, n);
}
template<> inline WChar* StringBase<WChar>::concat(WChar *dest, WChar const *src, LongInt n) {
  return wcsncat(dest, src, n);
}

template<> inline LongInt StringBase<Char>::getLength(Char const *s) {
  return strlen(s);
}
template<> inline LongInt StringBase<WChar>::getLength(WChar const *s) {
  return wcslen(s);
}

template<> inline Char StringBase<Char>::toUpper(Char c) {
  return toupper(c);
}
template<> inline WChar StringBase<WChar>::toUpper(WChar c) {
  return towupper(c);
}

template<> inline Char StringBase<Char>::toLower(Char c) {
  return tolower(c);
}
template<> inline WChar StringBase<WChar>::toLower(WChar c) {
  return towlower(c);
}


//==============================================================================
// Type Defs

typedef StringBase<Char> String;

typedef StringBase<WChar> WString;

}; // namespace

#endif
