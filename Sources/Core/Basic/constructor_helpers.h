/**
 * @file Core/Basic/constructor_helpers.h
 * Contains constructor related helper macros.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_CONSTRUCTORHELPERS_H
#define CORE_BASIC_CONSTRUCTORHELPERS_H

#define ATTR_INITIALIZATION_LOOP(attrs) \
  if (attrs.size() > 0) { \
    for (auto attr : attrs) { \
      if (attr.type == Argument::Type::PTR) { \
        this->setMember(attr.id, attr.ptr); \
      } else if (attr.type == Argument::Type::REF) { \
        this->setMember(attr.id, *attr.ref); \
      } else if (attr.type == Argument::Type::SHARED) { \
        this->setMember(attr.id, attr.shared); \
      } else { \
        throw EXCEPTION(InvalidArgumentException, attr.id, S("Invalid argument type.")); \
      } \
    } \
  }

#define MAP_ELEMENT_INITIALIZATION_LOOP(elements) \
  if (elements.size() > 0) { \
    for (auto element : elements) { \
      if (element.type == Argument::Type::PTR) { \
        this->setElement(element.id, element.ptr); \
      } else if (element.type == Argument::Type::SHARED) { \
        this->setElement(element.id, element.shared.get()); \
      } else { \
        throw EXCEPTION(InvalidArgumentException, element.id, S("Invalid argument type.")); \
      } \
    } \
  }

#define LIST_ELEMENT_INITIALIZATION_LOOP(elements) \
  if (elements.size() > 0) { \
    for (auto element : elements) this->addElement(element.get()); \
  }

#define IMPLEMENT_EMPTY_CONSTRUCTOR(type, ...) \
  public: type() { \
    __VA_ARGS__; \
  } \
  public: static SharedPtr<type> create() \
  { \
    return newSrdObj<type>(); \
  }


#define IMPLEMENT_ATTR_CONSTRUCTOR(type, ...) \
  public: type(std::initializer_list<Argument> const &attrs) \
  { \
    ATTR_INITIALIZATION_LOOP(attrs) \
    __VA_ARGS__; \
  } \
  public: static SharedPtr<type> create(std::initializer_list<Argument> const &attrs) \
  { \
    return newSrdObj<type>(attrs); \
  }


#define IMPLEMENT_LIST_CONSTRUCTOR(type, ...) \
  public: type(std::initializer_list<TioSharedPtr> const &elements) \
  { \
    LIST_ELEMENT_INITIALIZATION_LOOP(elements) \
    __VA_ARGS__; \
  } \
  public: static SharedPtr<type> create(std::initializer_list<TioSharedPtr> const &args) \
  { \
    return newSrdObj<type>(args); \
  }


#define IMPLEMENT_MAP_CONSTRUCTOR(type, ...) \
  public: type(std::initializer_list<Argument> const &elements) \
  { \
    MAP_ELEMENT_INITIALIZATION_LOOP(elements) \
    __VA_ARGS__; \
  } \
  public: static SharedPtr<type> create(std::initializer_list<Argument> const &elements) \
  { \
    return newSrdObj<type>(elements); \
  }


#define IMPLEMENT_ATTR_LIST_CONSTRUCTOR(type, ...) \
  public: type(std::initializer_list<Argument> const &attrs, \
               std::initializer_list<TioSharedPtr> const &elements) \
  { \
    ATTR_INITIALIZATION_LOOP(attrs) \
    LIST_ELEMENT_INITIALIZATION_LOOP(elements) \
    __VA_ARGS__; \
  } \
  public: static SharedPtr<type> create(std::initializer_list<Argument> const &attrs, \
                                        std::initializer_list<TioSharedPtr> const &elements) \
  { \
    return newSrdObj<type>(attrs, elements); \
  }


#define IMPLEMENT_ATTR_MAP_CONSTRUCTOR(type, ...) \
  public: type(std::initializer_list<Argument> const &attrs, \
               std::initializer_list<Argument> const &elements) \
  { \
    ATTR_INITIALIZATION_LOOP(attrs) \
    MAP_ELEMENT_INITIALIZATION_LOOP(elements) \
    __VA_ARGS__; \
  } \
  public: static SharedPtr<type> create(std::initializer_list<Argument> const &attrs, \
                                        std::initializer_list<Argument> const &elements) \
  { \
    return newSrdObj<type>(attrs, elements); \
  }

#endif
