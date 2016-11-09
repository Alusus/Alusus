/**
 * @file Core/Data/constructor_helpers.h
 * Contains constructor related helper macros.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_CONSTRUCTORHELPERS_H
#define CORE_DATA_CONSTRUCTORHELPERS_H

#define _ATTR_INITIALIZATION_LOOP \
  if (attrs.size() > 0) { \
    for (auto attr : attrs) { \
      if (attr.type == ArgumentType::TI_PTR) { \
        this->setRtMember(attr.id, attr.tiPtr); \
      } else if (attr.type == ArgumentType::TI_REF) { \
        this->setRtMember(attr.id, *attr.tiRef); \
      } else if (attr.type == ArgumentType::TI_SHARED) { \
        this->setRtMember(attr.id, attr.tiShared); \
      } else { \
        throw EXCEPTION(InvalidArgumentException, attr.id, STR("Invalid argument type.")); \
      } \
    } \
  }

#define _MAP_ELEMENT_INITIALIZATION_LOOP \
  if (elements.size() > 0) { \
    for (auto element : elements) { \
      if (element.type == ArgumentType::TI_PTR) { \
        this->set(element.id, element.tiPtr); \
      } else if (element.type == ArgumentType::TI_SHARED) { \
        this->set(element.id, element.tiShared.get()); \
      } else { \
        throw EXCEPTION(InvalidArgumentException, element.id, STR("Invalid argument type.")); \
      } \
    } \
  }

#define _LIST_ELEMENT_INITIALIZATION_LOOP \
  if (elements.size() > 0) { \
    for (auto element : elements) this->add(element); \
  }

#define IMPLEMENT_EMPTY_CONSTRUCTOR(type) \
  public: type() {} \
  public: static SharedPtr<type> create() \
  { \
    return std::make_shared<type>(); \
  }


#define IMPLEMENT_ATTR_CONSTRUCTOR(type) \
  public: type(std::initializer_list<Argument<Char const*>> const &attrs) \
  { \
    _ATTR_INITIALIZATION_LOOP \
  } \
  public: static SharedPtr<type> create(std::initializer_list<Argument<Char const*>> const &attrs) \
  { \
    return std::make_shared<type>(attrs); \
  }


#define IMPLEMENT_LIST_CONSTRUCTOR(type) \
  public: type(std::initializer_list<TioSharedPtr> const &elements) \
  { \
    _LIST_ELEMENT_INITIALIZATION_LOOP \
  } \
  public: static SharedPtr<type> create(std::initializer_list<TioSharedPtr> const &args) \
  { \
    return std::make_shared<type>(args); \
  }


#define IMPLEMENT_MAP_CONSTRUCTOR(type) \
  public: type(std::initializer_list<Argument<Char const*>> const &elements) \
  { \
    _MAP_ELEMENT_INITIALIZATION_LOOP \
  } \
  public: static SharedPtr<type> create(std::initializer_list<Argument<Char const*>> const &elements) \
  { \
    return std::make_shared<type>(elements); \
  }


#define IMPLEMENT_ATTR_LIST_CONSTRUCTOR(type) \
  public: type(std::initializer_list<Argument<Char const*>> const &attrs, \
               std::initializer_list<TioSharedPtr> const &elements) \
  { \
    _ATTR_INITIALIZATION_LOOP \
    _LIST_ELEMENT_INITIALIZATION_LOOP \
  } \
  public: static SharedPtr<type> create(std::initializer_list<Argument<Char const*>> const &attrs, \
                                        std::initializer_list<TioSharedPtr> const &elements) \
  { \
    return std::make_shared<type>(attrs, elements); \
  }


#define IMPLEMENT_ATTR_MAP_CONSTRUCTOR(type) \
  public: type(std::initializer_list<Argument<Char const*>> const &attrs, \
               std::initializer_list<Argument<Char const*>> const &elements) \
  { \
    _ATTR_INITIALIZATION_LOOP \
    _MAP_ELEMENT_INITIALIZATION_LOOP \
  } \
  public: static SharedPtr<type> create(std::initializer_list<Argument<Char const*>> const &attrs, \
                                        std::initializer_list<Argument<Char const*>> const &elements) \
  { \
    return std::make_shared<type>(attrs, elements); \
  }

#endif
